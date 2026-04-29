#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include "utils.h"
#include <string.h>
#include "signals.h"


// Affiche le mode d'emploi du programme
void print_usage(const char *prog_name) {
    printf("Utilisation : %s <chemin_du_repertoire_a_surveiller>\n", prog_name);
    printf("Exemple     : %s /home/user/test\n", prog_name);
}

// Vérifie si le dossier existe, si c'est bien un dossier, et si on a les droits
int check_directory_exists(const char *path) {
    struct stat path_stat;

    // 1. stat() récupère les informations sur le chemin. Si ça échoue, le chemin n'existe pas.
    if (stat(path, &path_stat) != 0) {
        return ERR_DIR_NOT_FOUND;
    }

    // 2. Vérification : Est-ce que c'est un répertoire ? (Pas un fichier texte, etc.)
    if (!S_ISDIR(path_stat.st_mode)) {
        return ERR_DIR_NOT_FOUND;
    }

    // 3. Vérification des permissions : Droit de lecture (R_OK) et d'exécution (X_OK)
    // Sous Linux, il faut X_OK sur un dossier pour pouvoir "entrer" dedans et le surveiller
    if (access(path, R_OK | X_OK) != 0) {
        return ERR_PERMISSION;
    }

    return SUCCESS;
}


// Initialise les mutex et variables de condition
void queue_init(EventQueue *q) {
    q->head = 0;
    q->tail = 0;
    q->count = 0;
    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->cond_non_empty, NULL);
}

// Le Monitor appelle cette fonction pour ajouter un événement
void queue_push(EventQueue *q, FileEvent event) {
    pthread_mutex_lock(&q->mutex);
    
    // Si la file est pleine, on écrase le plus ancien (logique circulaire basique)
    if (q->count == QUEUE_SIZE) {
        q->head = (q->head + 1) % QUEUE_SIZE;
        q->count--;
    }
    
    q->events[q->tail] = event;
    q->tail = (q->tail + 1) % QUEUE_SIZE;
    q->count++;
    
    // On réveille le Logger qui dort en attendant un événement
    pthread_cond_signal(&q->cond_non_empty);
    pthread_mutex_unlock(&q->mutex);
}

// Le Logger appelle cette fonction pour récupérer un événement (bloquant si vide)
FileEvent queue_pop(EventQueue *q) {
    pthread_mutex_lock(&q->mutex);
    
    // On s'endort TANT QUE la file est vide ET qu'on doit continuer à tourner
    while (q->count == 0 && keep_running) {
        pthread_cond_wait(&q->cond_non_empty, &q->mutex);
    }
    
    FileEvent event;
    // Si on a été réveillé par un Ctrl+C et que la file est vide
    if (!keep_running && q->count == 0) {
        event.type = EV_UNKNOWN; // Valeur "fantôme" pour dire au Logger de quitter
    } else {
        // Comportement normal
        event = q->events[q->head];
        q->head = (q->head + 1) % QUEUE_SIZE;
        q->count--;
    }
    
    pthread_mutex_unlock(&q->mutex);
    return event;
}