                                       ‫بسم الله الرَّحْمَنِ الرَّحِيمِ

# Residue C++ Client
Official C++ client library with feature-rich API to interact with residue seamlessly. It uses [Easylogging++](https://github.com/muflihun/easyloggingpp) to interact and take input from the user. Easylogging++ is highly efficient, well-tested library.

It is developed separately because of different license

[![Build Status](https://img.shields.io/travis/muflihun/residue-cpp/master.svg)](https://travis-ci.org/muflihun/residue-cpp) (`master`)

[![Build Status](https://img.shields.io/travis/muflihun/residue-cpp/develop.svg)](https://travis-ci.org/muflihun/residue-cpp) (`develop`)

[![Version](https://img.shields.io/github/release/muflihun/residue.svg)](https://github.com/muflihun/residue/releases/latest)

[![Documentation](https://img.shields.io/badge/docs-doxygen-blue.svg)](https://muflihun.github.io/residue)

[![GitHub license](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://github.com/muflihun/residue-cpp/blob/master/LICENCE)

[![Donate](https://img.shields.io/badge/Donate-PayPal-green.svg)](https://www.paypal.me/MuflihunDotCom/25)

## Build Matrix

| Branch | Platform | Build Status |
| -------- |:------------:|:------------:|
| `develop` | GNU/Linux 4.4 / Ubuntu 4.8.4 64-bit / `clang++` | [![Build Status](https://travis-matrix-badges.herokuapp.com/repos/muflihun/residue-cpp/branches/develop/1)](https://travis-ci.org/muflihun/residue-cpp) |
| `develop` | GNU/Linux 4.4 / Ubuntu 4.8.4 64-bit / `g++-4.9` | [![Build Status](https://travis-matrix-badges.herokuapp.com/repos/muflihun/residue-cpp/branches/develop/2)](https://travis-ci.org/muflihun/residue-cpp) |
| `develop` | GNU/Linux 4.4 / Ubuntu 4.8.4 64-bit / `g++-5` | [![Build Status](https://travis-matrix-badges.herokuapp.com/repos/muflihun/residue-cpp/branches/develop/3)](https://travis-ci.org/muflihun/residue-cpp) |
| `develop` | GNU/Linux 4.4 / Ubuntu 4.8.4 64-bit / `g++-6` | [![Build Status](https://travis-matrix-badges.herokuapp.com`/repos/muflihun/residue-cpp/`branches/develop/4)](https://travis-ci.org/muflihun/residue-cpp) |
| `develop` | GNU/Linux 4.4 / Ubuntu 4.8.4 64-bit / `g++-7` | [![Build Status](https://travis-matrix-badges.herokuapp.com/repos/muflihun/residue-cpp/branches/develop/5)](https://travis-ci.org/muflihun/residue-cpp) |
| `master` | GNU/Linux 4.4 / Ubuntu 4.8.4 64-bit / `clang++` | [![Build Status](https://travis-matrix-badges.herokuapp.com/repos/muflihun/residue-cpp/branches/master/1)](https://travis-ci.org/muflihun/residue-cpp) |
| `master` | GNU/Linux 4.4 / Ubuntu 4.8.4 64-bit / `g++-4.9` | [![Build Status](https://travis-matrix-badges.herokuapp.com/repos/muflihun/residue-cpp/branches/master/2)](https://travis-ci.org/muflihun/residue-cpp) |
| `master` | GNU/Linux 4.4 / Ubuntu 4.8.4 64-bit / `g++-5` | [![Build Status](https://travis-matrix-badges.herokuapp.com/repos/muflihun/residue-cpp/branches/master/3)](https://travis-ci.org/muflihun/residue-cpp) |
| `master` | GNU/Linux 4.4 / Ubuntu 4.8.4 64-bit / `g++-6` | [![Build Status](https://travis-matrix-badges.herokuapp.com/repos/muflihun/residue-cpp/branches/master/4)](https://travis-ci.org/muflihun/residue-cpp) |
| `master` | GNU/Linux 4.4 / Ubuntu 4.8.4 64-bit / `g++-7` | [![Build Status](https://travis-matrix-badges.herokuapp.com/repos/muflihun/residue-cpp/branches/master/5)](https://travis-ci.org/muflihun/residue-cpp) |

## Getting Started
This library is based on single source file. It automatically includes `<easylogging++.h>` header and expect it to be available on developer's system. Please refer to [documentation page](https://muflihun.github.io/residue/docs/class_residue.html) to check the API.

Please refer to [samples directory](/samples/) to get started

## Installation
This section shows you steps to install residue C++ client on your machine.

## Dependencies
  * C++11 (or higher)
  * [Ripe](https://github.com/muflihun/ripe) v3.3.0
  * Boost v1.59 or higher [Components: system]
  * [Easylogging++](https://github.com/muflihun/easyloggingpp) v9.95.0
  * [CMake Toolchains](https://cmake.org/) v2.8.12
 
# Get Code
You can either [download code from master branch](https://github.com/muflihun/residue-cpp/archive/master.zip) or clone it using `git`:

```
git clone git@github.com:muflihun/residue.git
```

# Build
Residue uses the CMake toolchains to create makefiles.
Steps to build Residue:

```
mkdir build
cd build
cmake -Dtest=ON ..
make
```

You can define following options in CMake (using `-D<option>=ON`)

|    Option    | Description                     |
| ------------ | ------------------------------- |
| `test`       | Compile unit tests              |
| `build_sample_app`      | Builds detailed-cmake sample           |
| `static_ripe`      | Link `ripe` statically (default is OFF)           |

Please consider running unit test before you move on

```
make test
```

The compilation process creates executable `residue` in build directory. You can install it in system-wide directory using:

```
make install
```

If the default path (`/usr/local`) is not where you want things installed, then set the `CMAKE_INSTALL_PREFIX` option when running cmake. e.g,

```
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/bin
```

### If Build Fails...
Make sure you have all the dependencies installed. You can use following script to install it all and then go back to [Build](#build) section (tested on Ubuntu 16.04 64-bit)

```
sudo apt-get install -y libboost-system-dev cmake
wget -O gtest.tar.gz https://github.com/google/googletest/archive/release-1.7.0.tar.gz
tar xf gtest.tar.gz
cd googletest-release-1.7.0
cmake -DBUILD_SHARED_LIBS=ON .
make
sudo cp -a include/gtest /usr/include
sudo cp -a libgtest_main.so libgtest.so /usr/lib/
cd ..
wget -O elpp-master.zip https://github.com/muflihun/easyloggingpp/archive/master.zip
unzip elpp-master.zip
cd easyloggingpp-master
cmake .
make
sudo make install
wget -O ripe-bin.tar.gz https://github.com/muflihun/ripe/releases/download/v3.3.0/ripe-3.3.0-x86_64-linux.tar.gz
tar xfz ripe-bin.tar.gz
cd ripe-3.3.0-x86_64-linux
sudo cp libripe.* /usr/lib/
sudo cp Ripe.h /usr/include/
sudo cp ripe /usr/bin/
```

#### RESIDUE_LIBRARY_LOCAL-NOTFOUND Error

If you get error at `cmake -Dbuild_sample_app=ON ..` step complaining about `RESIDUE_LIBRARY` not found, this is because sample-client application uses this, you can turn this option off, install the library and then rebuild with this option.

### Static Library
By default, residue builds shared library that you can link in order to connect to residue server seamlessly. You can choose to build static library instead using `build_static_lib` option in cmake

For example,

```
cmake -Dbuild_static_lib=ON ..
```

This will create `libresidue.a` and you can create one single static library (with boost bindings) using

```
ar -x libresidue.a
cp /usr/local/lib/libboost_system-mt.a . # Path has to match your system's path
ar -x libboost_system-mt.a
ar -qc libresidue_full.a *.o
```

This will essentially join all the object files from both the libraries (no name overlaps).

You then link with this library and ripe (dynamic) library using options `-lresidue_full -lripe`

## License

```
Copyright 2017 Muflihun Labs

https://github.com/muflihun/
https://muflihun.github.io
https://muflihun.com

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
```
