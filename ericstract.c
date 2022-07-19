/*
 * Copyright (c) 2022, Laurent Ghigonis <ooookiwi@gmail.com>
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <err.h>
#include <sys/mman.h>
#include <limits.h>
#include <endian.h>

#include "zlib.h"

/*
 * Upgrade Packages in OMT format consist of multiple files containing each a header followed by content data.
 * The overall upgrade content is often distributed in multiple Upgrade Files, one for each component and version.
 * One Upgrade File contains multiple pieces of data called 'records', that can themselves contain other records.
 * Certain type of records can also be split accross multiple Upgrade Files.
 * The content of a record can be raw or compressed using zlib, and can contain files of various other formats: uImage, cpio, FPGA bitstream, text files...
 * This program extracts, recombine and structure records to properly named files, and then executes binwalk on them.
 * 
 * Headers:
 * There are at least 2 main types of record headers: Normal and XPLF
 * - Normal headers
 * Normal header is described by the 'struct header_rec' is this program.
 * They are found at the start of files and their sub-records.
 * Record headers consist of a fixed-len header followed by an offset table to next records, followed by actual content.
 * Multi-part archives use a special sub-header that includes a name ending with uppercase A for the first archive part,
 * then B and so on. This header is described in 'struct header_archive'.
 * Compressed data uses it's own sub-header inside archives, described by 'struct header_archive_part'.
 * Differentiation of content type is based of the name included in the record header.
 * - XPLF headers
 * XPLF header is described by the 'struct header_xplf'.
 * They are found after decompression of a file record content, to describe another layer of compressed content.
 * Content described by a XPLF header can be compressed using lzma and encapsulated using xz format.
 *
 * CRC:
 * At the end of each record and each file, a 4-byte CRC is present.
 * It is unclear at this point how the CRC is computed (CRC32, ADLER32, maybe something else).
 */

#define CRC_LEN 4 * sizeof(uint8_t)
#define HEADER_XPLF_NAME_LEN 32
#define HEADER_ARCHIVE_NAME_LEN 8

struct __attribute__((__packed__)) header_rec {
	char	 name[8];				/* record name, matches file name if first header */
	uint32_t size;					/* content length, from header start */
	uint32_t type;					/* ascii */
	uint32_t unknown1;
	uint32_t unknown2;				/* FFFFFFFF */
	uint32_t two;
	uint32_t unknown3;
	char	 id[8];					/* ascii, repeated between records */
	uint32_t multirec_unknown1;		/* FFFFFFFF or 00030000 on multirecord file */
	uint32_t multirec_unknown2;		/* 00000000 or 000D80D8 on multirecord file */
	uint32_t zero;
	uint32_t multirec_unknown3;		/* 00000000 or 02000000 on multirecord file */
	uint32_t records_count;			/* number of records in this file */
	uint32_t first_block_not_indexed; /* 1 when first block is not in record offsets table */
	/* next is records offset table, relative to this header's start,
	 * followed by first record (header or directly content) */
};

struct __attribute__((__packed__)) header_archive {
	char	 name[8];				/* ends with [A-Z] in case of multi-part archive */
	uint32_t zero1[6];
	uint32_t one;
	uint32_t zero2[2];
	uint32_t size;					/* content length, from header start */
	uint32_t records_count;
	/* next is records offset table, relative to this header's start,
	 * followed by compressed record header (see struct header_compressed_rec).
	 * a gap can be found instead, indicated by a number of dword to skip. */
};

struct __attribute__((__packed__)) header_archive_part {
	uint32_t magic;
	uint32_t content_size;
	uint32_t unknown2;
	uint32_t decompressed_size;
	uint32_t zero[3];
	/* next is compressed content */
};

struct __attribute__((__packed__)) header_xplf {
	uint32_t type;					/* ascii XPLF */
	uint32_t unknown1;				/* 1 for XPLF, 808A46 for BLOB */
	uint32_t separator1;			/* 0000FFFF */
	char	 name[32];				/* ascii */
	uint32_t unknown2;
	uint32_t unknown3;
	uint32_t size;
	uint32_t unknown4;
	uint32_t separator2;			/* FFFFFFFF */
	uint32_t separator3;			/* FFFFFFFF */
	uint32_t records_count;
	/* next is records offset table, relative to this header's start,
	 * followed by first record (header or directly content) */
};

struct __attribute__((__packed__)) header_blob {
	uint32_t type;					/* ascii */
	uint32_t unknown1;
	uint32_t header_len;			/* header length starting now */
	uint32_t unknown2[3];
	char name[32];
	uint32_t unknown3;
	uint32_t zero[8];
};

struct __attribute__((__packed__)) header_rpdo {
	uint32_t type;					/* ascii */
	uint32_t unknown1;
	uint16_t unknown2;
};

/*
 * This program parses Upgrade File recursively and stores all records in 'struct record'.
 */

#define REC_CHILD_MAX 255
#define REC_DEPTH_MAX 25
#define REC_REASSEMBLY_MAX 255
#define REC_BINWALK_MAX 1024
#define Z_CHUNK_SIZE 262144

