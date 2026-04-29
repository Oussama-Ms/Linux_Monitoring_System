#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "logger.h"
#include "utils.h"
#include "signals.h"
#include "analyzer.h"
#include "stats.h"


const char* get_event_type_str(EventType type) {
    switch (type) {
        case EV_CREATE: return "CREATE FILE";
        case EV_DELETE: return "DELETE FILE";
        case EV_MODIFY: return "MODIFY FILE";
        case EV_RENAME: return "RENAME FILE";
        default: return "UNKNOWN EVENT";
    }
}

void *logger_thread(void *arg) {
    (void)arg;
    
    FILE *log_file = fopen(LOG_FILE_PATH, "a");
    if (!log_file) {
        perror("[LOGGER] Erreur : Impossible de créer/ouvrir le fichier log. Le dossier 'logs/' existe-t-il ?");
        return NULL;
    }

    printf("[LOGGER] Thread de journalisation démarré.\n");
    fflush(stdout);

    // On boucle tant que le programme tourne OU qu'il reste des messages à écrire
    while (keep_running || event_queue.count > 0) {
        FileEvent event = queue_pop(&event_queue);
        
        if (event.type == EV_UNKNOWN && !keep_running) {
            break;
        }

        struct tm *tm_info = localtime(&event.timestamp);
        char date_buffer[26];
        strftime(date_buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);

        // 1. Passage de l'événement dans le moteur d'analyse
        int is_alert = analyze_event(&event);
        const char* level = is_alert ? "ALERTE" : "INFO";

        // 2. Mise à jour des statistiques
        stats_update(event.type, is_alert);

        // 3. Affichage Terminal
        if (is_alert) {
            // Affichage en rouge pour les alertes (Séquence d'échappement ANSI)
            printf("\n\033[1;31m[ALERTE DE SÉCURITÉ]\033[0m Action suspecte : %s\n", get_event_type_str(event.type));
        } else {
            printf("\n[INFO] Action : %s\n", get_event_type_str(event.type));
        }
        printf(" ├── Fichier concerné : %s\n", event.filepath);
        fflush(stdout);

        // 4. Écriture Log
        fprintf(log_file, "[%s] [%s] %s : %s\n", date_buffer, level, get_event_type_str(event.type), event.filepath);
        fflush(log_file);
    }

    printf("[LOGGER] Arrêt propre du thread. Fichier log fermé.\n");
    fclose(log_file);
    return NULL;
}