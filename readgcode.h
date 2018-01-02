/*
 * gcoderead.h
 *
 *  Created on: Dec 2, 2017
 *      Author: arhimed
 */

#ifndef READGCODE_H_
#define READGCODE_H_

#include <stdbool.h>
#include "gcodestat.h"

#ifndef _MIN_
  #define _MIN_(a,b) (((a)<(b))?(a):(b))
#endif

#ifndef _MAX_
  #define _MAX_(a,b) (((a)>(b))?(a):(b))
#endif

bool comment(char *);
int gcode(char *);
double read_dwell(char *);
int read_maxfeed(char *, print_settings_t *);
int read_accel(char *, print_settings_t *);
int read_jdev(char *, print_settings_t *);
int read_rtime(char *, print_settings_t *);
int read_ptime(char *, print_settings_t *);
int read_speedover(char *, print_settings_t *);

#endif /* READGCODE_H_ */
