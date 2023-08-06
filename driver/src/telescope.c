/*
Fichier : lib.c
Auteur : Tanguy Dietrich
Description :
*/
#include "telescope.h"
#include <stdio.h>
#include <stdlib.h>


void telescope_free_response(telescope_response_t *response) {
    if (response->additionalInfo != NULL) {
        free(response->additionalInfo);
    }
}

void telescope_double_to_dms(double value, double *d, double *m, double *s) {
    *d = fabs(value);
	*m = 60.0 * (*d - floor(*d));
	*s = 60.0 * (*m - floor(*m));
}

// convert a string of the form HH:MM:SS# to a double
bool telescope_stringHMS_to_hms(char *stringHMS, double *h, double *m, double *s) {
    return sscanf(stringHMS, "%d:%d:%d", h, m, s) == 3;
}

double telescope_hms_to_double(double h, double m, double s) {
    return h + m / 60.0 + s / 3600.0;
}