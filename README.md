﷽

# Residue C++ Client
Official C++ client library with feature-rich API to interact with residue seamlessly. It uses [Easylogging++](https://github.com/muflihun/easyloggingpp) to interact and take input from the user. Easylogging++ is highly efficient, well-tested library.

[![Build Status](https://img.shields.io/travis/muflihun/residue-cpp/master.svg)](https://travis-ci.org/muflihun/residue-cpp) [![Build Status](https://img.shields.io/travis/muflihun/residue-cpp/develop.svg)](https://travis-ci.org/muflihun/residue-cpp) [![Version](https://img.shields.io/github/release/muflihun/residue-cpp.svg)](https://github.com/muflihun/residue-cpp/releases/latest) [![Documentation](https://img.shields.io/badge/docs-doxygen-blue.svg)](https://muflihun.github.io/residue/docs/annotated.html) [![Apache-2.0 license](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://github.com/muflihun/residue-cpp/blob/master/LICENCE) [![Donate](https://img.shields.io/badge/Donate-PayPal-green.svg)](https://www.paypal.me/MuflihunDotCom/25)

## Getting Started
This library is based on single source file. It automatically includes `<residue/easylogging++.h>` header and expect it to be available on developer's system. Please refer to [documentation page](https://muflihun.github.io/residue/docs/class_residue.html) to check the API.

Please refer to [samples directory](/samples/) to get started

## Installation
This section shows you steps to install residue C++ client library on your machine.

## Download Binary
You can download binary from [releases](https://github.com/muflihun/residue-cpp/releases) page for your platform. They are standalone libraries with dependency on `libz` that usually comes with operating system distribution.

 * Download tar archive for your platform
 * Download `easylogging.h` and `Residue.h`
 * Unzip tar archive and copy them to `/usr/local/lib/`
 * Copy `easylogging.h` to `/usr/local/include/residue/` as `easylogging++.h`
 * Copy `Residue.h` to `/usr/local/include/residue/`
 
In a nut shell you will do:

```
VERSION=1.1.0
TYPE=linux

## Headers
wget https://github.com/muflihun/residue-cpp/releases/download/v$VERSION/Residue.h
wget https://github.com/muflihun/residue-cpp/releases/download/v$VERSION/easylogging.h
mkdir /usr/local/include/residue/
cp Residue.h /usr/local/include/residue/
cp easylogging.h /usr/local/include/residue/easylogging++.h

## Dynamic lib
[[ $TYPE = "linux" ]] && EXTENSION="so" || EXTENSION="dylib"
wget https://github.com/muflihun/residue-cpp/releases/download/v$VERSION/libresidue-$VERSION-x86_64-$TYPE.tar.gz
tar -xf libresidue-$VERSION-x86_64-$TYPE.tar.gz
cp libresidue-$VERSION-x86_64-$TYPE/libresidue.$EXTENSION /usr/local/lib/libresidue-$VERSION.$EXTENSION
ln -s /usr/lib/libresidue-$VERSION.$EXTENSION /usr/local/lib/libresidue.$EXTENSION

## Static lib
wget https://github.com/muflihun/residue-cpp/releases/download/v$VERSION/libresidue-$VERSION-static-x86_64-$TYPE.tar.gz
tar -xf libresidue-$VERSION-static-x86_64-$TYPE.tar.gz
cp libresidue-$VERSION-static-x86_64-$TYPE/libresidue-static.a /usr/local/lib/
```

You should be ready to link your application against `libresidue`, both statically and dynamically.

If you use cmake, you may also be interested in [Residue CMake module](https://github.com/muflihun/residue-cpp/blob/master/cmake/FindResidue.cmake)

## Build
If you do not wish to download binaries, you can build your own library using following steps.

### Dependencies
  * C++11 compiler (or higher)
  * [Crypto++](https://www.cryptopp.com/) v5.6.5+ [with Pem Pack](https://raw.githubusercontent.com/muflihun/muflihun.github.io/master/downloads/pem_pack.zip)
  * [zlib-devel](https://zlib.net/)
 
### Get The Code
You can either [download code from master branch](https://github.com/muflihun/residue-cpp/archive/master.zip) or clone it using `git`:

```
git clone git@github.com:muflihun/residue-cpp.git
```

### Build
Residue C++ library uses the CMake toolchains to create makefiles.

In a nut shell, you will do:

```
mkdir build
cd build
cmake -Dtest=ON ..
make
```

### CMake Options
You can change following options in CMake (using `-D<option>=ON`)

|    Option    | Description                     |
| ------------ | ------------------------------- |
| `test`       | Compile unit tests              |
| `build_sample_app`      | Builds detailed-cmake sample           |
| `profiling`      | Turn on profiling for debugging purposes           |
| `special_edition`      | Build [special edition](https://github.com/muflihun/residue/blob/master/docs/INSTALL.md#special-edition)           |

### Run Tests
Please consider running unit test before you move on

```
make test
```

### Install
The compilation process creates `libresidue` (static and shared) in build directory. Please see [Static Library](#static-library) section below before installing. You can install it in system-wide directory using:

```
make install # Please read Static Library section below
```

If the default path (`/usr/local/lib`) is not where you want things installed, then set the `CMAKE_INSTALL_PREFIX` option when running cmake. e.g,

```
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/lib
```

### Setup
Make sure you have all the dependencies installed. You can use following script to install them and then go back to the [Build](#build) section (tested on Ubuntu 14.04 (Trusty) 64-bit)

```
### Essentials
sudo apt-get install -y cmake build-essential libz-dev
    # sudo yum install -y cmake zlib-devel # for rpm
    # sudo yum groupinstall 'Development Tools'

### Google Testing Library
sudo apt-get install -y libboost-system-dev cmake
wget -O gtest.tar.gz https://github.com/google/googletest/archive/release-1.7.0.tar.gz
tar xf gtest.tar.gz
cd googletest-release-1.7.0
cmake -DBUILD_SHARED_LIBS=ON .
make
sudo cp -a include/gtest /usr/include
sudo cp -a libgtest_main.so libgtest.so /usr/lib/

## Crypto++
wget https://raw.githubusercontent.com/muflihun/muflihun.github.io/master/downloads/cryptocpp.tar.gz
tar xf cryptocpp.tar.gz
cd cryptopp-CRYPTOPP_5_6_5
wget https://raw.githubusercontent.com/muflihun/muflihun.github.io/master/downloads/pem_pack.zip
unzip pem_pack.zip
cmake .
make
sudo make install
```

### Static Library
By default, residue builds shared and static libraries. But static library only contains residue specific objects.

You can use following command to produce correct static library that is independent of any other library.

```
mkdir build
cd build
sh ../tools/package.sh linux 1.1.0 # specify version carefully to match with what's in CMakeLists.txt
```

This will create:
 * `libresidue-1.1.0-x86_64-linux.tar.gz`
 * `libresidue-1.1.0-static-x86_64-linux.tar.gz`
 
Second one (`libresidue-1.1.0-static-x86_64-linux.tar.gz`) contains static library that is fully independent.

### Strip
You can take advantage [`strip`](https://linux.die.net/man/1/strip) if you wish to link your application statically. This will reduce binary size significantly. 

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

## License

```
Copyright 2017-present Muflihun Labs

https://github.com/muflihun/
https://muflihun.github.io/
https://muflihun.com/

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
