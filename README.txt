ericstract - extractor for Ericsson Upgrade Packages in OMT format

Ericsson later moved to .cxp format (tar archives) for it's radio systems firmwares.

compile
~~~~~~~

`make` will build using clang

`make with_gcc` will build using gcc

`make debug` will build using clang and use debugging flags

usage
~~~~~

usage: ericstract [-Elv] [-o <directory>] <upgrade_directory>
extractor for Upgrade Packages in OMT format
-E  do not run binwalk to finish extraction
-o  output directory
-l  only list content, no extraction
-v  verbose logging

dependencies
~~~~~~~~~~~~

libraries
* zlib

binaries
* binwalk

example usage
~~~~~~~~~~~~~

$ ./ericstract -o /tmp/extract /tmp/GSM_BTS_RUS_SW_G16B_R87C_\(OMT_FORMAT\)/
file CPAR77AZ [2974548]
    record CPAR 77AZ BCPU [2974548, 1 part]
        record CPAR 77AZ BCPU [2974476, 1 part]
            archive CPR00001 [2974402, 1 part]
                decompress archive part x00x00x01x04 [2974306]
                    part 0: writing file /tmp/extract/CPAR77AZ_CPAR_BCPU_CPR00001 [11443612]
                    unknown xFFxFFxFFxFF [11443612], no handler found
file CPRR79DZ [2323252]
    record CPRR 79DZ BCPU [2323252, 1 part]
        record CPRR 79DZ BCPU [2323180, 1 part]
            archive CPR00001 [2323107, 1 part]
                decompress archive part x00x00x01x04 [2323011]
                    part 0: writing file /tmp/extract/CPRR79DZ_CPRR_BCPU_CPR00001 [5380288]
                    unknown xFFxFFxFFxFF [5380288], no handler found
