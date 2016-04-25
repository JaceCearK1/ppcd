@echo off
%DEVKITPPC%\bin\powerpc-eabi-as.exe -mregnames -m750cl -mbig %1
%DEVKITPPC%\bin\powerpc-eabi-objcopy.exe -O binary a.out %1.bin
del a.out
pause