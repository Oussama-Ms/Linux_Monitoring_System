#ifndef UTILS_H
#define UTILS_H

#include <pthread.h>
#include <time.h>
#include "config.h"

// Types d'événements possibles
typedef enum {
    EV_CREATE,
    EV_DELETE,
    EV_MODIFY,
    EV_RENAME,
    EV_UNKNOWN
} EventType;

// Structure de l'événement
typedef struct {
    time_t timestamp;
    EventType type;
    char filepath[MAX_PATH_LEN];
} FileEvent;

// File d'attente circulaire sécurisée (Thread-safe)
#define QUEUE_SIZE 256

typedef struct {
    FileEvent events[QUEUE_SIZE];
    int head;
    int tail;
    int count;
    pthread_mutex_t mutex;
    pthread_cond_t cond_non_empty;
} EventQueue;

// Déclaration de la file globale (sera instanciée dans main.c)
extern EventQueue event_queue;

// Fonctions utilitaires
void print_usage(const char *prog_name);
int check_directory_exists(const char *path);

// Fonctions de la file d'attente
void queue_init(EventQueue *q);
void queue_push(EventQueue *q, FileEvent event);
FileEvent queue_pop(EventQueue *q);

#endif