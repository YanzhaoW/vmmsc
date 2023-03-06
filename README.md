# Unified VMM3 Slow Control and Calibration Software

# Contents

* [Prerequisites](#prerequisites)
* [Recommended Release](#recommended-release)
* [Requirements](#requirements)
* [Installation](#installation)
  * [Obtain the software](#obtain-the-software)
  * [Compiling the software](#compile-the-software)
  * [Qt](#installing-qt)
* [Contact Information](#contact)


## Prerequisites
The slow control software controls one or several SRS FEC v6 cards or ESS assister cards. If SRS FEC cards are used, the VMM3a hybrids have to be connected to a DVM v5 card. For SRS FECs, the recommended bitfile (fecv6_vmm3_top_21032100.bit) is provided in the slow control repo. When reading the firmware version number with the slow control (button "system parameters"), the version should read "21031200".

A really essential tool that you should install even before installing the slow control is Wireshark. Please install Wireshark with Lua support. In the slow control folder, we provide a lua script, that disassembles the UDP packages from the FEC, and displays the hits from the VMMs. The script vmm3a_plugin.lua is also provided in the slow control repository. Usually we create an alias in .bashrc to start Wireshark with the script:
```
alias essws='wireshark -X lua_script:/path_to_slow_control/vmmsc/vmm3a_plugin.lua'
```

## Recommended Release
The recommended release is **v1.0** which is for *VMM3* slow control and calibration. To obtain this release do:
```
git clone https://bitbucket.org/europeanspallationsource/slow_control_vmm3a.git
```
and follow the installation instructions below.

## Requirements

Here we list the (tested) software requirements.

* [Qt 5.7](https://www.qt.io/qt5-11)
* C++11 (gcc >=4.7)



## Installation
There are a few steps that need to be taken in order for you to obtain, install, and get the software running. These are discussed here.


### Compile the software

First navigate to the new directory. As mentioned above the software can be checked out the bitbucket gitlab repository.
Navigate to the following repository:
```
cd slow_control_vmm3a/build
```
Edit the vmmdcs.pro file and choose the correct clock source and slow control type:
#####################################################
# Define default value for clock source
# 0 = ESS readout, clock via ring (44.02625 MHz)
# 1 = ESS readout, clock from KC705 (44.444 MHz)
# 2 = SRS, ESS firmware with 44.4444 MHz
# 3 = SRS, RD51 firmware with 40 MHz
DEFINES += CLOCK_SOURCE=0

#####################################################
# Define default value for slow control (does not matter for SRS FEC)
# only relevant for ESS assister
# 0 = SRS
# 1 = ESS over master
# 2 = ESS over assister
DEFINES += SLOW_CONTROL=1

make sure you use the correct qmake in case you have several QT versions installed and run qmake:
```
qmake vmmdcs.pro
```
The next step is to compile the package via
```
make clean
make -j10
```
The executable will now be located in
vmmsc/build

To open the slow control on linux use the following command:
```
./vmmdcs
```
On macOS use this command:
```
./vmmdcs.app/Contents/MacOS/vmmdcs
```

 The vmmdcs.pro file is set up to point to the include paths / dependencies based
                on whether the operating system is linux or macOS (mac being the only other considered
                system other than linux).


## Installing Qt

If you do not have Qt on your system or you do, but not a recent enough version as required for the **VMM Software**, then you will
have to install Qt from the main [Qt pages](https://www.qt.io/qt5-11/). 

The full installation procedure of Qt takes less than 1 hour on a machine with adequate internet connection.

Detailed instructions on how to download, setup the installation, and install Qt are described in this set of
[Qt installation slides](https://twiki.cern.ch/twiki/pub/Atlas/NSWVmmDaqSoftware/qt_installation_PDF.pdf). Please
use these instructions to setup Qt for use with **VMM Software**.

## Contact

Questions, comments, suggestions, or help?

For present development, contact:
**Dorothea Pfeiffer**:  <dorothea.pfeiffer@cern.ch>
**Lucian Scharenberg**:  <Lucian.Scharenberg@cern.ch>

