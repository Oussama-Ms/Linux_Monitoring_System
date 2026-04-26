#ifndef UTILS_H
#define UTILS_H

#include <time.h>
#include "config.h"

// Types d'événements possibles
typedef enum {
    EV_CREATE,
    EV_DELETE,
    EV_MODIFY,
    EV_RENAME_FROM,
    EV_RENAME_TO,
    EV_UNKNOWN
} EventType;

// Structure de l'événement qui sera envoyée du moniteur vers l'analyseur/logger
typedef struct {
    time_t timestamp;
    EventType type;
    char filepath[MAX_PATH_LEN];
    int is_directory;
} FileEvent;

// Déclaration des fonctions utilitaires
void print_usage(const char *prog_name);
int check_directory_exists(const char *path);

#endif