struct record {
	uint8_t *ptr;
	size_t size;
	unsigned int depth;
	char *filename;
	char *out_filename;			/* part of the filename, to be concatenated from other record filenames in the tree */
	char *out_filename_full;	/* filename as created on disk */
	const char *out_fileext;
	int part;
	struct record *parent;
	struct record *childs[REC_CHILD_MAX];
	unsigned int childs_count;
	struct { /* decoded from record header */
		uint32_t size;
		uint32_t type;
		uint32_t records_count;
		uint32_t *offsets;
		char *name;
	} h;
	struct { /* archive extract and reassembly */
		uint8_t *buf;
		size_t size;
		struct record *seq_next;
		struct record *seq_prev;
	} extract;
};

enum extract_res {
	EXTRACT_FAILED_NO_HANDLER = 0,
	EXTRACT_FAILED_NOT_IMPLEMENTED,
	EXTRACT_FAILED_DEPTH_MAX_REACHED,
	EXTRACT_FAILED_DECOMPRESSION,
	EXTRACT_DONE,
	EXTRACT_PARTS_RECORDS,
	EXTRACT_PARTS_DUMP,
	EXTRACT_USE_BINWALK,
};

enum rec_type {
	REC_RAW = 0,
	REC_NORMAL,
	REC_ARCHIVE,
	REC_ARCHIVE_PART,
	REC_XPLF,
	REC_BLOB,
	REC_RPDO,
	REC_VEP,
};

/*
 * records extract handlers */
struct magic {
	char *magic;		/* first byte of header */
	char *type;			/* for REC_NORMAL only, 4th byte */
	enum rec_type rec;	/* record type */
	enum extract_res (*handler)(struct record *); /* handler function */
};

/* global configuration */
static struct conf {
	int no_binwalk;
	char *extract_dir_base;
	int only_list;
	int verbose;
} conf;

/* global statistics */
static struct stats {
	int records_count;
	int unknown_records;
	unsigned int max_depth;
	int extract_ok;
	int extract_errors;
	struct record *ucf;
	struct record *met;
	struct record *zfj;
	unsigned int warnings;
} stats;

/* records set for reassembly */
static struct reassembly {
	struct record *recs[REC_REASSEMBLY_MAX];
	size_t count;
} reassembly;

/* records set for extraction using binwalk */
static struct binwalk {
	struct record *recs[REC_BINWALK_MAX];
	int pids[REC_BINWALK_MAX];
	int status[REC_BINWALK_MAX];
	size_t count;
	size_t running;
} binwalk;

void usageexit(void);
enum extract_res rec_extract(struct record *, unsigned int);
enum extract_res rec_extract_new(struct record *, int, uint8_t *, size_t, unsigned int);
enum extract_res rec_handler_zfj(struct record *);
enum extract_res rec_handler_ucf(struct record *);
enum extract_res rec_handler_met(struct record *);
enum extract_res rec_handler_decapsulate(struct record *);
enum extract_res rec_handler_xplf(struct record *);
enum extract_res rec_handler_blob(struct record *);
enum extract_res rec_handler_rpdo(struct record *);
enum extract_res rec_handler_lmclist(struct record *);
enum extract_res rec_handler_archive(struct record *);
enum extract_res rec_handler_archive_part(struct record *);
enum extract_res rec_handler_raw(struct record *);
char *rec_header_ascii(struct record *);
void rec_out_filename(struct record *, const char *, size_t, const char *);
void rec_write(struct record *, unsigned int, uint8_t *, size_t);
void rec_free(struct record *);
uint8_t *z_inflate(uint8_t *, size_t, size_t *);
void sigchld_binwalk(int);
char *indent(int);
void xwarnx(char *fmt, ...);
void info(unsigned int, char *fmt, ...);
void verb(unsigned int, char *fmt, ...);
void *xmalloc(size_t);
char *ascii(uint8_t *, int);

static struct magic magics[] = {
	/* magic,	type,	rec,			handler */
	{ "XPLF",			NULL,	REC_XPLF,		rec_handler_xplf },
	{ "ZFJR",			NULL,	REC_RAW,		rec_handler_zfj },
	{ "TRPR",			NULL,	REC_NORMAL,		rec_handler_raw },
	{ NULL,				"BN2U",	REC_NORMAL,		rec_handler_decapsulate },
	{ NULL,				"BRUU",	REC_NORMAL,		rec_handler_decapsulate },
	{ NULL,				"BCPU",	REC_NORMAL,		rec_handler_decapsulate },
	{ NULL,				"BDXU",	REC_NORMAL,		rec_handler_raw },
	{ NULL,				"BTRU",	REC_NORMAL,		rec_handler_decapsulate },
	{ "\0\0\x01\x04",	NULL,	REC_ARCHIVE_PART, rec_handler_archive_part },
	{ "UCFR",			NULL,	REC_NORMAL,		rec_handler_ucf },
	{ "METR",			NULL,	REC_NORMAL,		rec_handler_met },
	{ "N2X0",			NULL,	REC_ARCHIVE,	rec_handler_archive },
	{ "RUS0",			NULL,	REC_ARCHIVE,	rec_handler_archive },
	{ "BLOB",			NULL,	REC_BLOB,		rec_handler_blob },
	{ "RPDO",			NULL,	REC_RPDO,		rec_handler_rpdo },
	{ "\x01\0\0\0",		NULL,	REC_RAW,		rec_handler_lmclist },
	//{ "\x3D\x60\x00\x01",NULL,	REC_NORMAL,		rec_handler_raw },
	{ "CPR0",			NULL,	REC_ARCHIVE,	rec_handler_archive },
	//{ "DXPR",			NULL,	REC_NORMAL,		rec_handler_raw },
	{ "VEP\0",			NULL,	REC_VEP,		rec_handler_raw },
	{ NULL,				NULL,	REC_RAW,		NULL },
};

