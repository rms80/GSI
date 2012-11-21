call vsvars32_vs2008_x64.bat


set OSTYPE=win32
configurator\configurator.exe win32 in=win32_config.txt
move makefile makefile_win32
configurator\configurator.exe win32 in=win32_config.txt variant=_mkl
move makefile makefile_win32_mkl 
configurator\configurator.exe win32 in=win32_config.txt variant=_atlas
move makefile makefile_win32_atlas
