call vcvars32_vs2005.bat


set OSTYPE=win32
rmdir /S /Q build/win32_mkl
configurator\configurator_win32.exe win32 in=win32_config.txt variant=_mkl
copy makefile makefile_win32_mkl
nmake clean
nmake