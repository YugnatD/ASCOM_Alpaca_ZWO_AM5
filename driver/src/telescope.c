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