file DXPR87CZ [4872000]
    record DXPR 87CZ BDXU [4872000, 46 parts]
        unknown x7Bx0Ax20L FIF=DEST [288732], no handler found
            part 0: writing file /tmp/extract/DXPR87CZ_1-x7Bx0Ax20L_FIF=DEST [288732]
        unknown BIOS BIOS_P_D [28348], no handler found
            part 0: writing file /tmp/extract/DXPR87CZ_2-BIOS_BIOS_P_D [28348]
        unknown BIOS BIOS_G_D [18516], no handler found
            part 0: writing file /tmp/extract/DXPR87CZ_3-BIOS_BIOS_G_D [18516]
        unknown BIOS BIOS_G_E [22668], no handler found
            part 0: writing file /tmp/extract/DXPR87CZ_4-BIOS_BIOS_G_E [22668]
        unknown BIOS BIOS_G1D [18272], no handler found
            part 0: writing file /tmp/extract/DXPR87CZ_5-BIOS_BIOS_G1D [18272]
        unknown BIOS BIOS_G2D [22848], no handler found
            part 0: writing file /tmp/extract/DXPR87CZ_6-BIOS_BIOS_G2D [22848]
        unknown BIOS BIOS_G_G [19512], no handler found
            part 0: writing file /tmp/extract/DXPR87CZ_7-BIOS_BIOS_G_G [19512]
        unknown BIOS BIOS_P_I [26460], no handler found
            part 0: writing file /tmp/extract/DXPR87CZ_8-BIOS_BIOS_P_I [26460]
        unknown BIOS BIOS_G_I [12452], no handler found
            part 0: writing file /tmp/extract/DXPR87CZ_9-BIOS_BIOS_G_I [12452]
        unknown BIOS BIOS_G_B [9780], no handler found
            part 0: writing file /tmp/extract/DXPR87CZ_10-BIOS_BIOS_G_B [9780]
        unknown FLAS H_SHLIB FLBI_P_D [8220], no handler found
            part 0: writing file /tmp/extract/DXPR87CZ_11-FLAS_H_SHLIB_FLBI_P_D [8220]
        unknown FLAS H_SHLIB FLBI_G_D [9044], no handler found
            part 0: writing file /tmp/extract/DXPR87CZ_12-FLAS_H_SHLIB_FLBI_G_D [9044]
        unknown FLAS H_SHLIB FLBI_G_E [9236], no handler found
            part 0: writing file /tmp/extract/DXPR87CZ_13-FLAS_H_SHLIB_FLBI_G_E [9236]
        unknown FLAS H_SHLIB FLBI_G1D [7492], no handler found
            part 0: writing file /tmp/extract/DXPR87CZ_14-FLAS_H_SHLIB_FLBI_G1D [7492]
        unknown FLAS H_SHLIB FLBI_G2D [9120], no handler found
            part 0: writing file /tmp/extract/DXPR87CZ_15-FLAS_H_SHLIB_FLBI_G2D [9120]
        unknown FLAS H_SHLIB FLBI_G_G [7424], no handler found
            part 0: writing file /tmp/extract/DXPR87CZ_16-FLAS_H_SHLIB_FLBI_G_G [7424]
        unknown FLAS H_SHLIB FLBI_P_I [8248], no handler found
            part 0: writing file /tmp/extract/DXPR87CZ_17-FLAS_H_SHLIB_FLBI_P_I [8248]
        unknown FLAS H_SHLIB FLBI_G_I [7440], no handler found
            part 0: writing file /tmp/extract/DXPR87CZ_18-FLAS_H_SHLIB_FLBI_G_I [7440]
        unknown FLAS H_SHLIB FLBI_G_B [9648], no handler found
            part 0: writing file /tmp/extract/DXPR87CZ_19-FLAS_H_SHLIB_FLBI_G_B [9648]
        unknown CFx00x00 [215452], no handler found
            part 0: writing file /tmp/extract/DXPR87CZ_20-CFx00x00 [215452]
        unknown CREC crecp [155968], no handler found
            part 0: writing file /tmp/extract/DXPR87CZ_21-CREC_crecp [155968]
        unknown CREC crec6 [214736], no handler found
            part 0: writing file /tmp/extract/DXPR87CZ_22-CREC_crec6 [214736]
        unknown TRNS C [482336], no handler found
            part 0: writing file /tmp/extract/DXPR87CZ_23-TRNS_C [482336]
        unknown boxa c001 [405760], no handler found
            part 0: writing file /tmp/extract/DXPR87CZ_24-boxa_c001 [405760]
        unknown boxp c001 [2308], no handler found
            part 0: writing file /tmp/extract/DXPR87CZ_25-boxp_c001 [2308]
        unknown dnx5Fp atsy_bin [213064], no handler found
            part 0: writing file /tmp/extract/DXPR87CZ_26-dnx5Fp_atsy_bin [213064]
        unknown dnx5Fs elf_test [213064], no handler found
            part 0: writing file /tmp/extract/DXPR87CZ_27-dnx5Fs_elf_test [213064]
        unknown moni tor_txt [34452], no handler found
            part 0: writing file /tmp/extract/DXPR87CZ_28-moni_tor_txt [34452]
        unknown swlo g_txt [10880], no handler found
            part 0: writing file /tmp/extract/DXPR87CZ_29-swlo_g_txt [10880]
        unknown ECx00x00 ec_dug [164200], no handler found
            part 0: writing file /tmp/extract/DXPR87CZ_30-ECx00x00_ec_dug [164200]
        unknown ECx00x00 ec_mac_p [188440], no handler found
            part 0: writing file /tmp/extract/DXPR87CZ_31-ECx00x00_ec_mac_p [188440]
        unknown ECx00x00 ec_mic_p [20472], no handler found
            part 0: writing file /tmp/extract/DXPR87CZ_32-ECx00x00_ec_mic_p [20472]
        unknown IDBP [216860], no handler found
            part 0: writing file /tmp/extract/DXPR87CZ_33-IDBP [216860]
        unknown LOMP [157588], no handler found
            part 0: writing file /tmp/extract/DXPR87CZ_34-LOMP [157588]
        unknown MPSO [94652], no handler found
            part 0: writing file /tmp/extract/DXPR87CZ_35-MPSO [94652]
        unknown MPSC [12276], no handler found
            part 0: writing file /tmp/extract/DXPR87CZ_36-MPSC [12276]
        unknown MPSS [110540], no handler found
            part 0: writing file /tmp/extract/DXPR87CZ_37-MPSS [110540]
        unknown OMTH [77732], no handler found
            part 0: writing file /tmp/extract/DXPR87CZ_38-OMTH [77732]
        unknown MPSR [37128], no handler found
            part 0: writing file /tmp/extract/DXPR87CZ_39-MPSR [37128]
        unknown RBSx00 [307492], no handler found
            part 0: writing file /tmp/extract/DXPR87CZ_40-RBSx00 [307492]
        unknown MPSM [102284], no handler found
            part 0: writing file /tmp/extract/DXPR87CZ_41-MPSM [102284]
        unknown rxf0 0002 [492], no handler found
            part 0: writing file /tmp/extract/DXPR87CZ_42-rxf0_0002 [492]
        unknown SSCx00 [396832], no handler found
            part 0: writing file /tmp/extract/DXPR87CZ_43-SSCx00 [396832]
        unknown TGCx00 [209520], no handler found
            part 0: writing file /tmp/extract/DXPR87CZ_44-TGCx00 [209520]
        unknown TSCx00 [262116], no handler found
            part 0: writing file /tmp/extract/DXPR87CZ_45-TSCx00 [262116]
file METR87CZ [1652]
    record METR 87CZ BMEU [1652, 4 parts]
        part 0: writing file /tmp/extract/MET_metadata.xml [372]
        part 1: writing file /tmp/extract/MET_metadata-1.xml [372]
        part 2: writing file /tmp/extract/MET_metadata-2.xml [368]
        part 3: writing file /tmp/extract/MET_metadata-3.xml [372]
