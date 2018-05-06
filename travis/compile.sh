#!/bin/sh
php-config
phpize && ./configure && make clean && make
