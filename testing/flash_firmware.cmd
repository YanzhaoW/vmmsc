setMode -bs
setMode -bs
setMode -bs
setMode -bs
setCable -port auto
Identify -inferir 
identifyMPM 
attachflash -position 1 -spi "AT45DB161E"
assignfiletoattachedflash -position 1 -file /home/fjaekel/VMM-software-RD51/testing/bit_mcs_files/vmm3h_v1_1.mcs
Program -p 1 -dataWidth 1 -spionly -e -v -loadfpga
quit