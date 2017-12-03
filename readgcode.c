/*
 * gcoderead.c
 *
 *  Created on: Dec 2, 2017
 *      Author: arhimed
 */
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "readgcode.h"

/*
 * comment(buffer) - is the whole line just a comment
 */
bool comment(char *buffer){
  short ret;
  char c;

  ret = 2;
  while ('\0'!=( c = *(buffer++))){
    switch (c) {
      case ';':
      case '#':
      case '\r':
      case '\n':
        ret = 1;
        break;
      case ' ':
      case '\t':
        continue;
      default:
	ret = 0;
    }
    if (ret != 2) break;
  }
  return ret > 0;
}

/*
 * return G-code
 */
int gcode(char *buffer) {
	char c;
	while ('\0' != (c = *(buffer++))) {
		switch (c) {
		case ';':
		case '#':
		case '\r':
		case '\n':
			return GCODE_IRRELEVANT;
		case ' ':
		case '\t':
			continue;
		case 'G':
		case 'g': //this actually shound not work but never mind
			//G
			switch (atoi(buffer)) {
			case 0:
			case 1:
				return GCODE_MOVE;
				// case 2: //cw circular unsupported
				// case 3: //cc circular unsupported
			case 4:
				return GCODE_DWELL;
			case 10:
			case 11:
				return GCODE_RETRACT;
			case 20: // who the hell uses this?!
				return GCODE_INCH;
			case 21:
				return GCODE_MM;
			case 90:
				return GCODE_ABS;
			case 91:
				return GCODE_REL;

			default:
				return GCODE_IRRELEVANT;
			}
			break;
		case 'M':
		case 'm': //this also should not work but who cares
			//M
			switch (atoi(buffer)) {
			case 82:
				return GCODE_EABS;
			case 83:
				return GCODE_EREL;
			case 203:
				return GCODE_MAXFEED;
			case 204:
				return GCODE_ACCEL;
			case 205:
				return GCODE_JDEV;
			case 207:
				return GCODE_RLEN;
			case 208:
				return GCODE_PLEN;
			case 220:
				return GCODE_SPEEDOVER;

			default:
				return GCODE_IRRELEVANT;
			}
		}
	}
	return GCODE_IRRELEVANT;
}

/*
 * always use name as upper case and function will & 0xDF so will check for lower case too
 */
int read_gvalue(char *buffer, char name, double * value){
  char c;

//  while ('\0'!=( c = *(buffer++)))
//    if ((c & 0xDF) == 'G')
      while( (c =  *(buffer++) ) != '\0')
        if ((c & 0xDF) == name ){
              *value = atof(buffer++);
              return(0);
  }
  return(-1);
}


/*
 * read_dwell(buffer) - G4
 * http://smoothieware.org/g4
 */
double read_dwell(char *buffer){
  char c;
  double ret;
  ret = 0.0;
  while ('\0'!=( c = *(buffer++)))
    if ((c & 0xDF)=='G')
      while( (c =  *(buffer++) ) != '\0')
        if ((c & 0xDF)=='P' || (c & 0xDF)=='S')
          switch (c & 0xDF){
            case 'S':
              ret += atof(buffer++);
              break;
            case 'P':
              ret += atof(buffer++)/1000.0;
              break;
            case ';':
            case '#':
              return ret;
  }
  return ret;
}

/*
 * M203
 */
int read_maxfeed(char *buff, print_settings_t *print_settings){
	read_gvalue(buff, 'X', &(print_settings->x_maxspeed));
	read_gvalue(buff, 'Y', &(print_settings->y_maxspeed));
	read_gvalue(buff, 'Z', &(print_settings->z_maxspeed));
	return(0);
}

int read_accel(char* buffer, print_settings_t *print_settings){
	double Saccel, Xaccel;
	Saccel = 0;
	Xaccel = 0;
	read_gvalue(buffer, 'S', &Saccel);
	read_gvalue(buffer, 'X', &Xaccel);
	if (min(Saccel, Xaccel) > 0) print_settings->accel = min(Saccel, Xaccel);
	return(0);
}

int read_jdev(char* buffer, print_settings_t *print_settings){
	return read_gvalue(buffer, 'X', &(print_settings->jdev));
}

int read_speedover(char* buffer, print_settings_t *print_settings){
	return read_gvalue(buffer, 'S', &(print_settings->speedoverride));
}

//TODO:
int read_rtime(char* buffer, print_settings_t *print_settings){return(-1);}
int read_ptime(char* buffer, print_settings_t *print_settings){return(-1);}
