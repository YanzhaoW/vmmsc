# NSW Electronics - Readout Software

# Contents

* [Recommended Release](#recommended-release)
* [Requirements](#requirements)
* [Installation](#installation)
  * [Obtain the software](#obtain-the-software)
  * [Compiling the software](#compile-the-software)
  * [Run the DAQ](#run-the-daq)
 
  * [On a CERN Linux PC](#installation-on-a-cern-linux-machine)
  * [ROOT](#installing-root)
  * [Qt](#installing-qt)
  * [Boost](#installing-boost)
     * [Step-by-step](#steps-to-install-boost)
* [Useful Links](#useful-links)
* [Contact Information](#contact)

## Recommended Release
The recommended release is **v4.0.0** which is for *VMM2* **AND** *VMM3* readout. To obtain this release do:
```
git clone -b v4.0.0 https://:@gitlab.cern.ch:8443/NSWelectronics/vmm_readout_software.git
```
and follow the installation instructions below.

## Requirements

Here we list the (tested) software requirements.

* [Qt 5.7](https://www.qt.io/qt5-7/)
* ROOT 5.34 (will not work with ROOT >=6)
* Boost 1.60
* C++11 (gcc >=4.7)

## Installation
There are a few steps that need to be taken in order for you to obtain, install, and get the DAQ software running. These are discussed here.

### Obtain the software

The nominal use case is to check out the [recommended tag](#recommended-release) of the software.
To do this, run the following command from a terminal in a directory where you would like the DAQ software to reside:

```
git clone -b <name-of-tag> https://gitlab.cern.ch/NSWelectronics/vmm_readout_software.git
```

where `<name-of-tag>` should be replaced by the desired release name.

### Compile the software

**note** if you do not have gcc>=4.7 the compilation steps outlined here will fail. If you are able (on  a CERN machine or otherwise) you can follow the steps outline in the [next section](#installation-on-a-cern-linux-machine) to obtain the correct gcc.

We next need to compile the libraries for ROOT to handle the ```vector<vector<int>>``` container class (used in the output data ntuples), build a MakeFile for the DAQ software, and compile the software. The first of these is handled by **rootcint** and the second by Qt's **qmake**. The requisite commands to do this are handled by bash script and sets you up to compile the software by calling **make** from within the ```vmm_readout_software/build/``` directory.

**Before doing this** you must first be sure that the  boostlib and boostinclude [(linux)][5]/[(mac)][9] variables are pointing to the correct location relevant to your machine. Edit them accordingly (and [obtain boost](#installing-boost) if needed)  and execute these commands:
```
cd vmm_readout_software/
source install.sh
make 2>&1 |tee vmmdcs_compile.log
```

If there are any errors during compilation that are unable to be resolved, inspect the log file you generated during compilation ("vmmdcs_compile.log") and send it to us and/or report the bug/error.

### Run the DAQ

Once the compilation begun by the call to ```make``` is complete you will have an executable ```vmmdcs``` that can be executed to open up the DAQ GUI. On Linux machines this executable, by default, will be located in ```vmm_readout_software/build/```. On typical MAC machines this executable tends to be located in ```vmm_readout_software/build/vmmdcs.app/Contents/MacOS/```. Whichever the case may be, the DAQ can be opened up by double-clicking on it from a graphical file-browser or by executing it from within a terminal:
```
./<path-to-vmmdcs-executable>/vmmdcs
```
Which will promptly open up the DAQ panel.


## Installation on a CERN Linux Machine

If you are installing the software on a CERN SLC6 machine, it may be the case that the version of gcc is not the correct one. To obtain the requisite version of gcc that is able to compile against C++11 (gcc>=4.7) it is suggested to use the [CERN devtoolset][11] to update your compiler (among other things). We have tested version 1.1 and so here we provide you the commands to obtain this:
```
yum install devtoolset-1.1
scl enable devtoolset-1.1 bash
```

To be sure that you now have the correct version of gcc, run:
```
gcc --version
```
which should tell you that the current version of gcc is 4.7.2.

If, later on or from a new terminal window, you call ```gcc --version``` and the version of gcc reported is not 4.7.2 you either need to call directly
```
scl enable devtoolset-1.1 bash
```
once again or add this line to your ```~/.bashrc```. There may be other options but these are what we have used.

## Installing ROOT

If you do not have ROOT on your system or you do, but not the correct version as required for **vmm_readout_software**, then you
will have to get the ROOT version required from the main [ROOT pages](https://root.cern.ch/).

Downloading and setting up ROOT can take between 5 and 30 minutes on a machine with adequate internet connection.

Detailed instructions on how to download and setup the correct version of ROOT are described in this set of
[ROOT installation slides](https://twiki.cern.ch/twiki/pub/Atlas/NSWVmmDaqSoftware/root_installation_PDF.pdf). Please
use these instructions to setup ROOT for use with **vmm_readout_software** if you do not already have the correct
version of ROOT.

## Installing Qt

If you do not have Qt on your system or you do, but not the correct version as required for **vmm_readout_software**, then you will
have to install Qt from the main [Qt pages](https://www.qt.io/qt5-7/). 

The full installation procedure of Qt takes less than 1 hour on a machine with adequate internet connection.

Detailed instructions on how to download, setup the installation, and install Qt are described in this set of
[Qt installation slides](https://twiki.cern.ch/twiki/pub/Atlas/NSWVmmDaqSoftware/qt_installation_PDF.pdf). Please
use these instructions to setup Qt for use with **vmm_readout_software**.

## Installing Boost

Yes this package requires [Boost][3]. Several uses of Boost are perhaps now replaced by standard C++, but nevertheless, Boost is used.

**The recommended version is Boost 1.60**

### Steps to install Boost

**IMPORTANT NOTE**

It is **highly recommended** to place the installation of Boost, i.e. the directory that you provide as "**path-to-install-directory**" below, in
a location that is near **vmm_readout_software** or at least in a location that you will remember as being for external dependencies of
**vmm_readout_software**. For example, see slide 10 in the [Qt installation slides](https://twiki.cern.ch/twiki/pub/Atlas/NSWVmmDaqSoftware/qt_installation_PDF.pdf).
If you use the directory structure as described on those slides, then it is unambiguous which Boost is being used to compile **vmm_readout_software**.
This requires you to copy the downloaded *tar.gz* file to this install directory and proceeding with steps 1-4 below in *that directory* and using the following
```--prefix``` command:

```
--prefix=${PWD}
```
**Installation steps:**

1.  Obtain the *tar.gz [source][4] for Boost and untar the file.
If you have ```wget``` on your machine (standard for Linux) you can simply do:
```
wget http://sourceforge.net/projects/boost/files/boost/1.60.0/boost_1_60_0.tar.gz
```
to get the *tar.gz file.

2.  Run the following commands to setup the build configuration:
```
cd boost_1_60_0
./bootstrap.sh --with-libraries=atomic,chrono,date_time,exception,filesystem,system,thread,timer --prefix=<install-directory>
```
You should specify a directory for the Boost installation using ```--prefix``` (```--prefix``` tells where to install Boost). In this way, you have control and know exactly where the installation
is happening. As stated above, it is recommended to use ```--prefix=${PWD}``` once you have moved the un-tarred Boost directory to a suitable location.

3.  Run the following commands to install Boost with the configuration set in step 2:
```
./b2 install threading=multi --layout=tagged --prefix=<path-to-install-directory>
```
In Step 2 we tell it only to build the required libraries (most of what we use in Boost is header-only and we only need the few compile-necessary items above) and with this subset of Boost, the install takes O(5) minutes (at least on my personal lap-top).

4.  At the end of the install after Step 3, you should have the following directories:
```
<install-directory>
<install-directory>/lib
```
The first of the above needs to be set as your [boostinclude variable][5] in [vmm_readout_software/build/vmmdcs.pro][6]. The second of the above needs to be set as your [boostlib variable][7] in [vmm_readout_software/build/vmmdcs.pro][6]
**For example**, if we installed boost to the directory ```vmm_daq/boost_1_60/``` under which there *should* be the directory ```boost/``` which contains a bunch of files
ending in ```.hpp``` (these are the header files for boost) and a directory ```lib/```, then in ```vmmdcs.pro``` you should have the lines:

```
    boostinclude=<full-path>/vmm_daq/boost_1_60/
    boostlib=<full-path>/vmm_daq/boost_1_60/lib
```


## Useful links

   * [NSWVmmDaqSoftware twiki][1]
   * [NSWelectronics twiki][2]

## Contact

Questions, comments, suggestions, or help?

**Daniel Antrim**: <daniel.joseph.antrim@cern.ch>

**George Iakovidis**:  <george.iakovidis@cern.ch>




[1]: https://twiki.cern.ch/twiki/bin/viewauth/Atlas/NSWVmmDaqSoftware
[2]: https://twiki.cern.ch/twiki/bin/viewauth/Atlas/NSWelectronics
[3]: http://www.boost.org/
[4]: http://www.boost.org/users/history/version_1_60_0.html
[5]: https://gitlab.cern.ch/NSWelectronics/vmm_readout_software/blob/master/build/vmmdcs.pro#L24
[6]: https://gitlab.cern.ch/NSWelectronics/vmm_readout_software/blob/master/build/vmmdcs.pro
[7]: https://gitlab.cern.ch/NSWelectronics/vmm_readout_software/blob/master/build/vmmdcs.pro#L25
[8]: http://brew.sh/index.html
[9]: https://gitlab.cern.ch/NSWelectronics/vmm_readout_software/blob/master/build/vmmdcs.pro#L31
[10]: https://gitlab.cern.ch/NSWelectronics/vmm_readout_software/blob/master/build/vmmdcs.pro#L30
[11]: http://linux.web.cern.ch/linux/devtoolset/
[12]: https://gitlab.cern.ch/NSWelectronics/vmm_readout_software

