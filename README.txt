---------------------
Details and Licenses
---------------------

GSI is meant to abstract the interface to various solver packages, making it easier to use them and experiment with different alternatives. Ultimately the goal of GSI was to provide a matlab "slash" operator (ie A\B) for C++ code that provided hardware-accellerated solves on all platforms. However, after years of fighting with integration issues I gave up in despair. Currently GSI is just a grab-bag of linear algebra packages, ported to build under windows, and hacked-up wrappers for two of the sparse linear system solvers.

The GSI zip currently includes:

BLAS/LAPACK:
   - standard netlib software implementations
   - http://netlib.org/blas/
   - http://www.netlib.org/lapack/ 
   - license: modified BSD (http://www.netlib.org/lapack/LICENSE.txt)

TAUCS:
   - symmetric sparse linear system solvers by Sivan Toledo
   - http://www.tau.ac.il/~stoledo/taucs/
   - LGPL or public-domain-with-attribution "Taucs license" (see webpage)

UMFPACK: 
   - non-symmetric sparse linear system solver by Tim Davis
   - http://www.cise.ufl.edu/research/sparse/umfpack/
   - included version 5.1 is LGPL, newer versions from website are GPL (contact author for non-GNU license)

PRIMME: 
   - sparse eigensolver by Andreas Stathopoulos
   - http://www.cs.wm.edu/~andreas/software/
   - license is LGPL

AMD: 
   - sparse matrix reordering package by Tim Davis
   - http://www.cise.ufl.edu/research/sparse/amd/
   - license is LGPL

METIS: 
   - sparse matrix partitioning package by George Karypis
   - http://glaros.dtc.umn.edu/gkhome/views/metis
   - freely redistributable for non-commercial uses, commercial use requires permission (details)

f2c: 
   - helper library for using fortran libs in C code
   - http://www.netlib.org/f2c/
   - presumed public domain (no clear license anywhere)

SparseLib: 
   - sparse matrix format library by Roldan Pozo, Karin Remington, and Andrew Lumsdaine
   - http://math.nist.gov/sparselib++/
   - public domain, recognition required (see header of source code files)

The bits of code outside of the packages/ subtree are provided under the Boost 1.0 License, so you can use them for any purpose, commercial or non-commercial. However, this code is essentially just glue between the above libraries and a more generic API, so it inherits the more restrictive licenses.


I am releasing GSI for two reasons. First, because my libgeometry depends on the sparse solver wrappers in a few places, so you may need to rebuild it if you want to port libgeometry. Second, because if you are trying to use any of these libraries on Windows, I have saved you the effort of getting them to build in Visual Studio (the projects should properly convert to Visual Studio 2010 if needed).

---------------------
Build Instructions
---------------------

Instructions for building SparseMatrixSolver.lib (libgeometry dependency):

open packages\all_packages.sln and build the Release_MT configuration (the others will not work)
open GSI_vc9.sln and build Release or Debug configurations (both should work)
Run (will run SolverTests executable to make sure linking works)
Note that this build depends on pre-complied BLAS and LAPLACK static libraries which are included in packages\lib\precompiled. If you would like to re-build *everything* from scratch, you need to install cygwin and build LAPACK using gcc/g77 within cygwin. The source code is included in packages\LAPACK3.1.1, in that folder just run GSI_build_lapack.sh.

Visual Studio projects are also included for PRIMME and SparseLib, in their respective folders under packages\

---------------------
Known Issues
---------------------

Currently this build system links with the static (ie non-DLL) version of the visual studio libraries (ie /MT instead of /MD). You cannot use /MD because (at time of writing) the cygwin LAPACK build depends on MINGW which internally assumes /MT. In theory you could recompile MINGW to use /MD instead. That would be a lot of work. Easier is to just use a different LAPACK/BLAS, which will also probably give you better performance (see next paragraph).

The included static BLAS and LAPACK libraries are the default software implementations. They are very slow. If you replace them with an optimized version (for example tuned for your machine, or using SSE instructions) you will see massive performance improvements. You can get optimized BLAS/LAPACK in various ways. One is to use ATLAS, which is somewhat complicated on windows. You will need to build ATLAS under cygwin, then convert to a windows lib/DLL. I have included some scripts for doing this in packages\ATLAS. Much easier is to use AMD's ACML or Intel's MKL. ACML is free to use and redistribute, even in commercial packages.

On OSX you can avoid this whole BLAS/LAPACK issue because you can just use the vecLib framework which ships with OSX, that contains hardware BLAS/LAPACK implementations. (Note to Microsoft: please do this! Please!). The packages above all come with unix-style makefiles and hence are trivial to build on OSX. Note that TAUCS will require some tweaking of the build scripts, see this handy blog post by Alec Jacobson for details.

(On Linux you will have to deal with the hardware BLAS/LAPACK question again, although Intel MKL is free to use on Linux.)





