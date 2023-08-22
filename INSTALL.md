# Linux (Ubuntu Linux 22.04)

## Prerequisites

This guide targets Ubuntu Linux 22.04.

This provider relies on OpenCV and other core development tools including gcc. These dependencies can be installed using the following commands:

```bash
sudo apt update
sudo apt install -y git cmake build-essential libopencv-dev qtbase5-dev qt5-qmake
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
cmake -DCMAKE_BUILD_TYPE=Release -DBR_WITH_OPENCV_NONFREE=OFF -DCMAKE_INSTALL_PREFIX=/opt/openbr ..
make
sudo make install
```

### Building the Provider

```bash
git clone git@github.com:mitre/biqt-face
cd biqt-face
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DOPENBR_DIR=/opt/openbr ..
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

# Windows

Windows is no longer a supported platform.
