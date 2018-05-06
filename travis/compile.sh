#!/bin/sh
w
php-config
gdb -v
ulimit -a
phpize && ./configure && make clean && make