__attribute__((__noreturn__)) void
usageexit(void)
{
	printf("usage: ericstract [-Elv] [-o <directory>] <upgrade_directory>\n");
	printf("extractor for Upgrade Packages in OMT format\n");
	printf("-E  do not run binwalk to finish extraction\n");
	printf("-o  output directory\n");
	printf("-l  only list content, no extraction\n");
	printf("-v  verbose logging\n");
	exit(1);
}

int
main(int argc, char **argv)
{
	char *upgrade_dir, *extract_dir_base = NULL;
	DIR *dir;
	struct stat fstat;
	struct dirent *de;
	struct record *rec;
	struct record *records_root[REC_CHILD_MAX];
	unsigned int source_files = 0, skipped_files = 0, n;
	struct header_rec *h;
	uint8_t *ptr;
	int ch;

	bzero(&conf, sizeof(conf));
	bzero(&stats, sizeof(stats));
	bzero(&reassembly, sizeof(reassembly));

	while ((ch = getopt(argc, argv, "Eo:lv")) != -1) {
		switch (ch) {
			case 'E':
				conf.no_binwalk = 1;
				break;
			case 'o':
				extract_dir_base = optarg;
				break;
			case 'l':
				conf.only_list = 1;
				break;
			case 'v':
				conf.verbose++;
				break;
			default:
				usageexit();
		}
	}
	argc -= optind;
	argv += optind;
	if (argc < 1)
		usageexit();

	upgrade_dir = realpath(argv[0], NULL);
	if (!upgrade_dir)
		errx(1, "upgrade directory does not exist");
	if (!extract_dir_base)
		extract_dir_base = "extract";
	if (stat(extract_dir_base, &fstat) == -1 && !conf.only_list) {
		mkdir(extract_dir_base, 0700);
	}
	conf.extract_dir_base = realpath(extract_dir_base, NULL);

	dir = opendir(upgrade_dir);
	if (!dir)
		errx(1, "could not open directory");

	verb(0, "[+] reading files in upgrade directory\n");

	while ((de = readdir(dir)) != NULL) {
		int f;
		char filepath[PATH_MAX];

		if (!strcmp(de->d_name, ".") || !strcmp(de->d_name, ".."))
			continue;
		snprintf(filepath, sizeof(filepath), "%s/%s", upgrade_dir, de->d_name);
		if (stat(filepath, &fstat) == -1) {
			xwarnx("could not stat file, skipping: %s\n", de->d_name);
			skipped_files++;
			continue;
		}
		if ((fstat.st_mode & S_IFMT) != S_IFREG) {
			info(0, "not a regular file, skipping: %s\n", de->d_name);
			skipped_files++;
			continue;
		}
		if (fstat.st_size < sizeof(struct header_rec)) {
			xwarnx("file too small, skipping: %s\n", de->d_name);
			skipped_files++;
			continue;
		}
		f = open(filepath, O_RDONLY);
		if (!f) {
			xwarnx("could not open file: %s, skipping\n", de->d_name);
			skipped_files++;
			continue;
		}
		ptr = mmap(0, fstat.st_size, PROT_READ, MAP_PRIVATE, f, 0);
		if (!ptr) {
			xwarnx("could not mmap file, skipping: %s\n", de->d_name);
			close(f);
			skipped_files++;
			continue;
		}
		h = (struct header_rec *)ptr;
		if (strncmp(h->name, de->d_name, 8)) {
			info(0, "not an Upgrade File, skipping: %s\n", de->d_name);
			close(f);
			skipped_files++;
			continue;
		}
		if (be32toh(h->size) != fstat.st_size) {
			xwarnx("file size (%ld) different from header->size (%u), skipping: %s\n", fstat.st_size, be32toh(h->size), de->d_name);
			close(f);
			skipped_files++;
			continue;
		}

		rec = xmalloc(sizeof(struct record));
		rec->ptr = ptr;
		rec->filename = strdup(de->d_name);
		rec->out_filename = strdup(de->d_name);
		rec->size = fstat.st_size;

		records_root[source_files] = rec;
		source_files++;
		close(f);
	}
	closedir(dir);

	verb(0, "[+] %s records\n", (conf.only_list) ? "listing" : "extracting");
	for (n=0; n<source_files; n++) {
		rec = records_root[n];
		info(0, "file %s [%li]\n", rec->filename, rec->size);
		rec_extract(rec, 1);
	}

	if (reassembly.count > 0) {
		struct record *rec2;
		uint8_t *buf;
		unsigned int n2, buf_size;

		verb(0, "[+] looking for sequences for reassembly in %d records\n", reassembly.count);
		for (n=0; n<reassembly.count; n++) {
			rec = reassembly.recs[n];
			for (n2=0; n2<reassembly.count; n2++) {
				if (n2 == n)
					continue;
				rec2 = reassembly.recs[n2];
				if (!strncmp(rec2->parent->h.name, rec->parent->h.name, 7)
						&& (rec2->parent->h.name[7] == rec->parent->h.name[7] + 1)
						&& (strcmp((char *)rec2->extract.buf, "XPLF") != 0)) {
					/* archive name start by the same 7 letters
					 * and filename 8th letter is +1 (like in B=A+1), mark as next in sequence
					 * and the content does not start by an XPLF header */
					verb(1, "file sequence detected: %s is followed by %s\n", rec->parent->h.name, rec2->parent->h.name);
					rec2->extract.seq_prev = rec;
					rec->extract.seq_next = rec2;
					break;
				}
			}
		}

		verb(0, "[+] performing reassembly from sequences start\n");
		for (n=0; n<reassembly.count; n++) {
			rec = reassembly.recs[n];
			if (rec->extract.seq_prev)
				continue;
			if (!rec->extract.seq_next) {
				xwarnx("reassembly: orphaned archive found: %s\n", rec->parent->h.name);
			}
			info(0, "reassembling archive %s\n", rec->parent->h.name);
			rec->depth = 0;
			rec_out_filename(rec, rec->parent->h.name, HEADER_ARCHIVE_NAME_LEN, NULL);
			buf = NULL;
			buf_size = 0;
			while (rec) {
				info(1, "concat %s\n", rec->parent->h.name);
				buf_size += rec->extract.size;
				buf = realloc(buf, buf_size);
				memcpy((buf+buf_size) - rec->extract.size, rec->extract.buf, rec->extract.size);
				rec = rec->extract.seq_next;
			}
			rec = reassembly.recs[n];
			rec_write(rec, 0, buf, buf_size);
			rec_extract_new(rec, 0, buf, buf_size, 1);
			free(buf);
		}
	}

	if (!conf.only_list && !conf.no_binwalk && binwalk.count > 0) {
		int pid, fd;
		char path[PATH_MAX], log[PATH_MAX];
		unsigned int tasks = (sysconf(_SC_NPROCESSORS_ONLN) / 2) + 1;
		sigset_t wait_sigchld;

		signal(SIGCHLD, sigchld_binwalk);
		sigfillset(&wait_sigchld);
		sigdelset(&wait_sigchld, SIGCHLD);

		verb(0, "[+] running binwalk on %d files using %d parallel tasks\n", binwalk.count, tasks);
		for (n=0; n<binwalk.count; n++) {
			rec = binwalk.recs[n];
			snprintf(path, sizeof(path), "%s/%s", extract_dir_base, rec->out_filename_full);
			snprintf(log, sizeof(log), "%s/_%s.binwalk.log", extract_dir_base, rec->out_filename_full);
			pid = fork();
			if (pid < 0) {
				xwarnx("could not fork: %s\n", strerror(errno));
			} else if (pid > 0) {
				info(0, "running binwalk on %s\n", path);
				binwalk.pids[n] = pid;
				binwalk.running++;
			} else {
				chdir(extract_dir_base);
				fd = open(log, O_WRONLY | O_CREAT, 0600);
				dup2(fd, 1);
				dup2(fd, 2);
				execlp("binwalk", "binwalk", "-eMv", path, NULL);
				perror("exec binwalk failed:");
				exit(0);
			}
			if (binwalk.running >= tasks)
				sigsuspend(&wait_sigchld);
		}
		while (binwalk.running > 0) {
			info(0, "waiting for %d binwalk instances to finish\n", binwalk.running);
			sigsuspend(&wait_sigchld); // race condition is possible, we could wait forever if last binwalk process just terminated
		}
		for (n=0; n<binwalk.count; n++) {
			if (binwalk.status[n] != 0) {
				xwarnx("binwalk exited with error %d on %s\n", binwalk.status[n], binwalk.recs[n]->out_filename_full);
			}
		}
	}

	printf("\nsource upgrade files       : %d\n", source_files);
	printf("skipped files              : %d\n", skipped_files);
	printf("total number of records    : %d\n", stats.records_count);
	printf("unknown records            : %d\n", stats.unknown_records);
	printf("records use binwalk        : %lu\n", binwalk.count);
	printf("maximum depth detected     : %u\n", stats.max_depth);
	printf("Upgrade File Info (ZFJ)    : %d\n", stats.zfj ? stats.zfj->h.records_count : 0);
	printf("Upgrade Control File (UCF) : %d\n", stats.ucf ? stats.ucf->h.records_count : 0);
	printf("Metadata File (MET)        : %d\n", stats.met ? stats.met->h.records_count : 0);
	printf("extracted files            : %d\n", stats.extract_ok);
	printf("warnings                   : %d\n", stats.warnings);
	printf("upgrade directory          : %s\n", upgrade_dir);
	printf("extract directory          : %s\n", extract_dir_base);

	if (!conf.only_list)
		verb(0, "[*] done, extracted %d files to %s\n", stats.extract_ok, extract_dir_base);

	for (n=0; n<source_files; n++) {
		rec_free(records_root[n]);
	}
	free(upgrade_dir);
	free(conf.extract_dir_base);

	return 0;
}

