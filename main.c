#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "config.h"
#include "utils.h"
#include "monitor.h"
#include "logger.h"
#include "signals.h"
#include "stats.h"

EventQueue event_queue;

int main(int argc, char *argv[]) {
    if (argc != 2) { print_usage(argv[0]); return ERR_ARGS; }

    char *target_dir = argv[1];
    if (check_directory_exists(target_dir) != SUCCESS) {
        fprintf(stderr, "Erreur : Impossible d'accéder au répertoire '%s'.\n", target_dir);
        return ERR_DIR_NOT_FOUND;
    }

    printf("Démarrage de SecureBox...\n");

    // 1. Configurer l'interception de Ctrl+C
    setup_signal_handling();

    // 2. Initialiser les statistiques globales
    stats_init();

    queue_init(&event_queue);

    pthread_t logger_tid, monitor_tid;

    if (pthread_create(&logger_tid, NULL, logger_thread, NULL) != 0) {
        perror("Erreur création thread logger"); return 1;
    }
    if (pthread_create(&monitor_tid, NULL, monitor_thread, (void *)target_dir) != 0) {
        perror("Erreur création thread monitor"); return 1;
    }

    // Le main attend sagement que le moniteur termine (ce qui arrive quand on fait Ctrl+C)
    pthread_join(monitor_tid, NULL);

    // Une fois le moniteur arrêté, on réveille de force le Logger pour qu'il s'arrête
    pthread_mutex_lock(&event_queue.mutex);
    pthread_cond_broadcast(&event_queue.cond_non_empty);
    pthread_mutex_unlock(&event_queue.mutex);

    // On attend que le logger termine d'écrire ses derniers fichiers
    pthread_join(logger_tid, NULL);

    printf("SecureBox a été arrêté proprement. Au revoir !\n");
    stats_print();
    return SUCCESS;
}