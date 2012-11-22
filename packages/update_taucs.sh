#!/bin/bash

rm -r taucs
rm -r taucs_full

wget https://gforge.inria.fr/frs/download.php/24628/taucs_full.tgz
tar xvzf taucs_full.tgz

patch taucs_full/configure < taucs_patches/configure_diff.txt # setting OSTYPE=linux64
patch taucs_full/src/taucs.h < taucs_patches/taucs_h_diff.txt # adding add #define TAUCS_BLAS_UNDERSCORE
patch taucs_full/config/linux64.mk < taucs_patches/linux64.diff # add -lgfortran

cd taucs_full && ./configure && make -j


