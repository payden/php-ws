#!/bin/sh
mkdir -pv build
cd build/
wget http://paydensutherland.com/libwebsock-1.0.4.tar.gz
tar -xvzf libwebsock-1.0.4.tar.gz
cd libwebsock-1.0.4
./configure && make && sudo make install
sudo /sbin/ldconfig
cd ../..
