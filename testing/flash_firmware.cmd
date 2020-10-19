setMode -bs
setMode -bs
setMode -bs
setMode -bs
setCable -port auto
Identify -inferir 
identifyMPM 
attachflash -position 1 -spi "AT45DB161E"
assignfiletoattachedflash -position 1 -file /please/set/path/vmm3h_v1_1.mcs
Program -p 1 -dataWidth 1 -spionly -e -v -loadfpga
quit