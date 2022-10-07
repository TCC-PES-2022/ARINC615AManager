    # ARINC615AManager

This is library for transferring data between two endpoints using ARINC 615A protocol.

This file is part of the [PROJECT WITHOUT A NAME YET] project.

To clone this repository, run:

    git clone https://github.com/TCC-PES-2022/ARINC615AManager.git
    cd ARINC615AManager
    git submodule update --init --recursive

Before building your project, you may need to install some dependencies. To do so, run:

    sudo apt install -y build-essential

Export the instalation path to the environment:
    export DESTDIR=<path_to_install>

You can also define this variable in your `.bashrc` file. or when calling any make rule. The default installation path is `/tmp`.

To build, run:
    make deps && make

To install, run:
    make install

To test, first build gtest:
    cd test && make gtest

Then run:
    make deps && make && make runtests

To generate test coverage, run:
    make report