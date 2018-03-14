::@ECHO OFF
:A
del *.exe /s
C:\MinGW\bin\mingw32-gcc.exe -Wall -O2  -c %cd%\main.c -o %cd%\main.o
C:\MinGW\bin\mingw32-g++.exe -o %cd%\makecmd.exe %cd%\main.o
del *.o /s
%cd%\makecmd.exe
pause
CLS
goto A