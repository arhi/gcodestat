@echo off

for %%a in (%1) do (
    set filepath=%%~dpa
    set filename=%%~na
    set extension=%%~xa
)   
set if=%filepath%%filename%%extension%
set of=%filepath%%filename%_M117%extension%

C:\gcodestat\gcodestat.exe --alert -d 0.02 -a 1000 --gcode="%if%" --output="%of%" --api_key=AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA --api_url="http://192.168.1.10/api/files/local" 

