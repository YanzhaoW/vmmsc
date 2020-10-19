#Source XILINX script with settings
source /opt/Xilinx/14.7/ISE_DS/settings64.sh 
export LC_NUMERIC=en_US.UTF-8
#Add correct path
LD_PRELOAD=/opt/Xilinx/usb-driver/libusb-driver.so
export LD_PRELOAD
impact -batch $(pwd)/../testing/cleancablelock.cmd
impact -batch $(pwd)/../testing/flash_firmware.cmd
