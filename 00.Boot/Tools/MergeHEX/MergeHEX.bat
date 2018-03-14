@echo off
del output\output.hex 
merge\mergehex.exe --merge hex\1.hex  hex\2.hex --output output\output.hex 
pause
