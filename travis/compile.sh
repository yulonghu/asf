#!/bin/sh
cd ./asf
phpize && ./configure && make clean && make