file N2AR87CZ [5487512]
    record N2AR 87CZ BN2U [5487512, 1 part]
        record N2AR 87CZ BN2U [5487440, 1 part]
            archive N2X0000A [5487368, 1 part]
                decompress archive part x00x00x01x04 [5487272]
                    storing in reassembly list
file N2BR87CZ [5487780]
    record N2BR 87CZ BN2U [5487780, 1 part]
        record N2BR 87CZ BN2U [5487708, 1 part]
            archive N2X0000B [5487634, 1 part]
                decompress archive part x00x00x01x04 [5487538]
                    storing in reassembly list
file N2CR87CZ [5487780]
    record N2CR 87CZ BN2U [5487780, 1 part]
        record N2CR 87CZ BN2U [5487708, 1 part]
            archive N2X0000C [5487634, 1 part]
                decompress archive part x00x00x01x04 [5487538]
                    storing in reassembly list
file N2DR87CZ [5460136]
    record N2DR 87CZ BN2U [5460136, 1 part]
        record N2DR 87CZ BN2U [5460064, 1 part]
            archive N2X0000D [5459990, 1 part]
                decompress archive part x00x00x01x04 [5459894]
                    storing in reassembly list
file RUAR87CZ [6687364]
    record RUAR 87CZ BRUU [6687364, 1 part]
        record RUAR 87CZ BRUU [6687292, 1 part]
            archive RUS0005A [6687218, 1 part]
                decompress archive part x00x00x01x04 [6687122]
                    storing in reassembly list
file RUBR87CZ [3618148]
    record RUBR 87CZ BRUU [3618148, 1 part]
        record RUBR 87CZ BRUU [3618076, 1 part]
            archive RUS0005B [3618001, 1 part]
                decompress archive part x00x00x01x04 [3617905]
                    storing in reassembly list
file RUCR87CZ [4831828]
    record RUCR 87CZ BRUU [4831828, 1 part]
        record RUCR 87CZ BRUU [4831756, 1 part]
            archive RUS0005C [4831684, 1 part]
                decompress archive part x00x00x01x04 [4831588]
                    storing in reassembly list
file RUDR87CZ [4845124]
    record RUDR 87CZ BRUU [4845124, 1 part]
        record RUDR 87CZ BRUU [4845052, 1 part]
            archive RUS0005D [4844978, 1 part]
                decompress archive part x00x00x01x04 [4844882]
                    storing in reassembly list
file RUER87CZ [4870300]
    record RUER 87CZ BRUU [4870300, 1 part]
        record RUER 87CZ BRUU [4870228, 1 part]
            archive RUS0005E [4870154, 1 part]
                decompress archive part x00x00x01x04 [4870058]
                    storing in reassembly list
file RUFR87CZ [3087848]
    record RUFR 87CZ BRUU [3087848, 1 part]
        record RUFR 87CZ BRUU [3087776, 1 part]
            archive RUS0005F [3087704, 1 part]
                decompress archive part x00x00x01x04 [3087608]
                    storing in reassembly list
file RUSR87BZ [8743304]
    record RUSR 87BZ BRUU [8743304, 1 part]
        record RUSR 87BZ BRUU [8743232, 1 part]
            archive RUS00001 [8743160, 1 part]
                decompress archive part x00x00x01x04 [8743064]
                    part 0: writing file /tmp/extract/RUSR87BZ_RUSR_BRUU_RUS00001 [9756234]
                    xplf XPLF CXP9 013268%6 CXP9013268%6           R63CE     [9756232, 8 parts]
                        decompress rpdo RPDO TÅÕ<û#É
                            part 0: writing file /tmp/extract/RUSR87BZ_RUSR_BRUU_RUS00001_CXP9013268%6_R63CE_CXC_132_1797-4_R36AB.rpdo [2064342]
                        decompress rpdo RPDO T×µ÷@
                            part 0: writing file /tmp/extract/RUSR87BZ_RUSR_BRUU_RUS00001_CXP9013268%6_R63CE_1-CXC_112_3669%1R63CE.rpdo [6365142]
                        unknown CXC 112 3631/4 [1398916], no handler found
                            part 0: writing file /tmp/extract/RUSR87BZ_RUSR_BRUU_RUS00001_CXP9013268%6_R63CE_2-CXC_112_3631-4 [1398916]
                        unknown CXC 112 3631/7 [1366176], no handler found
                            part 0: writing file /tmp/extract/RUSR87BZ_RUSR_BRUU_RUS00001_CXP9013268%6_R63CE_3-CXC_112_3631-7 [1366176]
                        unknown CXC 112 4243/2 [2198892], no handler found
                            part 0: writing file /tmp/extract/RUSR87BZ_RUSR_BRUU_RUS00001_CXP9013268%6_R63CE_4-CXC_112_4243-2 [2198892]
                        unknown x01x11x11x11 txl_tbl [133612], no handler found
                            part 0: writing file /tmp/extract/RUSR87BZ_RUSR_BRUU_RUS00001_CXP9013268%6_R63CE_5-x01x11x11x11_txl_tbl [133612]
                        unknown x01x00Sx81 rl r [1236972], no handler found
                            part 0: writing file /tmp/extract/RUSR87BZ_RUSR_BRUU_RUS00001_CXP9013268%6_R63CE_6-x01x00Sx81_rl_r [1236972]
                        lmclist x01x00x00x00 [332]
                            part 0: writing file /tmp/extract/RUSR87BZ_RUSR_BRUU_RUS00001_CXP9013268%6_R63CE_7-lmc_list.xml [328]
