#!/bin/bash


# create libamd.a
cd packages/AMD/Lib/ && make -j && cd -

#create BLAS and LAPACK
cd packages/LAPACK3.1.1/BLAS/SRC && make -f Makefile_linux && cd ../../ 
cd SRC
make -f Makefile_linux -j
cd ..

mv blas_WIN32.a libblasLinux.a
mv lapack_WIN32.a liblapackLinux.a

cd .. # back in GSI/packages


# UMFPACK
cd UMFPACK && mkdir build 
cd build && cmake .. && make -j && cd ../../


# install new version of taucs
if [ -f taucs_full/lib/linux64/libtaucs.a ]
then
  echo taucs was already updated
else
  echo updating taucs...
  ./update_taucs.sh 
fi

cd ..

mkdir build
cd build && cmake .. && make -j3





