#!/bin/sh
cp -r ./tests ./asf/
cd ./asf && make test
