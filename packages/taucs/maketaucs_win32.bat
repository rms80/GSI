call vcvars32_vs2005.bat


set OSTYPE=win32
rmdir /S /Q build/win32
configurator\configurator_win32.exe win32 in=win32_config.txt
copy makefile makefile_win32
nmake clean
nmake