file TRPR87CZ [4470436]
    record TRPR 87CZ BTRU [4470436, 32 parts]
        unknown x7Bx0Ax20L FIF=DEST [315504], no handler found
            part 0: writing file /tmp/extract/TRPR87CZ_1-x7Bx0Ax20L_FIF=DEST [315504]
        unknown BIOS BIOS_G_H [44056], no handler found
            part 0: writing file /tmp/extract/TRPR87CZ_2-BIOS_BIOS_G_H [44056]
        unknown BIOS BIOS_P_T [11964], no handler found
            part 0: writing file /tmp/extract/TRPR87CZ_3-BIOS_BIOS_P_T [11964]
        unknown BIOS BIOS_G_T [11496], no handler found
            part 0: writing file /tmp/extract/TRPR87CZ_4-BIOS_BIOS_G_T [11496]
        unknown BIOS BIOS_GXT [45344], no handler found
            part 0: writing file /tmp/extract/TRPR87CZ_5-BIOS_BIOS_GXT [45344]
        unknown FLAS H_SHLIB FLBI_G_H [17940], no handler found
            part 0: writing file /tmp/extract/TRPR87CZ_6-FLAS_H_SHLIB_FLBI_G_H [17940]
        unknown FLAS H_SHLIB FLBI_P_T [5472], no handler found
            part 0: writing file /tmp/extract/TRPR87CZ_7-FLAS_H_SHLIB_FLBI_P_T [5472]
        unknown FLAS H_SHLIB FLBI_G_T [21424], no handler found
            part 0: writing file /tmp/extract/TRPR87CZ_8-FLAS_H_SHLIB_FLBI_G_T [21424]
        unknown cdf0 0001 [490184], no handler found
            part 0: writing file /tmp/extract/TRPR87CZ_9-cdf0_0001 [490184]
        unknown TRNS D [105908], no handler found
            part 0: writing file /tmp/extract/TRPR87CZ_10-TRNS_D [105908]
        unknown edga r_fw [99280], no handler found
            part 0: writing file /tmp/extract/TRPR87CZ_11-edga_r_fw [99280]
        unknown LOMP [157588], no handler found
            part 0: writing file /tmp/extract/TRPR87CZ_12-LOMP [157588]
        unknown LREC lrecp [79732], no handler found
            part 0: writing file /tmp/extract/TRPR87CZ_13-LREC_lrecp [79732]
        unknown LREC lreccdp [157288], no handler found
            part 0: writing file /tmp/extract/TRPR87CZ_14-LREC_lreccdp [157288]
        unknown MPSO [94652], no handler found
            part 0: writing file /tmp/extract/TRPR87CZ_15-MPSO [94652]
        unknown MPSC [12276], no handler found
            part 0: writing file /tmp/extract/TRPR87CZ_16-MPSC [12276]
        unknown MPSS [267192], no handler found
            part 0: writing file /tmp/extract/TRPR87CZ_17-MPSS [267192]
        unknown rcf0 0001 rcf1_tb [151916], no handler found
            part 0: writing file /tmp/extract/TRPR87CZ_18-rcf0_0001_rcf1_tb [151916]
        unknown rcl0 0001 rcl1_tb [33716], no handler found
            part 0: writing file /tmp/extract/TRPR87CZ_19-rcl0_0001_rcl1_tb [33716]
        unknown rcf0 0001 rcf1_tsu [225932], no handler found
            part 0: writing file /tmp/extract/TRPR87CZ_20-rcf0_0001_rcf1_tsu [225932]
        unknown rcl0 0001 rcl1_tsu [33716], no handler found
            part 0: writing file /tmp/extract/TRPR87CZ_21-rcl0_0001_rcl1_tsu [33716]
        unknown rcf0 0001 rcf1_tx [233804], no handler found
            part 0: writing file /tmp/extract/TRPR87CZ_22-rcf0_0001_rcf1_tx [233804]
        unknown rcl0 0001 rcl1_tx [264], no handler found
            part 0: writing file /tmp/extract/TRPR87CZ_23-rcl0_0001_rcl1_tx [264]
        unknown RCx00x00 [20284], no handler found
            part 0: writing file /tmp/extract/TRPR87CZ_24-RCx00x00 [20284]
        unknown RCx5FT RX [701808], no handler found
            part 0: writing file /tmp/extract/TRPR87CZ_25-RCx5FT_RX [701808]
        unknown rxf0 0001 [1384], no handler found
            part 0: writing file /tmp/extract/TRPR87CZ_26-rxf0_0001 [1384]
        unknown TFSS WP [293780], no handler found
            part 0: writing file /tmp/extract/TRPR87CZ_27-TFSS_WP [293780]
        unknown TFSx00 [9664], no handler found
            part 0: writing file /tmp/extract/TRPR87CZ_28-TFSx00 [9664]
        unknown DSPX 0_FW [458392], no handler found
            part 0: writing file /tmp/extract/TRPR87CZ_29-DSPX_0_FW [458392]
        unknown DSPF 1_FW [144184], no handler found
            part 0: writing file /tmp/extract/TRPR87CZ_30-DSPF_1_FW [144184]
        unknown TRXC [201640], no handler found
            part 0: writing file /tmp/extract/TRPR87CZ_31-TRXC [201640]
