# VMM3 Slow Control and Calibration Software

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
The slow control software controls one or several SRS FEC v6 cards. The VMM3a hybrids have to be connected to a DVM v5 card. The ESS branch of the slow control only works with version 21031200 of the firmware. The recommended bitfiles for the FEC (fecv6_vmm3_top_21032100.bit) and the RD51 VMM3a hybrid (vmm3h_1_020920_20201118.bit) is provided in the slow control repo. When reading the FEC firmware version number with the slow control (button "system parameters"), the version should read "21031200".

A really essential tool that you should install even before installing the slow control is Wireshark. Please install Wireshark with Lua support. In the slow control folder, we provide a lua script, that disassembles the UDP packages from the FEC, and displays the hits from the VMMs. The script vmm3a_plugin.lua is also provided in the slow control repository. Usually we create an alias in .bashrc to start Wireshark with the script:
```
alias essws='wireshark -X lua_script:/path_to_slow_control/vmmsc/vmm3a_plugin.lua'
```

## Recommended Release
The recommended release is **v1.0** which is for *VMM3* slow control and calibration. To obtain this release do:
```
git clone https://gitlab.cern.ch/rd51-slow-control/vmmsc.git
```
and follow the installation instructions below.

## Requirements

Here we list the (tested) software requirements.

* [Qt 5.7](https://www.qt.io/qt5-11)
* C++11 (gcc >=4.7)



## Installation
There are a few steps that need to be taken in order for you to obtain, install, and get the software running. These are discussed here.

### Obtain the software

The nominal use case is to check out the current version of the software.
To do this, run the following command from a terminal in a directory where you would like the software to reside:

```
git clone https://gitlab.cern.ch/rd51-slow-control/vmmsc.git --recursive
```
To use the extensive testing feature, the command above also checks out two other git repositories as submodules.
The firmware files for FEC and hybrid, and the database for the test results can subsequently be found linked inside the testing folder. 

### Compile the software

First navigate to the new directory. As mentioned above the software can be checked out the CERN gitlab repository.
Navigate to the following repository:
```
cd VMM-software-RD51/build
```
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

## Test module

The test module has been written by Finn Jaekel (<s6fijaek@uni-bonn.de>). 
The module writes the test results to a database, and the results can be viewed with the additional tool
[VMM-Database-Browser](https://github.com/FinnJaekel/VMM-Database-Browser).
Please follow the instructions there to install the tool.


## Contact

Questions, comments, suggestions, or help?

Originally developed by:
**Manuel Guth**: <manuel.guth@cern.ch> and **Michael Lupberger**:  <michael.lupberger@cern.ch>

For present development, contact:
**Michael Lupberger**:  <michael.lupberger@cern.ch>
**Dorothea Pfeiffer**:  <dorothea.pfeiffer@cern.ch>
**Lucian Scharenberg**:  <Lucian.Scharenberg@cern.ch>
**Finn Jaekel**: <s6fijaek@uni-bonn.de>

For ESS branch: 
**Dorothea Pfeiffer**:  <dorothea.pfeiffer@cern.ch>
