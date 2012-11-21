Compilation on 64Bit-Linux:

- Compiling AMD:
cd Lib
make -f GNUmakefile  # create libamd.a in Lib

- Compiling Lapack/Blas
cd LAPACK3.1.1/BLAS

in LAPACK3.1.1/BLAS/SRC: 
append -cpp to last line

170: .f.o: 
171:  	$(FORTRAN) $(OPTS) -c $< -o $@ -cpp

in LAPACK3.1.1/make.inc:

13: PLAT = Linux 

To finally compile the library, execute make in LAPACK3.1.1/BLAS/SRC. The library will be stored in
LAPACK3.1.1 as blas_Linux, rename this to libblasLinux.a.

To compile Lapack, again append the -cpp option, this time in LAPACK3.1.1/SRC/Makefile in line 335 and make it. The lib is again stored on level above, rename it to  liblapackLinux.a.

- Compiling UMFPACK
> mkdir build && cd build && cmake .. && make 
Lib is stored as libUMFPACK.a

- taucs
https://gforge.inria.fr/frs/download.php/24628/taucs_full.tgz
l 64; OSTYPE=linux64 configure
src/taucs.h: #define TAUCS_BLAS_UNDERSCORE


- gsi:
in GSI: 
> mkdir build && cd build && cmake .. && make