/*
 * rec_extract - extract a record to a file on disk
 */
enum extract_res
rec_extract(struct record *rec, unsigned int depth)
{
	enum extract_res extract_res = EXTRACT_FAILED_NO_HANDLER;
	struct header_rec *h = (struct header_rec *)rec->ptr;
	struct header_archive *ha = (struct header_archive *)rec->ptr;
	struct header_archive_part *hap = (struct header_archive_part *)rec->ptr;
	struct header_xplf *hx = (struct header_xplf *)rec->ptr;
	struct header_blob *hb = (struct header_blob *)rec->ptr;
	uint32_t magic = be32toh(((uint32_t *)rec->ptr)[0]);
	uint32_t type = be32toh(h->type);
	struct magic *m;
	uint8_t *part;
	size_t part_size;
	unsigned int n;

	stats.records_count++;
	rec->depth = depth;
	if (depth > stats.max_depth)
		stats.max_depth = depth;
	if (depth > REC_DEPTH_MAX)
		return EXTRACT_FAILED_DEPTH_MAX_REACHED;

	/* call handler based on magic or type */
	for (n=0; magics[n].magic || magics[n].type; n++) {
		m = &magics[n];
		if ((m->magic && magic == be32toh(*(uint32_t *)m->magic))
				|| (m->type && type == be32toh(*(uint32_t *)m->type))) {
			switch (m->rec) {
			case REC_NORMAL:
				rec->h.name = h->name;
				rec->h.size = be32toh(h->size);
				rec->h.type = be32toh(h->type);
				rec->h.records_count = be32toh(h->records_count);
				rec->h.offsets = (uint32_t *)(rec->ptr + sizeof(struct header_rec));
				info(depth, "record %s [%d, %d %s]\n", rec_header_ascii(rec), rec->h.size, rec->h.records_count, rec->h.records_count == 1 ? "part" : "parts");
				break;
			case REC_ARCHIVE:
				rec->h.size = be32toh(ha->size);
				rec->h.name = h->name;
				rec->h.records_count = be32toh(ha->records_count);
				rec->h.offsets = (uint32_t *)(rec->ptr + sizeof(struct header_archive));
				info(depth, "archive %s [%d, %d %s]\n", rec->ptr, rec->h.size, rec->h.records_count, rec->h.records_count == 1 ? "part" : "parts");
				break;
			case REC_ARCHIVE_PART:
				rec->h.size = be32toh(hap->content_size);
				info(depth, "decompress archive part %s [%d]\n", rec_header_ascii(rec), rec->h.size);
				break;
			case REC_XPLF:
				rec->h.size = be32toh(hx->size);
				rec->h.records_count = be32toh(hx->records_count);
				rec->h.name = hx->name;
				rec->h.offsets = (uint32_t *)(rec->ptr + sizeof(struct header_xplf));
				info(depth, "xplf %s %.*s [%d, %d %s]\n", rec_header_ascii(rec), HEADER_XPLF_NAME_LEN, rec->h.name, rec->h.size, rec->h.records_count, rec->h.records_count == 1 ? "part" : "parts");
				break;
			case REC_BLOB:
				rec->h.name = hb->name;
				info(depth, "blob %s %.*s\n", rec_header_ascii(rec), HEADER_XPLF_NAME_LEN, rec->h.name);
				break;
			case REC_RPDO:
				rec->h.name = hb->name;
				info(depth, "decompress rpdo %s\n", rec_header_ascii(rec));
				break;
			case REC_RAW:
				rec->h.size = rec->size;
				rec->h.records_count = 1;
				break;
			case REC_VEP:
				break;
			}
			extract_res = m->handler(rec);
			break;
		}
	}

	switch (extract_res) {
	case EXTRACT_FAILED_NO_HANDLER:
		info(depth, "unknown %s [%d], no handler found\n", rec_header_ascii(rec), rec->size);
		stats.unknown_records++;
		if (!rec->parent || !rec->parent->out_filename_full) {
			rec_out_filename(rec, rec_header_ascii(rec), 0, NULL);
			rec_write(rec, 0, rec->ptr, rec->size);
			binwalk.recs[binwalk.count] = rec;
			binwalk.count++;
		}
		break;

	case EXTRACT_FAILED_NOT_IMPLEMENTED:
		info(depth+1, "extraction not implemented\n");
		extract_res = EXTRACT_DONE;
		break;

	case EXTRACT_FAILED_DEPTH_MAX_REACHED:
		info(depth+1, "depth max limit reached\n");
		extract_res = EXTRACT_DONE;
		break;

	case EXTRACT_FAILED_DECOMPRESSION:
		info(depth+1, "decompression failed\n");
		extract_res = EXTRACT_DONE;
		break;

	case EXTRACT_PARTS_RECORDS:
	case EXTRACT_PARTS_DUMP:
		for (n=0; n < rec->h.records_count; n++) {
			part = rec->ptr + be32toh(rec->h.offsets[n]);
			if (n == rec->h.records_count - 1)
				part_size = rec->h.size - be32toh(rec->h.offsets[n]) - CRC_LEN * 2;
			else
				part_size = be32toh(rec->h.offsets[n+1]) - be32toh(rec->h.offsets[n]);
			if (extract_res == EXTRACT_PARTS_RECORDS)
				rec_extract_new(rec, n, part, part_size, depth+1);
			else
				rec_write(rec, n, part, part_size);
		}
		extract_res = EXTRACT_DONE;
		break;

	case EXTRACT_USE_BINWALK:
		binwalk.recs[binwalk.count] = rec;
		binwalk.count++;
		break;

	case EXTRACT_DONE:
		break;
	}

	return extract_res;
}

