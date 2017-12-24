@echo off

for %%a in (%1) do (
    set filepath=%%~dpa
    set filename=%%~na
    set extension=%%~xa
)   
set if=%filepath%%filename%%extension%
set of=%filepath%%filename%_M117%extension%

e:\Dev\eclipse-workspace\gcodestat\gcodestat.exe -d 0.02 -a 1000 --gcode="%if%" --output="%of%" | c:\Windows\System32\msg.exe %username%
