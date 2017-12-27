/*
 * calcmove.h
 * Author: arhimed
 */

#ifndef CALCMOVE_H_
#define CALCMOVE_H_

#include <stdbool.h>
#include "gcodestat.h"

#ifndef _MIN_
  #define _MIN_(a,b) (((a)<(b))?(a):(b))
#endif


double calcmove(char *,  print_settings_t *);


#endif /* CALCMOVE_H_ */
