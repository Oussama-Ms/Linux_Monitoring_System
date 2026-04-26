#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "utils.h"

int main(int argc, char *argv[]) {
    // 1. Vérification des arguments
    if (argc != 2) {
        print_usage(argv[0]);
        return ERR_ARGS;
    }

    const char *target_dir = argv[1];

    // 2. Vérification de l'existence et des droits du répertoire [cite: 79, 81]
    if (check_directory_exists(target_dir) != SUCCESS) {
        fprintf(stderr, "Erreur : Impossible d'accéder au répertoire '%s'. Vérifiez qu'il existe et que vous avez les droits de lecture.\n", target_dir);
        return ERR_DIR_NOT_FOUND;
    }

    printf("Démarrage de SecureBox...\n");
    printf("Surveillance du répertoire : %s\n", target_dir);

    // TODO: Initialiser les Mutex et les Variables de Condition
    // TODO: Lancer le thread de Journalisation (logger.c)
    // TODO: Lancer le thread d'Analyse (analyzer.c)
    // TODO: Lancer le thread de Surveillance avec inotify (monitor.c)
    
    // TODO: Boucle principale ou attente des threads (pthread_join)

    return SUCCESS;
}