/*
 * gcodestat.h
 *
 *  Created on: Dec 2, 2017
 *      Author: arhimed
 */

#ifndef GCODESTAT_H_
#define GCODESTAT_H_

#include <stdbool.h>

#define LINE_BUFFER_LENGTH         4096
#define DEFAULT_ACCELERATION       100
#define DEFAULT_JUNCTION_DEVIATION 0.02
#define DEFAULT_JERK               4.00
#define DEFAULT_MAX_SPEED           100000
#define DEFAULT_RETRACT_TIME       0.0
#define DEFAULT_PRIME_TIME         0.0

#define GCODE_IRRELEVANT 0
#define GCODE_MOVE       1
#define GCODE_DWELL      2
#define GCODE_RETRACT    3
#define GCODE_INCH       4
#define GCODE_MM         5
#define GCODE_ABS        6
#define GCODE_REL        7
#define GCODE_EABS       8
#define GCODE_EREL       9
#define GCODE_MAXFEED   10
#define GCODE_ACCEL     11
#define GCODE_JDEV      12
#define GCODE_RLEN      13
#define GCODE_PLEN      14
#define GCODE_SPEEDOVER 15

//TODO: make X,Y,Z accel/jdev separately configured
typedef struct {
	double accel;
	double jdev;
	double x_maxspeed;
	double y_maxspeed;
	double z_maxspeed;
	double rtime;
	double ptime;
	double speedoverride;
	bool abs;
	bool eabs;
	bool mm;
	bool jerk;
} print_settings_t;

#endif /* GCODESTAT_H_ */
