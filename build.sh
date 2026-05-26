#!/bin/sh

# Very optimized
#g++ -O3 -ffast-math -ftree-vectorize -march=native -s -o noise4 noise4.cpp

# Optimized
g++ -O3 -s -o noise4 noise4.cpp
