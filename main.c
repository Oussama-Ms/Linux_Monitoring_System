#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "config.h"
#include "utils.h"
#include "monitor.h" // Ne pas oublier d'inclure le header !

int main(int argc, char *argv[]) {
    if (argc != 2) {
        print_usage(argv[0]);
        return ERR_ARGS;
    }

    char *target_dir = argv[1];

    if (check_directory_exists(target_dir) != SUCCESS) {
        fprintf(stderr, "Erreur : Impossible d'accéder au répertoire '%s'.\n", target_dir);
        return ERR_DIR_NOT_FOUND;
    }

    printf("Démarrage de SecureBox...\n");

    // Identifiant du thread
    pthread_t monitor_tid;

    // Lancement du thread de surveillance
    if (pthread_create(&monitor_tid, NULL, monitor_thread, (void *)target_dir) != 0) {
        perror("Erreur lors de la création du thread de surveillance");
        return 1;
    }

    // On dit au processus principal (main) d'attendre que le thread se termine
    // (Comme le thread a une boucle infinie, le programme va rester en vie)
    pthread_join(monitor_tid, NULL);

    return SUCCESS;
}