call vcvars32_vs2005.bat


set OSTYPE=win32
rmdir /S /Q build/win32_atlas
configurator\configurator_win32.exe win32 in=win32_config.txt variant=_atlas
copy makefile makefile_win32_atlas
nmake clean
nmake