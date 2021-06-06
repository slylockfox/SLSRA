REM video on cracking after flashing ROM: https://www.youtube.com/watch?v=OiuM35WFkdU
REM from XT1921-2_JAMES_A_CRICKET_8.0.0_OCPS27.91-157-28-12_cid1_subsidy-CRICKET_regulatory-DEFAULT_CFC.xml.zip\flashfile.xml
REM see https://forum.xda-developers.com/t/firmware-guide-moto-e-5th-gen-firmware-flashing-restoration-guide.3820901/
REM see https://www.reddit.com/r/LineageOS/comments/bsat8p/another_moto_z2_force_attempt/
REM see download of ROM: https://mirrors.lolinet.com/firmware/moto/james/official/Cricket/

fastboot getvar max-sparse-size
fastboot oem fb_mode_set
fastboot flash partition gpt.bin
fastboot flash bootloader bootloader.img
fastboot flash modem NON-HLOS.bin
fastboot flash fsg fsg.mbn
fastboot erase modemst1
fastboot erase modemst2
fastboot flash dsp adspso.bin
fastboot flash logo logo.bin
fastboot flash boot boot.img
fastboot flash recovery recovery.img
fastboot flash system system.img_sparsechunk.0
fastboot flash system system.img_sparsechunk.1
fastboot flash system system.img_sparsechunk.2
fastboot flash system system.img_sparsechunk.3
fastboot flash system system.img_sparsechunk.4
fastboot flash vendor vendor.img
fastboot flash  oem oem.img
fastboot erase cache
fastboot erase userdata
fastboot erase DDR
fastboot oem fb_mode_clear
fastboot reboot