file UCFR87BZ [34232]
    record UCFR 87BZ BUCU [34232, 1 part]
        part 0: writing file /tmp/extract/UCF_upgrade_control_file.xml [34088]
file ZFJR87CZ [18704]
    zfj ZFJR 87CZ FIF=DEST [18704]
        part 0: writing file /tmp/extract/ZFJ_file_info.txt [18688]
reassembling archive N2X0000A
    concat N2X0000A
    concat N2X0000B
    concat N2X0000C
    concat N2X0000D
    part 0: writing file /tmp/extract/N2AR87CZ_N2AR_BN2U_N2X0000A [21943426]
    xplf XPLF CXP9 013268%1 CXP9013268%15          R63CG     [21943424, 4 parts]
        blob BLOB CXP9029240/1           R63CG    
            part 0: writing file /tmp/extract/N2AR87CZ_N2AR_BN2U_N2X0000A_CXP9013268%15_R63CG_CXP9029240-1_R63CG.blob [18970768]
        blob BLOB t¹ë CXC1739363/1           R1A01    
            part 0: writing file /tmp/extract/N2AR87CZ_N2AR_BN2U_N2X0000A_CXP9013268%15_R63CG_1-CXC1739363-1_R1A01.blob [12144]
        blob BLOB Q¬@ CXC1739373/1           R1A01    
            part 0: writing file /tmp/extract/N2AR87CZ_N2AR_BN2U_N2X0000A_CXP9013268%15_R63CG_2-CXC1739373-1_R1A01.blob [2959804]
        lmclist x01x00x00x00 [332]
            part 0: writing file /tmp/extract/N2AR87CZ_N2AR_BN2U_N2X0000A_CXP9013268%15_R63CG_3-lmc_list.xml [328]
reassembling archive RUS0005A
    concat RUS0005A
    concat RUS0005B
    part 0: writing file /tmp/extract/RUAR87CZ_RUAR_BRUU_RUS0005A [13517134]
    xplf XPLF CXP9 013268%9 CXP9013268%9           R63CG     [13517132, 11 parts]
        decompress rpdo RPDO x'
            part 0: writing file /tmp/extract/RUAR87CZ_RUAR_BRUU_RUS0005A_CXP9013268%9_R63CG_CXC1321797-3_R38AB.rpdo [2158550]
        decompress rpdo RPDO TÕµ÷<È
            part 0: writing file /tmp/extract/RUAR87CZ_RUAR_BRUU_RUS0005A_CXP9013268%9_R63CG_1-CXC_112_3669%2R63CG.rpdo [9289686]
        unknown CXC 173 7545/1 [99040], no handler found
            part 0: writing file /tmp/extract/RUAR87CZ_RUAR_BRUU_RUS0005A_CXP9013268%9_R63CG_2-CXC_173_7545-1 [99040]
        unknown CXC 173 1436/1 [4031272], no handler found
            part 0: writing file /tmp/extract/RUAR87CZ_RUAR_BRUU_RUS0005A_CXP9013268%9_R63CG_3-CXC_173_1436-1 [4031272]
        unknown CXC 112 3979/1 [97364], no handler found
            part 0: writing file /tmp/extract/RUAR87CZ_RUAR_BRUU_RUS0005A_CXP9013268%9_R63CG_4-CXC_112_3979-1 [97364]
        unknown CXC 112 3979/2 [74156], no handler found
            part 0: writing file /tmp/extract/RUAR87CZ_RUAR_BRUU_RUS0005A_CXP9013268%9_R63CG_5-CXC_112_3979-2 [74156]
        unknown CXC 173 5806/3 [43728], no handler found
            part 0: writing file /tmp/extract/RUAR87CZ_RUAR_BRUU_RUS0005A_CXP9013268%9_R63CG_6-CXC_173_5806-3 [43728]
        unknown x01x11x11x11 txl_tbl [133612], no handler found
            part 0: writing file /tmp/extract/RUAR87CZ_RUAR_BRUU_RUS0005A_CXP9013268%9_R63CG_7-x01x11x11x11_txl_tbl [133612]
        unknown CXC 112 4021/1 [860496], no handler found
            part 0: writing file /tmp/extract/RUAR87CZ_RUAR_BRUU_RUS0005A_CXP9013268%9_R63CG_8-CXC_112_4021-1 [860496]
        unknown x01x00Sx81 [3741220], no handler found
            part 0: writing file /tmp/extract/RUAR87CZ_RUAR_BRUU_RUS0005A_CXP9013268%9_R63CG_9-x01x00Sx81 [3741220]
        lmclist x01x00x00x00 [328]
            part 0: writing file /tmp/extract/RUAR87CZ_RUAR_BRUU_RUS0005A_CXP9013268%9_R63CG_10-lmc_list.xml [324]
