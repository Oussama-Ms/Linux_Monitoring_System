#include <stdio.h>
#include "stats.h"

SystemStats global_stats;

void stats_init() {
    global_stats.total_events = 0;
    global_stats.creations = 0;
    global_stats.deletions = 0;
    global_stats.modifications = 0;
    global_stats.renames = 0;
    global_stats.alerts = 0;
    pthread_mutex_init(&global_stats.mutex, NULL);
}

void stats_update(EventType type, int is_alert) {
    pthread_mutex_lock(&global_stats.mutex);
    
    global_stats.total_events++;
    if (is_alert) global_stats.alerts++;

    switch (type) {
        case EV_CREATE: global_stats.creations++; break;
        case EV_DELETE: global_stats.deletions++; break;
        case EV_MODIFY: global_stats.modifications++; break;
        case EV_RENAME: global_stats.renames++; break;
        default: break;
    }
    
    pthread_mutex_unlock(&global_stats.mutex);
}

void stats_print() {
    pthread_mutex_lock(&global_stats.mutex);
    printf("\n================ STATISTIQUES FINALES ================\n");
    printf("Total événements : %d\n", global_stats.total_events);
    printf("Créations        : %d\n", global_stats.creations);
    printf("Suppressions     : %d\n", global_stats.deletions);
    printf("Modifications    : %d\n", global_stats.modifications);
    printf("Renommages       : %d\n", global_stats.renames);
    printf("Alertes          : %d\n", global_stats.alerts);
    printf("======================================================\n");
    pthread_mutex_unlock(&global_stats.mutex);
}