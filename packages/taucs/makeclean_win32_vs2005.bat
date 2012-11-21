# set visual studio variables
call vcvars32_vs2005.bat

copy makefile_win32 makefile
nmake clean

copy makefile_win32_mkl makefile
nmake clean

copy makefile_win32_atlas makefile
nmake clean

del makefile*