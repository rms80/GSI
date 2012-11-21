#########################################################
# WIN32                                                 #
#########################################################
OBJEXT=.obj
LIBEXT=.lib
EXEEXT=.exe
F2CEXT=.c
# make sure there is a space after the backslash
PATHSEP=\\
DEFFLG=/D

CC        = cl
CFLAGS    = /nologo /O2 /W0 /D "WIN32" /MT  
#CFLAGS    = /nologo /O2 /W3 /D "WIN32" /MT   
COUTFLG   =/Fo

FC        = $(CC)
FFLAGS    = $(CFLAGS)
FOUTFLG   = $(COUTFLG)

LD        = $(CC)
LDFLAGS   = /nologo /MT /F64000000
LOUTFLG   = /Fe

RM        = del /Q
AR        = lib /nologo
AOUTFLG   = /out:

RANLIB    = echo

LIBBLAS   = ..\\lib\\precompiled\\blas_win32.lib
LIBLAPACK = ..\\lib\\precompiled\\lapack_win32.lib

LIBMETIS  = ..\\lib\\metis\\METIS_MT.lib
LIBF77    = ..\\lib\\libf2c\\libf2c_MT.lib
LIBC      =









