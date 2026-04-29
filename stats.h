#ifndef STATS_H
#define STATS_H

#include <pthread.h>
#include "utils.h"

// Structure pour stocker les statistiques globales
typedef struct {
    int total_events;
    int creations;
    int deletions;
    int modifications;
    int renames;
    int alerts;
    pthread_mutex_t mutex;
} SystemStats;

// Variable globale partagée
extern SystemStats global_stats;

void stats_init();
void stats_update(EventType type, int is_alert);
void stats_print();

#endif