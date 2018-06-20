#!/bin/sh

# Environment
PHP_TEST_PATH="./tests"
PHP_TO_PATH="./asf"

# Check Test Directory
if [ -d ${PHP_TO_PATH}/${PHP_TEST_PATH:2} ]; then
    rm -rf ${PHP_TO_PATH}/${PHP_TEST_PATH:2}
fi

# Copy Test Directory
cp -r ${PHP_TEST_PATH} ${PHP_TO_PATH}

# Make test
cd ${PHP_TO_PATH} && make test
