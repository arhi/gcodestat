# gcodestat
Calculate real time needed to execute G-Code (so you know how long your 3D print will actually take)

This code is based on acceleration and junction_deviation calculation smoothieware is doing while executing G-Code. Marlin uses acceleration and jerk so calculation is bit different so it will not be as precise with Marlin based printers as it will be with Smoothieware based ones.

Anyhow even with junction_deviation set to 0 and just using acceleration the Marlin based printer will execute G-code to much closer time then what slicers normally predict, so this code is useful for Marlin too. On TODO list is to add full Marlin's support for jer (if someone else does it and create pull request that would be even better :D )

There are some sample gcode's in the samples directory along with a sample config file for smoothieware (the gcodestat knows how to extract data from config file for smoothieware)

Not everything is taken into account for now. For example if you use firmware retraction/prime feature those are ignored, temperature reaching time is also ignored etc.. 

example:

```
e:\Dev\eclipse-workspace\gcodestat>gcodestat.exe -c samples\config.txt -g samples\31min17sec.gcode
gcodestat v0.0
Starting with parameters:
        acceleration:           1000.000000 mm/sec/sec
        junction deviation:     0.020000
        x max speed:            30000.000000 mm/min
        y max speed:            30000.000000 mm/min
        z max speed:            1200.000000 mm/min
        retraction time:        0.000000 sec
        prime time:             0.000000 sec
        speed override:         1.000000
        absolute movement:      yes
        absolute extrusion:     yes
        metering unit:          mm

Total seconds: 1646.337528
Total 27:26
```
```
e:\Dev\eclipse-workspace\gcodestat>gcodestat.exe -c samples\config.txt -g samples\2h17min42sec.gcode
gcodestat v0.0
Starting with parameters:
        acceleration:           1000.000000 mm/sec/sec
        junction deviation:     0.020000
        x max speed:            30000.000000 mm/min
        y max speed:            30000.000000 mm/min
        z max speed:            1200.000000 mm/min
        retraction time:        0.000000 sec
        prime time:             0.000000 sec
        speed override:         1.000000
        absolute movement:      yes
        absolute extrusion:     yes
        metering unit:          mm

Total seconds: 8396.563453
Total 02:19:56
```
```
e:\Dev\eclipse-workspace\gcodestat>gcodestat.exe -c samples\config.txt -g samples\2h23min5sec.gcode
gcodestat v0.0
Starting with parameters:
        acceleration:           1000.000000 mm/sec/sec
        junction deviation:     0.020000
        x max speed:            30000.000000 mm/min
        y max speed:            30000.000000 mm/min
        z max speed:            1200.000000 mm/min
        retraction time:        0.000000 sec
        prime time:             0.000000 sec
        speed override:         1.000000
        absolute movement:      yes
        absolute extrusion:     yes
        metering unit:          mm

Total seconds: 8718.500820
Total 02:25:18
```
```
e:\Dev\eclipse-workspace\gcodestat>gcodestat.exe -c samples\config.txt -g samples\53min18sec.gcode
gcodestat v0.0
Starting with parameters:
        acceleration:           1000.000000 mm/sec/sec
        junction deviation:     0.020000
        x max speed:            30000.000000 mm/min
        y max speed:            30000.000000 mm/min
        z max speed:            1200.000000 mm/min
        retraction time:        0.000000 sec
        prime time:             0.000000 sec
        speed override:         1.000000
        absolute movement:      yes
        absolute extrusion:     yes
        metering unit:          mm

Total seconds: 3026.757087
Total 50:26
```

The filenames in the samples dir have name indicating "Real time" it took for the code to print (measured by octoprint after the print). The bed preheat time is not there so only extruder heat time is there and that accounts for small discrepancy in time between real print and calculated time. There are also some other places where discrepancies can occur but it's "acceptable" ..

for e.g. you see that 2h17min42sec.gcode file that was printed in reality in 2:17:42 have calculated time of 02:19:56 while Simplify3D predicted 1:39:00 .. so while our estimate was 2min out for over 2 hour print Simplify3D missed the mark by 40 minutes.

To integrate this with Simplify3D you can create a batch file like:

```
@echo off

for %%a in (%1) do (
    set filepath=%%~dpa
    set filename=%%~na
    set extension=%%~xa
)   
set if=%filepath%%filename%%extension%
set of=%filepath%%filename%_M117%extension%

E:\Dev\eclipse-workspace\gcodestat\gcodestat.exe -d 0.02 -a 1000 --alert --gcode="%if%" --output="%of%" --api_key=YOUROCTOPRINTAPIKEY --api_url="http://192.168.0.1/api/files/local" 

```

 --alert will display the windows alert showing time to print
 --api_key & --api_url if set will upload the file to the octoprint (finally you can send to octoprint from simplify3d)

and then put in the "Scripts" tab, "additional terminal commands for postprocessing" a line that call's it:

```
e:\path\to\gcodestat\gs.bat "[output_filepath]" 
```

Note that if you don't have --output and you have api_key and api_url the input file will be uploaded to octoprint, but if you have --output then the output file will be the one being uploaded.

One good side effect of this is that finally your API key will not be embedded in the g-code (if you used curl in the additional post processing script to upload to octoprint) so a little bit security there :)

if you don't want the windows popup to show you time left you can add -q or --quiet so you suppress any output

the M117 file you create, even if you don't have a LCD on your printer and you use Octoprint as a host you can use some of the Octoprint plugins to display M117 messages in browser during print, for e.g.
https://github.com/AmedeeBulle/StatusLine
https://github.com/jneilliii/OctoPrint-M117PopUp


# OctoPrint
If you are using OctoPrint I wrote a plugin [gcodestatEstimator](https://github.com/arhi/OctoPrint-gcodestatEstimator) that will use the M117 codes embedded by the gcodestat to inform OctoPrint about "real" time to finish the print
