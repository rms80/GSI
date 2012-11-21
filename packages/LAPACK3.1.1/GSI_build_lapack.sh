#!/bin/bash

# build standard f77 blas and lapack

make blaslib
make lapacklib

# make windows libs
# !! assumes vcvarsall.bat has already been called...
lib blas_WIN32.a /OUT:blas_win32.lib
lib lapack_WIN32.a /OUT:lapack_win32.lib


#copy to output

if [ ! -e "../lib" ]; then
    mkdir ../lib
fi

if [ ! -e "../lib/precompiled" ]; then
    mkdir ../lib/precompiled
fi


cp blas_WIN32.a lapack_WIN32.a ../lib/precompiled
cp blas_win32.lib lapack_win32.lib ../lib/precompiled

