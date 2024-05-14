#!/bin/bash

# Compiling rsbuild.cpp
# g++ -c -std=c++17 xxhash.cpp
# g++ -c -O2 -I cereal/include/ -std=c++17 bfilt.cpp
# g++ -O2 -std=c++17 -o sa_radix_sort sa_radix_sort.cpp
# g++ -O2 -std=c++17 -o sa_divsufsort sa_divsufsort.cpp
g++ -O2 -std=c++17 -o sa_radix sa_radix.cpp
g++ -O2 -std=c++17 -o sa_radix_slow sa_radix_slow.cpp
g++ -O2 -std=c++17 -o sa_dc3 sa_dc3.cpp