reassembling archive RUS0005C
    concat RUS0005C
    concat RUS0005D
    concat RUS0005E
    concat RUS0005F
    part 0: writing file /tmp/extract/RUCR87CZ_RUCR_BRUU_RUS0005C [19570870]
    xplf XPLF CXP9 013268%1 CXP9013268%12          R63CG     [19570868, 8 parts]
        decompress rpdo RPDO x'
            part 0: writing file /tmp/extract/RUCR87CZ_RUCR_BRUU_RUS0005C_CXP9013268%12_R63CG_CXC1321797-3_R38AB.rpdo [2158550]
        decompress rpdo RPDO TÕÿ<È
            part 0: writing file /tmp/extract/RUCR87CZ_RUCR_BRUU_RUS0005C_CXP9013268%12_R63CG_1-CXC_112_3669%5R63CG.rpdo [9162710]
        unknown CXC 112 4128/1 [846148], no handler found
            part 0: writing file /tmp/extract/RUCR87CZ_RUCR_BRUU_RUS0005C_CXP9013268%12_R63CG_2-CXC_112_4128-1 [846148]
        unknown CXC 173 1436/1 [4031272], no handler found
            part 0: writing file /tmp/extract/RUCR87CZ_RUCR_BRUU_RUS0005C_CXP9013268%12_R63CG_3-CXC_173_1436-1 [4031272]
        unknown x01x11x11x11 txl_tbl [133612], no handler found
            part 0: writing file /tmp/extract/RUCR87CZ_RUCR_BRUU_RUS0005C_CXP9013268%12_R63CG_4-x01x11x11x11_txl_tbl [133612]
        unknown CXC 173 5846/1 [8053276], no handler found
            part 0: writing file /tmp/extract/RUCR87CZ_RUCR_BRUU_RUS0005C_CXP9013268%12_R63CG_5-CXC_173_5846-1 [8053276]
        unknown x01x11x11x11 H board/sw [2108660], no handler found
            part 0: writing file /tmp/extract/RUCR87CZ_RUCR_BRUU_RUS0005C_CXP9013268%12_R63CG_6-x01x11x11x11_H_board-sw [2108660]
        lmclist x01x00x00x00 [332]
            part 0: writing file /tmp/extract/RUCR87CZ_RUCR_BRUU_RUS0005C_CXP9013268%12_R63CG_7-lmc_list.xml [328]
