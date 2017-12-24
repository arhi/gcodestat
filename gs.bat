@echo off

for %%a in (%1) do (
    set filepath=%%~dpa
    set filename=%%~na
    set extension=%%~xa
)   
set if=%filepath%%filename%%extension%
set of=%filepath%%filename%_M117%extension%

E:\Dev\eclipse-workspace\gcodestat\gcodestat.exe -d 0.02 -a 1000 --gcode="%if%" --output="%of%" | c:\Windows\System32\msg.exe %username%
E:\bin\curl-7.50.3-win64-mingw\bin\curl.exe -H "X-Api-Key: XXXXXXYOURAPIKEYXXX" -F "select=false" -F "print=false" -F "file=@%of%" "http://octoprintip/api/files/local"

