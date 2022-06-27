ericstract - extractor for Ericsson Upgrade Packages

compile
~~~~~~~

`make` will build using clang

`make with_gcc` will build using gcc

usage
~~~~~

./ericstract [-v] <path_to_upgrade_directory>

dependencies
~~~~~~~~~~~~

* zlib

example usage
~~~~~~~~~~~~~

$ ./ericstract "/tmp/GSM_BTS_RUS_SW_G16B_R87C_(OMT_FORMAT)/"
upgrade_directory: /tmp/GSM_BTS_RUS_SW_G16B_R87C_(OMT_FORMAT)
extract_directory: /tmp/extract
[+] reading files in archives directory
CPAR77AZ [2974548] infered zlib archive
CPRR79DZ [2323252] infered zlib archive
DXPR87CZ [4872000] unknown file type
METR87CZ [1652] found Metadata File
N2AR87CZ [5487512] infered zlib archive
N2BR87CZ [5487780] infered zlib archive
N2CR87CZ [5487780] infered zlib archive
N2DR87CZ [5460136] infered zlib archive
RUAR87CZ [6687364] infered zlib archive
RUBR87CZ [3618148] infered zlib archive
RUCR87CZ [4831828] infered zlib archive
RUDR87CZ [4845124] infered zlib archive
RUER87CZ [4870300] infered zlib archive
RUFR87CZ [3087848] infered zlib archive
RUSR87BZ [8743304] infered zlib archive
TRPR87CZ [4470436] unknown file type
UCFR87BZ [34232] found Upgrade Control File
ZFJR87CZ [18704] found Upgrade File Info
[+] summary of Upgrade files
found 18 Upgrade files
found Upgrade File Info (ZFJ): ZFJR87CZ
found Upgrade Control File (UCF): UCFR87BZ
found Metadata File (MET): METR87CZ
[+] looking for file sequences
[+] unpacking files
extracting CPAR77AZ to CPAR77AZ
extracting CPRR79DZ to CPRR79DZ
extracting DXPR87CZ to DXPR87CZ
extracting METR87CZ to MET_metadata.xml
extracting N2AR87CZ to N2XR87CZ
extracting RUAR87CZ to RUXR87CZ
extracting RUSR87BZ to RUSR87BZ
extracting TRPR87CZ to TRPR87CZ
extracting UCFR87BZ to UCF_upgrade_control_file.xml
extracting ZFJR87CZ to ZFJ_file_info.txt
[*] done, extracted 10 files to /tmp/extract