enum extract_res
rec_extract_new(struct record *rec, int part, uint8_t *ptr, size_t size, unsigned int depth)
{
	struct record *new;

	new = xmalloc(sizeof(struct record));
	new->ptr = ptr;
	new->size = size;
	new->parent = rec;
	new->part = part;
	rec->childs[rec->childs_count] = new;
	rec->childs_count++;

	return rec_extract(new, depth);
}

enum extract_res
rec_handler_zfj(struct record *rec)
{
	struct header_rec *h = (struct header_rec *)rec->ptr;
	uint32_t size = be32toh(h->size);

	info(rec->depth, "zfj %s [%d]\n", rec_header_ascii(rec), rec->h.size);
	rec_out_filename(rec, "ZFJ_file_info", 0, "txt");
	rec_write(rec, 0, rec->ptr + 3*sizeof(uint32_t), size - 3*sizeof(uint32_t) - CRC_LEN);
	stats.zfj = rec;

	return EXTRACT_DONE;
}

enum extract_res
rec_handler_ucf(struct record *rec)
{
	rec_out_filename(rec, "UCF_upgrade_control_file", 0, "xml");
	stats.ucf = rec;

	return EXTRACT_PARTS_DUMP;
}

enum extract_res
rec_handler_met(struct record *rec)
{
	rec_out_filename(rec, "MET_metadata", 0, "xml");
	stats.met = rec;

	return EXTRACT_PARTS_DUMP;
}

