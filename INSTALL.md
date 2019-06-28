# Linux (CentOS Linux 7.4)

## Prerequisites

This guide targets CentOS Linux 7.4.

This provider relies on OpenCV and other core development tools including gcc. These dependencies can be installed from 
the [Extra Packages for Enterprise Linux (EPEL)](https://fedoraproject.org/wiki/EPEL#How_can_I_use_these_extra_packages.3F) 
repository using the following commands:

```bash
sudo yum groupinstall "Development Tools"
sudo yum install -y cmake3 opencv-devel opencv qt5-qtbase qt5-qtbase-devel
```

## Building and Installing

### Building and Installing the OpenBR Dependency

The following steps clone the OpenBR project repository, build the project's source code, and install the 
resulting executables, libraries, configuration files, and resources to `/opt/openbr`.

```bash
git clone https://github.com/biometrics/openbr.git
cd openbr
git submodule init
git submodule update
mkdir build
cd build
cmake3 -DCMAKE_BUILD_TYPE=Release -DBR_WITH_OPENCV_NONFREE=OFF -DCMAKE_INSTALL_PREFIX=/opt/openbr ..
make
sudo make install
```

### Building the Provider

```bash
git clone git@github.com:mitre/biqt-face
cd biqt-face
mkdir build
cd build
cmake3 -DCMAKE_BUILD_TYPE=Release -DOPENBR_DIR=/opt/openbr ..
make
sudo make install
```

## Running the Provider

After installation, you can call the provider using the reference BIQT CLI as follows:

```
# Runs all face providers (including this one).
$>biqt -m "face" <image path>

# Runs only the biqt-face provider.
$>biqt -p "BIQTFace" <image path>
```

# Windows 10

## Prerequisites

This guide targets Microsoft Windows 10 64-bit.

The following tools are required to build the biqt-face provider from source. When installing CMake and Git,
please select the option to add the executables to the system's `PATH` environment variable.
  * [CMake 3.9+](https://cmake.org/files/v3.9/cmake-3.9.2-win64-x64.msi)
  * [Git 2.16+](https://git-scm.com/)
  * [Microsoft Visual Studio Community Edition 2013](https://www.visualstudio.com/vs/older-downloads/) (Registration required.)
  * [OpenCV 2.4.12 Win pack](https://opencv.org/releases.html)
  * [Qt 5.4.2 for MSVC2013 x64](https://download.qt.io/archive/qt/5.4/5.4.2/)
  
This guide assumes that OpenCV 2.4.12 is installed to `C:\opencv` and Qt 5.4.2 is installed to `C:\Qt5.4.2`.

Further, this guide assumes that BIQT has been installed and that the `%BIQT_HOME%` environment variable is set.

## Building and Installing

As an administrator, execute the following commands from the VS2013 x64 Cross Tools Command Prompt. By default, a shortcut to 
this prompt can be found by browsing to `C:\Program Files (x86)\Microsoft Visual Studio 12.0\Common7\Tools\Shortcuts`. Right-click 
on the shortcut and select 'Run as administrator' from the popup menu.

**Note:** Using the VS2012 x64 Cross Tools Command Prompt Start Menu shortcut will not work!

### Building and Installing the OpenBR Dependency

The following steps clone the OpenBR project repository, build the project's source code, and install the 
resulting executables, libraries, configuration files, and resources to `C:\openbr`.

```
git clone https://github.com/biometrics/openbr.git
cd openbr
git submodule init
git submodule update
mkdir build
cd build
cmake.exe -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=C:/openbr -DBR_WITH_OPENCV_NONFREE=OFF -DOpenCV_DIR=C:/opencv/build -DCMAKE_PREFIX_PATH=C:/Qt5.4.2/5.4/msvc2013_64 ..
nmake
nmake install
```

### Building the Provider

Again as an administrator, execute the following commands from the VS2013 x64 Cross Tools Command Prompt.

```
git clone git@github.com:mitre/biqt-face
cd biqt-face
mkdir build
cmake -G "NMake Makefiles" -DOpenCV_DIR=C:/opencv -DOPENBR_DIR=C:/openbr -DCMAKE_PREFIX_PATH=C:/Qt5.4.2/5.4/msvc2013_64 -DCMAKE_BUILD_TYPE=Release ..
nmake
nmake install
```

The provider will be installed into `C:\Program Files\biqt\providers`.

## Running the Provider

After installation, you can call the provider using the reference BIQT CLI as follows:

```
# Runs all face providers (including this one).
C:\Users\user>biqt -m "face" <image path>

# Runs only the biqt-face provider.
C:\Users\user>biqt -p "BIQTFace" <image path>
```
