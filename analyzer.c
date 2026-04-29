#include <string.h>
#include <time.h>
#include "analyzer.h"

#define MAX_DELETIONS_WINDOW 5
#define DELETION_TIME_LIMIT 10 // en secondes

// Tableau statique pour mémoriser l'historique des suppressions
static time_t deletion_timestamps[MAX_DELETIONS_WINDOW] = {0};
static int deletion_index = 0;

int analyze_event(FileEvent *event) {
    int is_suspicious = 0;

    // Extraction du nom de fichier (sans le chemin complet)
    char *filename = strrchr(event->filepath, '/');
    if (filename == NULL) {
        filename = event->filepath; // Pas de slash trouvé
    } else {
        filename++; // Avancer d'un caractère pour ignorer le '/'
    }

    // Règle 1 : Création d'un fichier caché
    if (event->type == EV_CREATE && filename[0] == '.') {
        is_suspicious = 1;
    }

    // Règle 2 : Extension dangereuse (ex: .sh pour un script bash)
    if (event->type == EV_CREATE || event->type == EV_RENAME) {
        char *ext = strrchr(filename, '.');
        if (ext != NULL) {
            if (strcmp(ext, ".sh") == 0 || strcmp(ext, ".exe") == 0 || strcmp(ext, ".bin") == 0) {
                is_suspicious = 1;
            }
        }
    }

    // Règle 3 : Algorithme de fenêtre glissante pour les suppressions rapides
    if (event->type == EV_DELETE) {
        deletion_timestamps[deletion_index] = event->timestamp;
        deletion_index = (deletion_index + 1) % MAX_DELETIONS_WINDOW;

        // Vérification de la durée entre la suppression la plus ancienne et la nouvelle
        time_t oldest = deletion_timestamps[deletion_index]; 
        if (oldest != 0 && (event->timestamp - oldest) <= DELETION_TIME_LIMIT) {
            is_suspicious = 1;
        }
    }

    return is_suspicious;
}