enum extract_res
rec_handler_decapsulate(struct record *rec)
{
	char buf[4+1+4+1];
	struct header_rec *h = (struct header_rec *)rec->ptr;
	uint8_t *next = rec->ptr + sizeof(struct header_rec) + rec->h.records_count * sizeof(uint32_t);
	uint32_t next_size = rec->size - (sizeof(struct header_rec) + rec->h.records_count * sizeof(uint32_t));

	if (rec->parent) {
		snprintf(buf, sizeof(buf), "%.4s_%.4s", h->name, (char *)&h->type);
		rec_out_filename(rec, buf, 0, NULL);
	}

	return rec_extract_new(rec, -1, next, next_size, rec->depth+1);
}

enum extract_res
rec_handler_xplf(struct record *rec)
{
	struct header_xplf *h = (struct header_xplf *)rec->ptr;

	rec_out_filename(rec, h->name, HEADER_XPLF_NAME_LEN, NULL);

	return EXTRACT_PARTS_RECORDS;
}

enum extract_res
rec_handler_blob(struct record *rec)
{
	struct header_blob *h = (struct header_blob *)rec->ptr;

	rec_out_filename(rec, h->name, HEADER_XPLF_NAME_LEN, "blob");
	rec_write(rec, 0, rec->ptr + sizeof(struct header_blob), rec->size - sizeof(struct header_blob));

	return EXTRACT_USE_BINWALK;
}

enum extract_res
rec_handler_rpdo(struct record *rec)
{
	uint8_t *z_beg = rec->ptr + sizeof(struct header_rpdo);
	uint8_t *z_end = rec->ptr + rec->size - 4;
	uint8_t *name_limit = rec->ptr + rec->size - 64;
	size_t z_len = z_end - z_beg, size;
	uint8_t *p, *buf;
	char *name = NULL;

	for (p=z_end; p >= name_limit; p--) {
		if (p[0] == 0x0 && p[1] == 0x0 && p[2] == 0x43 && p[3] == 0x58) {
			name = (char *)(p+2);
			break;
		}
	}
	verb(rec->depth+1, "RPDO z_beg=%p %02x%02x z_end=%p %02x%02x z_len=%lu name=%s\n", z_beg, z_beg[0], z_beg[1], z_end, z_end[0], z_end[1], z_len, name);

	if (name)
		rec_out_filename(rec, name, HEADER_XPLF_NAME_LEN, "rpdo");
	buf = z_inflate(z_beg, z_len, &size);
	if (buf) {
		rec_write(rec, 0, buf, size);
		free(buf);
	}

	return EXTRACT_DONE;
}

enum extract_res
rec_handler_lmclist(struct record *rec)
{
	info(rec->depth, "lmclist %s [%d]\n", rec_header_ascii(rec), rec->h.size);
	rec_out_filename(rec, "lmc_list", 0, "xml");
	rec_write(rec, 0, rec->ptr + sizeof(uint32_t), rec->size - CRC_LEN);

	return EXTRACT_DONE;
}

enum extract_res
rec_handler_archive(struct record *rec)
{
	return EXTRACT_PARTS_RECORDS;
}

