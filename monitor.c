#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/inotify.h>
#include <limits.h>
#include <linux/limits.h>
#include <string.h>
#include <time.h>
#include <errno.h>    // Pour détecter l'erreur d'interruption
#include "monitor.h"
#include "config.h"
#include "utils.h"
#include "signals.h"
#include <poll.h>
#include "stats.h"

#ifndef NAME_MAX
#define NAME_MAX 255
#endif

#define BUF_LEN (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))

void *monitor_thread(void *arg) {
    char *target_dir = (char *)arg;
    char buffer[BUF_LEN];

    int fd = inotify_init();
    if (fd < 0) { perror("Erreur inotify_init"); return NULL; }

    int wd = inotify_add_watch(fd, target_dir, IN_CREATE | IN_DELETE | IN_MODIFY | IN_MOVED_FROM | IN_MOVED_TO);
    if (wd == -1) { perror("Erreur inotify_add_watch"); close(fd); return NULL; }

    printf("[MONITOR] En écoute sur le répertoire : %s\n", target_dir);
    fflush(stdout);

    // On utilise keep_running ici !
    while (keep_running) {
    
        // On vérifie si un signal SIGUSR1 a levé ce drapeau !
        if (print_stats_request) {
            // On affiche les statistiques de manière thread-safe (grâce au mutex dans stats.c)
            stats_print(); 
            
            // On rabaisse le drapeau pour ne pas réafficher les stats à la boucle suivante
            print_stats_request = 0; 
        }
        // Préparation de poll() pour surveiller notre file descriptor inotify (fd)
        struct pollfd pfd = { fd, POLLIN, 0 };
        
        // On attend un événement pendant maximum 500 millisecondes
        int poll_ret = poll(&pfd, 1, 500);

        if (poll_ret < 0) {
            if (errno == EINTR) break; // Interrompu par Ctrl+C
            perror("Erreur poll");
            break;
        } else if (poll_ret == 0) {
            // Le délai de 500ms est écoulé, il ne s'est rien passé.
            // On fait un 'continue' pour remonter au début du while et vérifier 'keep_running' !
            continue;
        }

        // Si on arrive ici, c'est qu'il y a vraiment un événement à lire !
        int length = read(fd, buffer, BUF_LEN);  
        
        if (length < 0) {
            perror("Erreur de lecture inotify");
            break;
        }

        int i = 0;
        while (i < length) {
            struct inotify_event *event = (struct inotify_event *)&buffer[i];

            if (event->len) {
                FileEvent new_event;
                new_event.timestamp = time(NULL);
                strncpy(new_event.filepath, event->name, MAX_PATH_LEN - 1);
                new_event.filepath[MAX_PATH_LEN - 1] = '\0';

                if (event->mask & IN_CREATE) new_event.type = EV_CREATE;
                else if (event->mask & IN_DELETE) new_event.type = EV_DELETE;
                else if (event->mask & IN_MODIFY) new_event.type = EV_MODIFY;
                else if (event->mask & (IN_MOVED_FROM | IN_MOVED_TO)) new_event.type = EV_RENAME;
                else new_event.type = EV_UNKNOWN;

                if(new_event.type != EV_UNKNOWN) {
                    queue_push(&event_queue, new_event);
                }
            }
            i += sizeof(struct inotify_event) + event->len;
        }
    }

    printf("\n[MONITOR] Arrêt de la surveillance...\n");
    inotify_rm_watch(fd, wd);
    close(fd);
    return NULL;
}