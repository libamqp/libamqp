#!/bin/bash

if [ "$(uname)" = "Linux" ]
then
    sudo apt-get install libtool autoconf
fi

sh ./autogen.sh
./configure
make
sudo make install
