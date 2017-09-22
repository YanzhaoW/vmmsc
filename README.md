# VMM3 Slow Control and Calibration Software

# Contents

* [Recommended Release](#recommended-release)
* [Requirements](#requirements)
* [Installation](#installation)
  * [Obtain the software](#obtain-the-software)
  * [Compiling the software](#compile-the-software)
  * [Qt](#installing-qt)
* [Contact Information](#contact)

## Recommended Release
The recommended release is **v1.0** which is for *VMM3* slow control and calibration. To obtain this release do:
```
git clone https://gitlab.cern.ch/mguth/VMM-software-RD51.git
```
and follow the installation instructions below.

## Requirements

Here we list the (tested) software requirements.

* [Qt 5.7](https://www.qt.io/qt5-7/)
* C++11 (gcc >=4.7)

## Installation
There are a few steps that need to be taken in order for you to obtain, install, and get the software running. These are discussed here.

### Obtain the software

The nominal use case is to check out the current version of the software.
To do this, run the following command from a terminal in a directory where you would like the software to reside:

```
git clone https://gitlab.cern.ch/mguth/VMM-software-RD51.git
```

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
VMM-software-RD51/build

To open the slow control use the following command:
```
./vmmdcs
```

 The vmmdcs.pro file is set up to point to the include paths / dependencies based
                on whether the operating system is linux or macOS (mac being the only other considered
                system other than linux).


## Installing Qt

If you do not have Qt on your system or you do, but not a recent enough version as required for the **VMM Software**, then you will
have to install Qt from the main [Qt pages](https://www.qt.io/qt5-7/). 

The full installation procedure of Qt takes less than 1 hour on a machine with adequate internet connection.

Detailed instructions on how to download, setup the installation, and install Qt are described in this set of
[Qt installation slides](https://twiki.cern.ch/twiki/pub/Atlas/NSWVmmDaqSoftware/qt_installation_PDF.pdf). Please
use these instructions to setup Qt for use with **VMM Software**.

## Contact

Questions, comments, suggestions, or help?

**Manuel Guth**: <manuel.guth@cern.ch>

**Michael Lupberger**:  <michael.lupberger@cern.ch>



