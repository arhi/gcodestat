/*
 * readconfig.c
 *
 *  Created on: Dec 2, 2017
 *      Author: arhimed
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "gcodestat.h"

#define MAXLINELENGTH 150 //max config line for smoothieware cannot be longer then 132 chars

int get_config_value(char *lb, char* name, double *val) {
	char *p, *q;
	int found;

	found = -1;
	p = strstr(lb, name);
	if (p != NULL) {
		q = strchr(lb, '#');
		if ((q == NULL) || (p < q)) {
			if ((p - lb) == 0) {
				found = 0;
			} else if (p[-1] == ' ') {
				found = 0;
			}
		}
	}
	if (found == 0) {
		*val = atof(p + strlen(name));
	}
	return (found);
}

/*
 * read_config
 * http://smoothieware.org/configuration-options
 */
int read_config(char * filename, print_settings_t * print_settings) {
	FILE *f;
	char *lb;

	if (NULL == (f = fopen(filename, "r"))) {
		fprintf(stderr, "Cannot open %s for read\n", filename);
		return (-1);
	}

	if (NULL == (lb = calloc(MAXLINELENGTH, 1))) {
		fprintf(stderr, "ERROR: Cannot allocate ram for line buffer\n");
		fclose(f);
		return (-1);
	}

	while (fgets(lb, MAXLINELENGTH, f)) {
		get_config_value(lb, "acceleration ", &(print_settings->accel));
		get_config_value(lb, "junction_deviation ", &(print_settings->jdev));
		get_config_value(lb, "x_axis_max_speed ", &(print_settings->x_maxspeed));
		get_config_value(lb, "y_axis_max_speed ", &(print_settings->y_maxspeed));
		get_config_value(lb, "z_axis_max_speed ", &(print_settings->z_maxspeed));
	}

	fclose(f);
	return (0);
}
