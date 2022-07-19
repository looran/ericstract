#!/bin/sh

trace() { echo "$ $*" >&2; "$@"; }
err() { echo "error: $@"; exit 1; }

EXTRACT_DIR=/tmp/ericstract_test
LOG=/tmp/ericstract_test.log

set -e

do_test() {
	expect_sources=$1
	expect_records=$2
    expect_unknown=$3
    expect_binwalk=$4
	expect_extracted=$5
	up_dir=$6

	echo "=== testing $up_dir"

	trace rm -rf $EXTRACT_DIR
	trace ./ericstract -o $EXTRACT_DIR $up_dir -E > $LOG

	tail -n 13 $LOG > $LOG.sum
	cat $LOG.sum

	[ $(grep "source upgrade files" $LOG.sum |cut -d: -f2) == $expect_sources ]		|| err "bad number of source files"
	[ $(grep "total number of records" $LOG.sum |cut -d: -f2) == $expect_records ]	|| err "bad number of records found"
	[ $(grep "unknown records" $LOG.sum |cut -d: -f2) == $expect_unknown ]	    	|| err "bad number of unknown records"
	[ $(grep "records use binwalk" $LOG.sum |cut -d: -f2) == $expect_binwalk ]	    || err "bad number of binwalk uses"
	[ $(grep "warnings" $LOG.sum |cut -d: -f2) == 0 ]								|| err "warnings found"
	[ $(grep "extracted files" $LOG.sum |cut -d: -f2) == $expect_extracted ]		|| err "invalid number of extracted files"

	echo test ok
}

trace make

# from https://www.4shared.com/rar/8eD9pMRTca/Ericsson.html
#do_test 18 147 0 98 $HOME/doc/telco/ericsson/sw/Ericsson_rar/Ericsson/GSM_BTS_RUS_SW_G16B_R87C_\(OMT_FORMAT\)/
#do_test 18 172 96 121 $HOME/doc/telco/ericsson/sw/Ericsson_rar/Ericsson/GSM_BTS_RUS_SW_G16B_R87C_\(OMT_FORMAT\)/
do_test 18 172 96 100 119 $HOME/doc/telco/ericsson/sw/Ericsson_rar/Ericsson/GSM_BTS_RUS_SW_G16B_R87C_\(OMT_FORMAT\)/

echo all tests OK
