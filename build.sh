#!/bin/bash

make clean
make

# testing starts from here
./cli_libev_test
./cli_netlib_test