enum extract_res
rec_handler_archive_part(struct record *rec)
{
	size_t uncompressed_size_expected, uncompressed_size_result, z_len;
	struct header_archive_part *h = (struct header_archive_part *)rec->ptr;
	uint8_t *z_begin, *z_end, *buf;

	z_len = be32toh(h->content_size);
	uncompressed_size_expected = be32toh(h->decompressed_size);
	
	z_begin = rec->ptr + sizeof(struct header_archive_part);
	z_end = z_begin + z_len;
	verb(rec->depth, "uncompress zbeg=%x zbeg+1=%x zend=%x zlen=%zu uncompressed_size_expected=%zu\n", *z_begin, *(z_begin+1), *z_end, z_len, uncompressed_size_expected);

	buf = z_inflate(z_begin, z_len, &uncompressed_size_result);
	if (uncompressed_size_result != uncompressed_size_expected) {
		xwarnx("uncompressed size %zd != from expected uncompressed size %zd\n", uncompressed_size_result, uncompressed_size_expected);
	}

	rec->extract.buf = buf;
	rec->extract.size = uncompressed_size_result;
	verb(rec->depth, "archive_part head %s\n", ascii(buf, 32));

	if (rec->parent->h.name[7] >= 'A' && rec->parent->h.name[7] <= 'Z') {
		info(rec->depth+1, "storing in reassembly list\n");
		reassembly.recs[reassembly.count] = rec;
		reassembly.count++;
		return EXTRACT_DONE;
	} else {
		rec_out_filename(rec, rec->parent->h.name, HEADER_ARCHIVE_NAME_LEN, NULL);
		rec_write(rec, 0, rec->extract.buf, rec->extract.size);
		rec_extract_new(rec, -1, rec->extract.buf, rec->extract.size, rec->depth+1);
		return EXTRACT_USE_BINWALK;
	}
}

enum extract_res
rec_handler_raw(struct record *rec)
{
	//return EXTRACT_FAILED_NOT_IMPLEMENTED;
	return EXTRACT_PARTS_RECORDS;
}

/* use some empiric rules to get a printable name from a header's start */
char *
rec_header_ascii(struct record *rec)
{
	static char buf[255];
	char *p = buf;
	size_t len;

	if (*(uint32_t *)rec->ptr == 0x11111101 && !rec->ptr[4]) {
		/* read name from wide characters */
		char *s = (char *)rec->ptr+5;
		while ((*p++ = *s))
			s += 2;
	} else {
		/* convert header to ascii */
		strcpy(buf, ascii(rec->ptr, 4));
		p += strlen(buf);
	}
	if (isalnum(rec->ptr[4])) {
		/* more characters */
		*p++ = ' ';
		len = strnlen((char *)rec->ptr+4, 8);
		strncpy(p, (char *)rec->ptr+4, len);
		p += len;
	}
	if (isalnum(rec->ptr[12])) {
		/* more characters */
		*p++ = ' ';
		len = strnlen((char *)rec->ptr+12, 4);
		strncpy(p, (char *)rec->ptr+12, len);
		p += len;
	}
	if (isalnum(rec->ptr[16])) {
		/* more characters */
		*p++ = ' ';
		len = strnlen((char *)rec->ptr+16, 8);
		strncpy(p, (char *)rec->ptr+16, len);
		p += len;
	}
	*p = '\0';

	return buf;
}

void
rec_write(struct record *rec, unsigned int n, uint8_t *start, size_t size)
{
	struct record *rec2;
	char out_filepath[PATH_MAX], num[4];
	unsigned int out_filepath_len, len;
	FILE *f;

	if (conf.only_list)
		return;

	/* build file name by concatenating parent records out_filename, separated by "_" */
	rec2 = rec;
	out_filepath_len = 0;
	do {
		if (rec2->out_filename) {
			len = strlen(rec2->out_filename);
			if (out_filepath_len + len + 1 >= sizeof(out_filepath)) {
				xwarnx("rec_write: path too long: %d + %d\n", out_filepath_len, len);
				break;
			}
			if (out_filepath_len > 0) {
				memmove(out_filepath+len+1, out_filepath, out_filepath_len+1);
			}
			strcpy(out_filepath, rec2->out_filename);
			if (out_filepath_len > 0) {
				out_filepath[len] = '_';
				len++;
			}
			out_filepath_len += len;
			if (rec2->part > 0) {
				len = snprintf(num, sizeof(num), "%d", rec2->part);
				if (out_filepath_len > 0)
					memmove(out_filepath+len+1, out_filepath, out_filepath_len+1);
				strcpy(out_filepath, num);
				out_filepath[len] = '-';
				out_filepath_len += len + 1;
			}
		}
		//verb(rec->depth, "rec_write filename %s = %s [%d]\n", out_filepath, ascii((uint8_t *)out_filepath, out_filepath_len), out_filepath_len);
	} while ((rec2 = rec2->parent) != NULL);

	/* append part number and extension, if available */
	if (n > 0) {
		out_filepath_len += snprintf(num, sizeof(num), "-%d", n);
		strcat(out_filepath, num);
	}
	if (rec->out_fileext) {
		strcat(out_filepath, ".");
		strcat(out_filepath, rec->out_fileext);
		out_filepath_len += strlen(rec->out_fileext) + 1;
	}
	verb(rec->depth+1, "rec_write path %s\n", out_filepath);

	/* save full filename in the record */
	if (rec->out_filename_full)
		free(rec->out_filename_full);
	rec->out_filename_full = strdup(out_filepath);

	/* prepend extract_dir_base directory */
	len = strlen(conf.extract_dir_base);
	memmove(out_filepath+len+1, out_filepath, out_filepath_len+1);
	strcpy(out_filepath, conf.extract_dir_base);
	out_filepath[len] = '/';

	info(rec->depth+1, "part %d: writing file %s [%lu]\n", n, out_filepath, size);

	/* write the file */
	f = fopen(out_filepath, "w");
	if (!f) {
		warn("error writing file");
		stats.extract_errors++;
		return;
	}
	fwrite(start, size, 1, f);
    fclose(f);

	stats.extract_ok++;
}

