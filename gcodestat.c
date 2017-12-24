/*
 * File gcodestat.c
 * Author: arhimed@gmail.com
 * Date 20070721
 *
 * TODO: support marlin
 * TODO: read config (marlin and smoothieware) for default values
 * TODO: support change of parameters from g-code (maxfeed, jdev, accel..)
 * TODO: support max extruder feed rate
 * TODO: reorganize code
 * TODO: move to github (currently on gitlab)
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#include "gcodestat.h"
#include "calcmove.h"
#include "readconfig.h"
#include "readgcode.h"

/*
 * print_usage() - print usage info
 */
void print_usage() {
	fprintf(stderr, "gcodestat \n");
	fprintf(stderr, "\t-h, --help\t\t\t\tshow help\n");
	fprintf(stderr, "\t-1, --quiet\t\t\t\tsuppress any output\n");
	fprintf(stderr, "\t-g, --gcode samples/test.gcode\t\tFile to analyze\n");
	fprintf(stderr, "\t-c, --config samples/config.txt\t\tSmoothieware config file\n");
	fprintf(stderr, "\t-o, --output <outputfile.gcode>\t\tWhere to save modified G-Code file\n");
	fprintf(stderr, "\t-a, --acceleration %d\t\t\tDefault XY acceleration in mm/sec/sec\n", DEFAULT_ACCELERATION);
	fprintf(stderr, "\t-d, --junction_deviation %f\tDefault Junction Deviation\n", DEFAULT_JUNCTION_DEVIATION);
	fprintf(stderr, "\t-j, --jerk %f\t\t\tDefault jerk\n", DEFAULT_JERK);
	fprintf(stderr, "\t-f, --max_feed %d\t\t\tMax feed\n", DEFAULT_MAX_SPEED);
	fprintf(stderr, "\t-r, --retract_time %f\t\tRetraction time in sec\n",DEFAULT_RETRACT_TIME);
	fprintf(stderr, "\t-p, --prime_time %f\t\tPrime time in sec\n",DEFAULT_PRIME_TIME);
	fprintf(stderr, "\t-s, --percent_step 10\t\t\tChange LCD data every ##%%\n");
	fprintf(stderr, "\n\n");
	return;
}

/*
 * print_config(*print_settings) - print config info
 */
void print_config(print_settings_t * ps){
	fprintf(stderr, "gcodestat v0.1\n");
	fprintf(stderr, "Starting with parameters:\n");
	fprintf(stderr, "\tacceleration: \t\t%f mm/sec/sec\n", ps->accel);
	fprintf(stderr, "\tjunction deviation: \t%f\n", ps->jdev);
	fprintf(stderr, "\tx max speed: \t\t%f mm/min\n", ps->x_maxspeed);
	fprintf(stderr, "\ty max speed: \t\t%f mm/min\n", ps->y_maxspeed);
	fprintf(stderr, "\tz max speed: \t\t%f mm/min\n", ps->z_maxspeed);
	fprintf(stderr, "\tretraction time: \t%f sec\n", ps->rtime);
	fprintf(stderr, "\tprime time: \t\t%f sec\n", ps->ptime);
	fprintf(stderr, "\tspeed override: \t%f\n", ps->speedoverride);
	fprintf(stderr, "\tabsolute movement: \t%s\n", ps->abs?"yes":"no");
	fprintf(stderr, "\tabsolute extrusion: \t%s\n", ps->eabs?"yes":"no");
	fprintf(stderr, "\tmetering unit: \t\t%s\n", ps->mm?"mm":"inch");
	fprintf(stderr, "\n");
    return;
}

void print_timeleft(FILE* f, long int sec){
    fprintf(f, "( ");
    if (sec > 60*60*24*7) fprintf(f, "%ld weeks, ", sec / 60 / 60 / 24 / 7);
    if (sec > 60*60*24  ) fprintf(f, "%ld days, " , (sec / 60 / 60 / 24) % 7);
    if (sec > 60*60     ) fprintf(f, "%02ld:"     , (sec / 3600) %24);
    fprintf(f, "%02ld:"                           , (sec / 60) % 60);
    fprintf(f, "%02ld )"                          ,  sec % 60);
}

/*
 * main
 */
