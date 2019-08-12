# violetminer

[![Build Status](https://travis-ci.org/turtlecoin/violetminer.svg?branch=master)](https://travis-ci.org/turtlecoin/violetminer)

![image](https://upload.wikimedia.org/wikipedia/commons/thumb/8/87/Argon_discharge_tube.jpg/640px-Argon_discharge_tube.jpg)

## Algorithms Supported
* TurtleCoin - choose `turtlecoin` or `chukwa`
* WrkzCoin - choose `wrkzcoin` or `chukwa_wrkz`

## Notes
* Supports xmrig-proxy - Make sure to enable `niceHash: true` in your pool config.

## Example Config

Simply go through the normal setup and a config will be written to `config.json` on completion.
Alternatively, copy and modify the following config to your purposes.

```
{
    "pools": [
        {
            "agent": "",
            "algorithm": "turtlecoin",
            "host": "127.0.0.1",
            "niceHash": false,
            "password": "",
            "port": 3333,
            "rigID": "",
            "username": "TRTLv2Fyavy8CXG8BPEbNeCHFZ1fuDCYCZ3vW5H5LXN4K2M2MHUpTENip9bbavpHvvPwb4NDkBWrNgURAd5DB38FHXWZyoBh4wW"
        },
        {
            "agent": "",
            "algorithm": "wrkzcoin",
            "host": "127.0.0.1",
            "niceHash": true,
            "password": "",
            "port": 5000,
            "rigID": "",
            "username": "WrkzjJMM8h9F8kDU59KUdTN8PvZmzu2HchyBG15R4SjLD4EcMg6qVWo3Qeqp4nNhgh1CPL7ixCL1P4MNwNPr5nTw11ma1MMXr7"
        }
    ],
    "threadCount": 12
}
```

## Compiling

### Windows

- Download the [Build Tools for Visual Studio 2019](https://visualstudio.microsoft.com/thank-you-downloading-visual-studio/?sku=BuildTools&rel=16) Installer
- When it opens up select **C++ build tools**, it automatically selects the needed parts
- Install the latest full version of OpenSSL if you want to compile with SSL support. (currently OpenSSL 1.1.1c). Select the appropriate version for your system:
  - [OpenSSL 64-bit](https://slproweb.com/download/Win64OpenSSL-1_1_1c.exe)
  - [OpenSSL 32-bit](https://slproweb.com/download/Win32OpenSSL-1_1_1c.exe)

For 64-bit:
- From the start menu, open 'x64 Native Tools Command Prompt for VS 2019'.
- `git clone https://github.com/turtlecoin/violetminer`
- `cd violetminer`
- `git submodule update --init --recursive`
- `mkdir build`
- `cd build`
- `cmake -G "Visual Studio 16 2019" -A x64`
- `MSBuild violetminer.sln /p:Configuration=Release /m`

For 32-bit:
- From the start menu, open 'x86 Native Tools Command Prompt for VS 2019'.
- `git clone https://github.com/turtlecoin/violetminer`
- `cd violetminer`
- `git submodule update --init --recursive`
- `mkdir build`
- `cd build`
- `cmake -G "Visual Studio 16 2019" -A Win32 ..`
- `MSBuild violetminer.sln /p:Configuration=Release /p:Platform=Win32 /m` 

### Ubuntu, using GCC

- `sudo add-apt-repository ppa:ubuntu-toolchain-r/test -y`
- `sudo apt-get update`
- `sudo apt-get install aptitude -y`
- `sudo aptitude install -y build-essential g++-8 gcc-8 git python-pip libssl-dev`
- `sudo pip install cmake`
- `export CC=gcc-8`
- `export CXX=g++-8`
- `git clone https://github.com/turtlecoin/violetminer`
- `cd violetminer`
- `git submodule update --init --recursive`
- `mkdir build`
- `cd build`
- `cmake ..`
- `make`

### Ubuntu, using Clang

- `sudo add-apt-repository ppa:ubuntu-toolchain-r/test -y`
- `wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | sudo apt-key add -`

You need to modify the below command for your version of ubuntu - see https://apt.llvm.org/

* Ubuntu 14.04 (Trusty)
- `sudo add-apt-repository "deb https://apt.llvm.org/trusty/ llvm-toolchain-trusty 6.0 main"`

* Ubuntu 16.04 (Xenial)
- `sudo add-apt-repository "deb https://apt.llvm.org/xenial/ llvm-toolchain-xenial 6.0 main"`

* Ubuntu 18.04 (Bionic)
- `sudo add-apt-repository "deb https://apt.llvm.org/bionic/ llvm-toolchain-bionic 6.0 main"`

- `sudo apt-get update`
- `sudo apt-get install aptitude -y`
- `sudo aptitude install -y -o Aptitude::ProblemResolver::SolutionCost='100*canceled-actions,200*removals' build-essential clang-6.0 libstdc++-7-dev git python-pip libssl-dev`
- `sudo pip install cmake`
- `export CC=clang-6.0`
- `export CXX=clang++-6.0`
- `git clone https://github.com/turtlecoin/violetminer`
- `cd violetminer`
- `git submodule update --init --recursive`
- `mkdir build`
- `cd build`
- `cmake ..`
- `make`

### Generic Linux

- `git clone https://github.com/turtlecoin/violetminer`
- `cd violetminer`
- `git submodule update --init --recursive`
- `mkdir build`
- `cd build`
- `cmake ..`
- `make`

## Developing

* Update submodules to lastest commit: `git submodule foreach git pull origin master`
