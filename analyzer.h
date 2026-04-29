#ifndef ANALYZER_H
#define ANALYZER_H

#include "utils.h"

// Analyse l'événement et retourne 1 si c'est une ALERTE, 0 si INFO
int analyze_event(FileEvent *event);

#endif