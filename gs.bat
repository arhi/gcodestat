@echo off

e:\Dev\eclipse-workspace\gcodestat\gcodestat.exe -d 0.02 -a 1000 --gcode=%* | c:\Windows\System32\msg.exe %username%
