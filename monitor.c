#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/inotify.h>
#include <limits.h>
#include <linux/limits.h> // Ajout crucial pour NAME_MAX sous Ubuntu
#include "monitor.h"
#include "config.h"

// Sécurité : Si NAME_MAX n'est toujours pas défini, on le force à 255
#ifndef NAME_MAX
#define NAME_MAX 255
#endif

#define BUF_LEN (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))

void *monitor_thread(void *arg) {
    char *target_dir = (char *)arg;
    char buffer[BUF_LEN];

    int fd = inotify_init();
    if (fd < 0) {
        perror("Erreur inotify_init");
        return NULL;
    }

    int wd = inotify_add_watch(fd, target_dir, IN_CREATE | IN_DELETE | IN_MODIFY | IN_MOVED_FROM | IN_MOVED_TO);
    if (wd == -1) {
        perror("Erreur inotify_add_watch");
        close(fd);
        return NULL;
    }

    printf("[MONITOR] En écoute sur le répertoire : %s\n", target_dir);
    fflush(stdout); // On force l'affichage immédiat

    while (1) {
        int length = read(fd, buffer, BUF_LEN);  
        
        if (length < 0) {
            perror("Erreur de lecture inotify");
            break;
        }

        int i = 0;
        while (i < length) {
            struct inotify_event *event = (struct inotify_event *)&buffer[i];

            if (event->len) {
                if (event->mask & IN_CREATE) {
                    printf("[ÉVÉNEMENT] Création détectée : %s\n", event->name);
                }
                else if (event->mask & IN_DELETE) {
                    printf("[ÉVÉNEMENT] Suppression détectée : %s\n", event->name);
                }
                else if (event->mask & IN_MODIFY) {
                    printf("[ÉVÉNEMENT] Modification détectée : %s\n", event->name);
                }
                else if (event->mask & (IN_MOVED_FROM | IN_MOVED_TO)) {
                    printf("[ÉVÉNEMENT] Renommage/Déplacement détecté : %s\n", event->name);
                }
            }
            i += sizeof(struct inotify_event) + event->len;
        }
        // IMPORTANT : Forcer l'affichage à l'écran après chaque rafale d'événements
        fflush(stdout);
    }

    inotify_rm_watch(fd, wd);
    close(fd);
    return NULL;
}