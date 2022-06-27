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
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <err.h>
#include <sys/mman.h>
#include <limits.h>

#include "zlib.h"

#define UF_MAX 255
#define OFFSET_ZLIB_START 0xE8
#define OFFSET_ZLIB_UNCOMPRESSED_LEN 0xD8
#define CRC_LEN 4

enum uf_type {
	UF_UNKNOWN = 0,
	UF_UCF,
	UF_ZFJ,
	UF_MET,
	UF_ZLIB,
};

/* structure for Upgrade File */
struct uf_entry {
	enum uf_type type;
	char *filename;
	char *out_filename;
	size_t size;
	uint8_t *ptr;
	uint32_t *ptr32;
	struct uf_entry *seq_next;
	struct uf_entry *seq_prev;
};

static struct conf {
	int verbose;
} conf;

static struct stats {
	int extracted_files;
} stats;

void usageexit(void);
int uf_extract(struct uf_entry *, char *);
void uf_free(struct uf_entry *);

__attribute__((__noreturn__)) void
usageexit(void) {
	printf("usage: ericstract [-C] <upgrade_directory>\n");
	printf("extractor for Ericsson Upgrade Packages\n");
	printf("-v verbose\n");
	exit(1);
}

int
main(int argc, char **argv) {
	char *upgrade_dir, *extract_dir;
	DIR *dir;
	struct stat fstat;
	struct dirent *de;
	struct uf_entry *uf, *uf2;
	struct uf_entry *ufs[UF_MAX];
	unsigned int n, n2;
	unsigned int ufs_count = 0;
	struct uf_entry *ucf = NULL;
	struct uf_entry *met = NULL;
	struct uf_entry *zfj = NULL;
	int ch;

	while ((ch = getopt(argc, argv, "v")) != -1) {
		switch (ch) {
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
	printf("upgrade_directory: %s\n", upgrade_dir);
	extract_dir = "extract";
	if (stat(extract_dir, &fstat) == -1) {
		mkdir(extract_dir, 0700);
	}
	extract_dir = realpath(extract_dir, NULL);
	printf("extract_directory: %s\n", extract_dir);

	dir = opendir(upgrade_dir);
	if (!dir)
		errx(1, "could not open directory");

	printf("[+] reading files in archives directory\n");

	while ((de = readdir(dir)) != NULL) {
		int f;
		char filepath[NAME_MAX];

		if (!strcmp(de->d_name, ".") || !strcmp(de->d_name, ".."))
			continue;
		snprintf(filepath, sizeof(filepath), "%s/%s", upgrade_dir, de->d_name);
		if (stat(filepath, &fstat) == -1) {
			perror(filepath);
			continue;
		}
		if ((fstat.st_mode & S_IFMT) != S_IFREG) {
			warnx("not a regular file, skipping: %s", de->d_name);
			continue;
		}
		if (fstat.st_size < 0xEB) {
			warnx("file too small: %s", de->d_name);
			continue;
		}
		f = open(filepath, O_RDONLY);
		if (!f) {
			warnx("could not open file: %s", de->d_name);
			continue;
		}

		uf = malloc(sizeof(struct uf_entry));
		bzero(uf, sizeof(struct uf_entry));
		uf->filename = strdup(de->d_name);
		uf->size = (size_t) fstat.st_size;

		uf->ptr = mmap(0, uf->size, PROT_READ, MAP_PRIVATE, f, 0);
		uf->ptr32 = (uint32_t *)uf->ptr;
		if (!uf->ptr) {
			warnx("could not mmap file: %s", de->d_name);
			close(f);
			uf_free(uf);
			continue;
		}
		if (strncmp((char *)uf->ptr, de->d_name, 8)) {
			warnx("not an Ericsson Upgrade file, skipping: %s", de->d_name);
			close(f);
			uf_free(uf);
			continue;
		}
		printf("%s [%zu] ", de->d_name, uf->size);
		if (!strncmp(de->d_name, "ZFJ", 3)) {
			printf("found Upgrade File Info\n");
			uf->type = UF_ZFJ;
			uf->out_filename = strdup("ZFJ_file_info.txt");
			zfj = uf;
		} else if (!strncmp(de->d_name, "UCF", 3)) {
			printf("found Upgrade Control File\n");
			uf->type = UF_UCF;
			uf->out_filename = strdup("UCF_upgrade_control_file.xml");
			ucf = uf;
		} else if (!strncmp(de->d_name, "MET", 3)) {
			printf("found Metadata File\n");
			uf->type = UF_MET;
			uf->out_filename = strdup("MET_metadata.xml");
			met = uf;
		} else if (uf->ptr[OFFSET_ZLIB_START] == 0x78 && uf->ptr[OFFSET_ZLIB_START+1] == 0x9C) {
			printf("infered zlib archive\n");
			uf->type = UF_ZLIB;
		} else {
			printf("unknown file type\n");
			uf->type = UF_UNKNOWN;
		}

		ufs[ufs_count] = uf;
		ufs_count++;
		close(f);
	}
	closedir(dir);

	printf("[+] summary of Upgrade files\n");
	printf("found %d Upgrade files\n", ufs_count);
	printf("found Upgrade File Info (ZFJ): %s\n", zfj ? zfj->filename : "no");
	printf("found Upgrade Control File (UCF): %s\n", ucf ? ucf->filename : "no");
	printf("found Metadata File (MET): %s\n", met ? met->filename : "no");

	printf("[+] looking for file sequences\n");
	for (n=0; n<ufs_count; n++) {
		uf = ufs[n];
		for (n2=0; n2<ufs_count; n2++) {
			if (n2 == n)
				continue;
			uf2 = ufs[n2];
			if(!strncmp(uf2->filename, uf->filename, 2)) {
				/* filenames start by the same 2 letters */
				if (uf2->filename[2] == uf->filename[2] + 1) {
					/* filename 3rd letter is +1 (like in B=A+1), mark as sequence */
					uf2->seq_prev = uf;
					uf->seq_next = uf2;
					break;
				}
			}
		}
	}

	printf("[+] unpacking files\n");

	for (n=0; n<ufs_count; n++) {
		uf = ufs[n];

		if (uf->seq_prev)
			continue;
		uf_extract(uf, extract_dir);
	}

	printf("[*] done, extracted %d files to %s\n", stats.extracted_files, extract_dir);

	for (n=0; n<ufs_count; n++) {
		uf_free(ufs[n]);
	}
	free(upgrade_dir);
	free(extract_dir);

	return 0;
}

int
uf_extract(struct uf_entry *uf, char *extract_dir) {
	char out_filepath[NAME_MAX];
	FILE *f;
	size_t out_len, uncompressed_size_result;
	unsigned char *buf = NULL;
	size_t buf_size = 0;
	uint8_t *z_begin, *z_end;
	unsigned long z_len;
	int res;
	struct uf_entry *cuf;

	if (!uf->out_filename) {
		if (uf->seq_next) {
			snprintf(out_filepath, sizeof(out_filepath), "%c%cX%s", uf->filename[0], uf->filename[1], &uf->filename[3]);
			uf->out_filename = strdup(out_filepath);
		} else {
			uf->out_filename = strdup(uf->filename);
		}
	}
	printf("extracting %s to %s\n", uf->filename, uf->out_filename);
	snprintf(out_filepath, NAME_MAX, "%s/%s", extract_dir, uf->out_filename);
	f = fopen(out_filepath, "w");

	switch (uf->type) {
	case UF_ZFJ:
		fwrite(&uf->ptr32[3], uf->size - (3*4) - CRC_LEN, 1, f);
		break;
	case UF_UCF:
		fwrite(&uf->ptr32[34], uf->size-(34*4) - 4 - CRC_LEN, 1, f);
		break;
	case UF_MET:
		fwrite(&uf->ptr32[40], uf->size-(40*4) - 4 - CRC_LEN, 1, f);
		break;
	case UF_ZLIB:
		cuf = uf;
		while (cuf) {
			z_begin = &cuf->ptr[OFFSET_ZLIB_START];
			z_end = &cuf->ptr[cuf->size - CRC_LEN];
			z_len = (unsigned long)(z_end-z_begin);
			out_len = ntohl(*(uint32_t *)(cuf->ptr + OFFSET_ZLIB_UNCOMPRESSED_LEN));
			buf_size += out_len;
			buf = realloc(buf, buf_size);
			uncompressed_size_result = out_len;

			if (conf.verbose > 0) {
				printf("uncompress: zbeg=%x zbeg+1=%x zend=%x zlen=%zu size=%zu uncomsize=%zu\n", *z_begin, *(z_begin+1), *z_end, z_len, cuf->size, out_len);
			}
			res = uncompress((buf+buf_size)-out_len, &uncompressed_size_result, z_begin, z_len);
			if (res != Z_OK) {
				warn("error while uncompressing content of file %s: %d", cuf->filename, res);
				cuf = cuf->seq_next;
				free(buf);
				continue;
			}
			if (uncompressed_size_result != out_len) {
				warnx("uncompressed size %zd != from expected uncompressed size %zd", uncompressed_size_result, out_len);
			}

			cuf = cuf->seq_next;
		}
		fwrite(buf, buf_size, 1, f);
		free(buf);
		break;
	case UF_UNKNOWN:
		fwrite(uf->ptr, uf->size, 1, f);
		break;
	}

    fclose(f);
	stats.extracted_files++;

	return 0;
}

void
uf_free(struct uf_entry *uf) {
	if (uf->filename)
		free(uf->filename);
	if (uf->out_filename)
		free(uf->out_filename);
	if (uf->ptr) {
		munmap(uf->ptr, uf->size);
	}
	free(uf);
}