void
rec_free(struct record *rec)
{
	if (rec->filename)
		free(rec->filename);
	if (rec->out_filename)
		free(rec->out_filename);
	if (rec->out_filename_full)
		free(rec->out_filename_full);
	if (rec->size && !rec->parent) {
		/* no parent means file based */
		munmap(rec->ptr, rec->size);
	}
	if (rec->extract.buf)
		free(rec->extract.buf);
	if (rec->childs_count > 0) {
		unsigned int n;

		for (n=0; n < rec->childs_count; n++) {
			rec_free(rec->childs[n]);
		}
	}
	free(rec);
}

/*
 * filename: string that can be freed
 * ext: static string
 */
void
rec_out_filename(struct record *rec, const char *filename, size_t filename_max, const char *ext)
{
	char buf[NAME_MAX];
	char *p = buf, *s = buf;

	if (rec->out_filename)
		free(rec->out_filename);

	if (filename_max == 0)
		filename_max = strlen(filename);
	/* format filename: replace '/' by '-' and remove spaces */
	strncpy(buf, filename, filename_max);
	buf[filename_max] = '\0';
	do {
		while (*p == ' ' && *(p+1) == ' ')
			++p;
		if (*p == '/')
			*p = '-';
		else if (*p == ' ') {
			if (!*(p+1))
				++p;
			else
				*p = '_';
		}
	} while ((*s++ = *p++));
	*(s-1) = '\0';

	verb(rec->depth+1, "filename %s ext %s\n", buf, ext);
	rec->out_filename = strdup(buf);
	rec->out_fileext = ext;
}

uint8_t *
z_inflate(uint8_t *in, size_t in_size, size_t *out_size)
{
	uint8_t *buf = NULL, *in_p = in;
	size_t size = 0, alloc_size = 0;
	z_stream strm;
	int res;

	bzero(&strm, sizeof(strm));
	if (inflateInit(&strm) != Z_OK)
        return NULL;

	do {
		strm.avail_in = (in + in_size) - in_p;
		if (strm.avail_in <= 0)
            break;
		strm.next_in = in_p;
		do {
			alloc_size += Z_CHUNK_SIZE;
			buf = realloc(buf, alloc_size);
			strm.avail_out = Z_CHUNK_SIZE;
			strm.next_out = buf + alloc_size - Z_CHUNK_SIZE;
			res = inflate(&strm, Z_NO_FLUSH);
			if (res != Z_OK && res != Z_STREAM_END) {
				xwarnx("z_inflate decompression failed, error %d\n", res);
				inflateEnd(&strm);
				free(buf);
				return NULL;
			}
			size += Z_CHUNK_SIZE - strm.avail_out;
			verb(0, "z_inflate in_size=%d strm.avail_in=%d size=%d\n", in_size, strm.avail_in, size);
		} while (strm.avail_out == 0); /* no more output */
		in_p += Z_CHUNK_SIZE;
	} while (res != Z_STREAM_END); /* done when inflate() says it's done */

	inflateEnd(&strm);
	*out_size = size;
	return buf;
}

void
sigchld_binwalk(int sig)
{
	int pid;
	int status;
	int n = 0;

	pid = wait(&status);
	while (binwalk.pids[n] != pid)
		n++;
	binwalk.status[n] = status;
	binwalk.running--;
}

char *
indent(int depth)
{
	static char buf[REC_DEPTH_MAX*4];
	
	memset(buf, ' ', depth*4);
	buf[depth*4] = '\0';
	return buf;
}

void
info(unsigned int depth, char *fmt, ...)
{
	va_list argp;

	printf("%s", indent(depth));
	va_start(argp, fmt);
	vprintf(fmt, argp);
	va_end(argp);
}


void
xwarnx(char *fmt, ...)
{
	va_list argp;

	stats.warnings++;
	printf("warning: ");
	va_start(argp, fmt);
	vprintf(fmt, argp);
	va_end(argp);
}

void
verb(unsigned int depth, char *fmt, ...)
{
	va_list argp;

	if (conf.verbose == 0)
		return;

	printf("%sVERB ", indent(depth));
	va_start(argp, fmt);
	vprintf(fmt, argp);
	va_end(argp);
}

void *
xmalloc(size_t size)
{
	void *p;

	p = malloc(size);
	if (!p)
		err(1, "malloc");
	bzero(p, size);
	return p;
}

char *
ascii(uint8_t *ptr, int len)
{
	static char buf[255];
	char *p = buf;
	int n;

	for (n=0; n<len; n++) {
		if (isalnum(*(ptr + n)))
			p += sprintf(p, "%c", *(ptr + n));
		else
			p += sprintf(p, "x%02X", *(ptr + n));
	}

	return buf;
}