running binwalk on /tmp/extract/CPAR77AZ_CPAR_BCPU_CPR00001
running binwalk on /tmp/extract/CPRR79DZ_CPRR_BCPU_CPR00001
running binwalk on /tmp/extract/DXPR87CZ_1-x7Bx0Ax20L_FIF=DEST
running binwalk on /tmp/extract/DXPR87CZ_2-BIOS_BIOS_P_D
running binwalk on /tmp/extract/DXPR87CZ_3-BIOS_BIOS_G_D
running binwalk on /tmp/extract/DXPR87CZ_4-BIOS_BIOS_G_E
running binwalk on /tmp/extract/DXPR87CZ_5-BIOS_BIOS_G1D
running binwalk on /tmp/extract/DXPR87CZ_6-BIOS_BIOS_G2D
running binwalk on /tmp/extract/DXPR87CZ_7-BIOS_BIOS_G_G
running binwalk on /tmp/extract/DXPR87CZ_8-BIOS_BIOS_P_I
running binwalk on /tmp/extract/DXPR87CZ_9-BIOS_BIOS_G_I
running binwalk on /tmp/extract/DXPR87CZ_10-BIOS_BIOS_G_B
running binwalk on /tmp/extract/DXPR87CZ_11-FLAS_H_SHLIB_FLBI_P_D
running binwalk on /tmp/extract/DXPR87CZ_12-FLAS_H_SHLIB_FLBI_G_D
running binwalk on /tmp/extract/DXPR87CZ_13-FLAS_H_SHLIB_FLBI_G_E
running binwalk on /tmp/extract/DXPR87CZ_14-FLAS_H_SHLIB_FLBI_G1D
running binwalk on /tmp/extract/DXPR87CZ_15-FLAS_H_SHLIB_FLBI_G2D
running binwalk on /tmp/extract/DXPR87CZ_16-FLAS_H_SHLIB_FLBI_G_G
running binwalk on /tmp/extract/DXPR87CZ_17-FLAS_H_SHLIB_FLBI_P_I
running binwalk on /tmp/extract/DXPR87CZ_18-FLAS_H_SHLIB_FLBI_G_I
running binwalk on /tmp/extract/DXPR87CZ_19-FLAS_H_SHLIB_FLBI_G_B
running binwalk on /tmp/extract/DXPR87CZ_20-CFx00x00
running binwalk on /tmp/extract/DXPR87CZ_21-CREC_crecp
running binwalk on /tmp/extract/DXPR87CZ_22-CREC_crec6
running binwalk on /tmp/extract/DXPR87CZ_23-TRNS_C
running binwalk on /tmp/extract/DXPR87CZ_24-boxa_c001
running binwalk on /tmp/extract/DXPR87CZ_25-boxp_c001
running binwalk on /tmp/extract/DXPR87CZ_26-dnx5Fp_atsy_bin
running binwalk on /tmp/extract/DXPR87CZ_27-dnx5Fs_elf_test
running binwalk on /tmp/extract/DXPR87CZ_28-moni_tor_txt
running binwalk on /tmp/extract/DXPR87CZ_29-swlo_g_txt
running binwalk on /tmp/extract/DXPR87CZ_30-ECx00x00_ec_dug
running binwalk on /tmp/extract/DXPR87CZ_31-ECx00x00_ec_mac_p
running binwalk on /tmp/extract/DXPR87CZ_32-ECx00x00_ec_mic_p
running binwalk on /tmp/extract/DXPR87CZ_33-IDBP
running binwalk on /tmp/extract/DXPR87CZ_34-LOMP
running binwalk on /tmp/extract/DXPR87CZ_35-MPSO
running binwalk on /tmp/extract/DXPR87CZ_36-MPSC
running binwalk on /tmp/extract/DXPR87CZ_37-MPSS
running binwalk on /tmp/extract/DXPR87CZ_38-OMTH
running binwalk on /tmp/extract/DXPR87CZ_39-MPSR
running binwalk on /tmp/extract/DXPR87CZ_40-RBSx00
running binwalk on /tmp/extract/DXPR87CZ_41-MPSM
running binwalk on /tmp/extract/DXPR87CZ_42-rxf0_0002
running binwalk on /tmp/extract/DXPR87CZ_43-SSCx00
running binwalk on /tmp/extract/DXPR87CZ_44-TGCx00
running binwalk on /tmp/extract/DXPR87CZ_45-TSCx00
running binwalk on /tmp/extract/RUSR87BZ_RUSR_BRUU_RUS00001_CXP9013268%6_R63CE_2-CXC_112_3631-4
running binwalk on /tmp/extract/RUSR87BZ_RUSR_BRUU_RUS00001_CXP9013268%6_R63CE_3-CXC_112_3631-7
running binwalk on /tmp/extract/RUSR87BZ_RUSR_BRUU_RUS00001_CXP9013268%6_R63CE_4-CXC_112_4243-2
running binwalk on /tmp/extract/RUSR87BZ_RUSR_BRUU_RUS00001_CXP9013268%6_R63CE_5-x01x11x11x11_txl_tbl
running binwalk on /tmp/extract/RUSR87BZ_RUSR_BRUU_RUS00001_CXP9013268%6_R63CE_6-x01x00Sx81_rl_r
running binwalk on /tmp/extract/RUSR87BZ_RUSR_BRUU_RUS00001
running binwalk on /tmp/extract/TRPR87CZ_1-x7Bx0Ax20L_FIF=DEST
running binwalk on /tmp/extract/TRPR87CZ_2-BIOS_BIOS_G_H
running binwalk on /tmp/extract/TRPR87CZ_3-BIOS_BIOS_P_T
running binwalk on /tmp/extract/TRPR87CZ_4-BIOS_BIOS_G_T
running binwalk on /tmp/extract/TRPR87CZ_5-BIOS_BIOS_GXT
running binwalk on /tmp/extract/TRPR87CZ_6-FLAS_H_SHLIB_FLBI_G_H
running binwalk on /tmp/extract/TRPR87CZ_7-FLAS_H_SHLIB_FLBI_P_T
running binwalk on /tmp/extract/TRPR87CZ_8-FLAS_H_SHLIB_FLBI_G_T
running binwalk on /tmp/extract/TRPR87CZ_9-cdf0_0001
running binwalk on /tmp/extract/TRPR87CZ_10-TRNS_D
running binwalk on /tmp/extract/TRPR87CZ_11-edga_r_fw
running binwalk on /tmp/extract/TRPR87CZ_12-LOMP
running binwalk on /tmp/extract/TRPR87CZ_13-LREC_lrecp
running binwalk on /tmp/extract/TRPR87CZ_14-LREC_lreccdp
running binwalk on /tmp/extract/TRPR87CZ_15-MPSO
running binwalk on /tmp/extract/TRPR87CZ_16-MPSC
running binwalk on /tmp/extract/TRPR87CZ_17-MPSS
running binwalk on /tmp/extract/TRPR87CZ_18-rcf0_0001_rcf1_tb
running binwalk on /tmp/extract/TRPR87CZ_19-rcl0_0001_rcl1_tb
running binwalk on /tmp/extract/TRPR87CZ_20-rcf0_0001_rcf1_tsu
running binwalk on /tmp/extract/TRPR87CZ_21-rcl0_0001_rcl1_tsu
running binwalk on /tmp/extract/TRPR87CZ_22-rcf0_0001_rcf1_tx
running binwalk on /tmp/extract/TRPR87CZ_23-rcl0_0001_rcl1_tx
running binwalk on /tmp/extract/TRPR87CZ_24-RCx00x00
running binwalk on /tmp/extract/TRPR87CZ_25-RCx5FT_RX
running binwalk on /tmp/extract/TRPR87CZ_26-rxf0_0001
running binwalk on /tmp/extract/TRPR87CZ_27-TFSS_WP
running binwalk on /tmp/extract/TRPR87CZ_28-TFSx00
running binwalk on /tmp/extract/TRPR87CZ_29-DSPX_0_FW
running binwalk on /tmp/extract/TRPR87CZ_30-DSPF_1_FW
running binwalk on /tmp/extract/TRPR87CZ_31-TRXC
running binwalk on /tmp/extract/N2AR87CZ_N2AR_BN2U_N2X0000A_CXP9013268%15_R63CG_CXP9029240-1_R63CG.blob
running binwalk on /tmp/extract/N2AR87CZ_N2AR_BN2U_N2X0000A_CXP9013268%15_R63CG_1-CXC1739363-1_R1A01.blob
running binwalk on /tmp/extract/N2AR87CZ_N2AR_BN2U_N2X0000A_CXP9013268%15_R63CG_2-CXC1739373-1_R1A01.blob
running binwalk on /tmp/extract/RUAR87CZ_RUAR_BRUU_RUS0005A_CXP9013268%9_R63CG_2-CXC_173_7545-1
running binwalk on /tmp/extract/RUAR87CZ_RUAR_BRUU_RUS0005A_CXP9013268%9_R63CG_3-CXC_173_1436-1
running binwalk on /tmp/extract/RUAR87CZ_RUAR_BRUU_RUS0005A_CXP9013268%9_R63CG_4-CXC_112_3979-1
running binwalk on /tmp/extract/RUAR87CZ_RUAR_BRUU_RUS0005A_CXP9013268%9_R63CG_5-CXC_112_3979-2
running binwalk on /tmp/extract/RUAR87CZ_RUAR_BRUU_RUS0005A_CXP9013268%9_R63CG_6-CXC_173_5806-3
running binwalk on /tmp/extract/RUAR87CZ_RUAR_BRUU_RUS0005A_CXP9013268%9_R63CG_7-x01x11x11x11_txl_tbl
running binwalk on /tmp/extract/RUAR87CZ_RUAR_BRUU_RUS0005A_CXP9013268%9_R63CG_8-CXC_112_4021-1
running binwalk on /tmp/extract/RUAR87CZ_RUAR_BRUU_RUS0005A_CXP9013268%9_R63CG_9-x01x00Sx81
running binwalk on /tmp/extract/RUCR87CZ_RUCR_BRUU_RUS0005C_CXP9013268%12_R63CG_2-CXC_112_4128-1
running binwalk on /tmp/extract/RUCR87CZ_RUCR_BRUU_RUS0005C_CXP9013268%12_R63CG_3-CXC_173_1436-1
running binwalk on /tmp/extract/RUCR87CZ_RUCR_BRUU_RUS0005C_CXP9013268%12_R63CG_4-x01x11x11x11_txl_tbl
running binwalk on /tmp/extract/RUCR87CZ_RUCR_BRUU_RUS0005C_CXP9013268%12_R63CG_5-CXC_173_5846-1
running binwalk on /tmp/extract/RUCR87CZ_RUCR_BRUU_RUS0005C_CXP9013268%12_R63CG_6-x01x11x11x11_H_board-sw
waiting for 4 binwalk instances to finish
waiting for 3 binwalk instances to finish
waiting for 2 binwalk instances to finish
waiting for 1 binwalk instances to finish

source upgrade files       : 18
skipped files              : 23
total number of records    : 172
unknown records            : 96
records use binwalk        : 100
maximum depth detected     : 6
Upgrade File Info (ZFJ)    : 1
Upgrade Control File (UCF) : 1
Metadata File (MET)        : 4
extracted files            : 119
warnings                   : 0
upgrade directory          : /tmp/GSM_BTS_RUS_SW_G16B_R87C_(OMT_FORMAT)
extract directory          : /tmp/extract