int main(int argc, char** argv){
  FILE *f = NULL;
  FILE *output_file = NULL;
  char *lb = NULL;
  char *gcodefile = NULL;
  char *gcodeout  = NULL;
  double seconds = 0;
  double total_seconds = 0;
  print_settings_t print_settings;
  bool quiet = false;
  int pass;
  double next_pct = 100;
  double pct_step = 0.1;


  static struct option long_options[] = {
      {"help", no_argument, NULL, 'h'},
      {"quiet", no_argument, NULL, 'q'},
	  {"gcode", required_argument, NULL, 'g'},
      {"output", required_argument, NULL, 'o'},
      {"config", required_argument, NULL, 'c'},
      {"acceleration", required_argument, NULL, 'a'},
      {"junction_deviation", required_argument, NULL, 'd'},
      {"jerk", required_argument, NULL, 'j'},
      {"max_x_speed", required_argument, NULL, 'x'},
      {"max_y_speed", required_argument, NULL, 'y'},
      {"max_z_speed", required_argument, NULL, 'z'},
      {"retract_time", required_argument, NULL, 'r'},
      {"prime_time", required_argument, NULL, 'p'},
      {"percent_step", required_argument, NULL, 's'},
      {NULL, 0, NULL, 0}
  };


  print_settings.accel         = DEFAULT_ACCELERATION;        // default acceleration mm/sec/sec
  print_settings.jdev          = DEFAULT_JUNCTION_DEVIATION;  // default junction deviation (ex jerk)
  print_settings.abs           = true;                        // by default ABS moves
  print_settings.eabs          = true;                        // by default extruder movement is absolute
  print_settings.mm            = true;                        // by default move in mm
  print_settings.x_maxspeed    = DEFAULT_MAX_SPEED;           // default max speed
  print_settings.y_maxspeed    = DEFAULT_MAX_SPEED;           // default max speed
  print_settings.z_maxspeed    = DEFAULT_MAX_SPEED;           // default max speed
  print_settings.rtime         = DEFAULT_RETRACT_TIME;        // default retract time
  print_settings.ptime         = DEFAULT_PRIME_TIME;          // default prime time
  print_settings.jerk          = false;                       // default we use junction deviation
  print_settings.speedoverride = 1.0;
  quiet                        = false;
  next_pct                     = 0.9;
  seconds                      = 0.0;
  
  int option_index = 0;
	int getopt_result;
	while ((getopt_result = getopt_long(argc, argv, "q?hg:c:a:d:j:x:y:z:r:p:o:s:", long_options, &option_index)) != -1) {
		switch (getopt_result) {
		case 'q':
			quiet = true;
			break;
		case 'h':
		case '?':
			print_usage();
			return (-1);
			break;
		case 'g':
			if (optarg) {
				gcodefile = strdup(optarg);
			} else {
				print_usage();
				return (-1);
			}
			break;
		case 'o':
			if (optarg) {
				gcodeout = strdup(optarg);
			} else {
				print_usage();
				return (-1);
			}
			break;
		case 'c':
			if (optarg) {
				if (read_config(optarg, &print_settings)) {
					fprintf(stderr, "Error reading config from %s\n", optarg);
					return (-1);
				}
			} else {
				print_usage();
				return (-1);
			}
			break;
		case 'a':
			if (optarg) {
				print_settings.accel = atof(optarg);
				if (print_settings.accel == 0) {
					fprintf(stderr, "Invalid acceleration value: %s\n", optarg);
					return (-1);
				}
			} else {
				print_usage();
				return (-1);
			}
			break;
		case 'd':
			if (optarg) {
				print_settings.jdev = atof(optarg);
				if (print_settings.jdev == 0) {
					fprintf(stderr, "Invalid junction deviation value: %s\n",
							optarg);
					return (-1);
				}
			} else {
				print_usage();
				return (-1);
			}
			break;
		case 'j':
			if (optarg) {
				//TODO: implement jerk thingy
				fprintf(stderr, "TODO: JERK IS NOT YET SUPORTED, SORRY\n");
				return (-999);
			} else {
				print_usage();
				return (-1);
			}
			break;
		case 'x':
			if (optarg) {
				print_settings.x_maxspeed = atof(optarg);
				if (print_settings.x_maxspeed == 0) {
					fprintf(stderr, "Invalid max_x_speed value: %s\n", optarg);
					return (-1);
				}
			} else {
				print_usage();
				return (-1);
			}
			break;
		case 'y':
			if (optarg) {
				print_settings.y_maxspeed = atof(optarg);
				if (print_settings.y_maxspeed == 0) {
					fprintf(stderr, "Invalid max_y_speed value: %s\n", optarg);
					return (-1);
				}
			} else {
				print_usage();
				return (-1);
			}
			break;
		case 'z':
			if (optarg) {
				print_settings.z_maxspeed = atof(optarg);
				if (print_settings.z_maxspeed == 0) {
					fprintf(stderr, "Invalid max_z_speed value: %s\n", optarg);
					return (-1);
				}
			} else {
				print_usage();
				return (-1);
			}
			break;
		case 'r':
			if (optarg) {
				print_settings.rtime = atof(optarg);
				if (print_settings.rtime == 0) {
					fprintf(stderr, "Invalid retraction time value: %s\n",
							optarg);
					return (-1);
				}
			} else {
				print_usage();
				return (-1);
			}
			break;
		case 'p':
			if (optarg) {
				print_settings.ptime = atof(optarg);
				if (print_settings.ptime == 0) {
					fprintf(stderr, "Invalid prime time value: %s\n", optarg);
					return (-1);
				}
			} else {
				print_usage();
				return (-1);
			}
			break;
		case 's':
			if (optarg) {
				pct_step = atof(optarg)/100.0F;
				if (pct_step > 1.0F || pct_step < 0.01F) {
					fprintf(stderr, "Invalid percent step value: %s, please use value between 1 and 100\n", optarg);
					return (-1);
				}
			} else {
				print_usage();
				return (-1);
			}
			break;
		}
	}

  if (gcodefile == NULL) gcodefile = strdup("samples/test.gcode");

  if (NULL == (lb=calloc(LINE_BUFFER_LENGTH,1))){
     fprintf(stderr,"ERROR: Cannot allocate ram for line buffer\n");
     return(-3);
  }

  if (NULL == (f = fopen(gcodefile, "r"))){
      fprintf(stderr,"Cannot open %s for read\n", gcodefile);
      print_usage();
      return(-2);
  }

  if (gcodeout != NULL) {
	  if (NULL == (output_file = fopen(gcodeout, "w"))){
	      fprintf(stderr,"Cannot open %s for write\n", gcodeout);
	      fclose(f);
	      print_usage();
	      return(-2);
	  }
  }


  if (!quiet) print_config(&print_settings);
  next_pct = 1 - pct_step;
  for (pass = 0; pass < (output_file != NULL ?2:1); pass++) {
	    if (pass == 1){
	    	rewind(f);
	    	total_seconds = seconds;
	    	seconds = 0;
	    	fprintf(output_file, "M117 100%% Remaining ");
	    	print_timeleft(output_file, (long int) floor(total_seconds));
     	    fprintf(output_file, "\n");

	    }
		while (fgets(lb, LINE_BUFFER_LENGTH, f)) {
			if (pass == 1) {
				fputs(lb, output_file);
				if (next_pct > 0.01 && (total_seconds - seconds)/total_seconds < next_pct ){
					fprintf(output_file, "M117 %i%% Remaining ", (int) floor(next_pct * 100));
			    	print_timeleft(output_file, (long int) floor(total_seconds - seconds));
			    	fprintf(output_file, "\n");
					next_pct -= pct_step;
				}
			}

			if (comment(lb)) continue;

			switch (gcode(lb)) {
			case GCODE_MOVE:
				seconds += calcmove(lb, &print_settings);
				break;
			case GCODE_DWELL:
				seconds += read_dwell(lb);
				break;
			case GCODE_RETRACT:
				break;
			case GCODE_INCH:
				print_settings.mm = false;
				break;
			case GCODE_MM:
				print_settings.mm = true;
				break;
			case GCODE_ABS:
				print_settings.abs = true;
				break;
			case GCODE_REL:
				print_settings.abs = false;
				break;
			case GCODE_EABS:
				print_settings.eabs = true;
				break;
			case GCODE_EREL:
				print_settings.eabs = false;
				break;
			case GCODE_MAXFEED:
				read_maxfeed(lb, &print_settings);
				break;
			case GCODE_ACCEL:
				read_accel(lb, &print_settings);
				break;
			case GCODE_JDEV:
				read_jdev(lb, &print_settings);
				break;
			case GCODE_RLEN:
				read_rtime(lb, &print_settings);
				break;
			case GCODE_PLEN:
				read_ptime(lb, &print_settings);
				break;
			case GCODE_SPEEDOVER:
				read_speedover(lb, &print_settings);
				break;
			}

		}
  }




  fclose(f);

  if (!quiet){
    long int sec = (long int) floor(seconds);
    fprintf(stdout, "Total time: ");
	print_timeleft(stdout, sec);
    fprintf(stdout, "\n");
  }

  fclose(output_file);
  return 0;
}
