#define _GNU_SOURCE // Pour pouvoir utiliser strsignal() dans les handlers de signaux
#include <stdio.h>
#include <stdlib.h>
#include "signals.h"

// Initialisation de nos drapeaux (flags) de contrôle
volatile sig_atomic_t keep_running = 1;
volatile sig_atomic_t print_stats_request = 0;

// Ce handler sera appelé si l'utilisateur fait Ctrl+C (SIGINT) 
// ou utilise la commande 'kill <PID>' (SIGTERM)
void handle_sigint_sigterm(int sig) {
    (void)sig; // On ignore le paramètre 'sig' pour éviter un avertissement du compilateur
    
    // On indique au reste du programme qu'il est temps de faire ses valises
    keep_running = 0; 
}

// Ce handler sera appelé uniquement lorsqu'on enverra le signal utilisateur SIGUSR1
void handle_sigusr1(int sig) {
    (void)sig;
    
    // On lève le drapeau ! On ne fait pas de printf ici car c'est instable 
    // à l'intérieur d'une routine d'interruption système.
    print_stats_request = 1; 
}

// Fonction appelée par main() pour configurer le comportement du noyau Linux face à nos processus
void setup_signal_handling() {
    struct sigaction sa_stop, sa_usr1;

    // 1. Configuration pour l'arrêt propre (SIGINT et SIGTERM)
    sa_stop.sa_handler = handle_sigint_sigterm; // La fonction à appeler
    sa_stop.sa_flags = 0;                       // Pas d'options spéciales
    sigemptyset(&sa_stop.sa_mask);              // On ne bloque pas d'autres signaux pendant l'exécution du handler
    
    // On abonne notre programme à SIGINT et SIGTERM
    if (sigaction(SIGINT, &sa_stop, NULL) == -1 || sigaction(SIGTERM, &sa_stop, NULL) == -1) {
        perror("Erreur lors de la configuration de l'arrêt (SIGINT/SIGTERM)");
        exit(EXIT_FAILURE);
    }

    // 2. Configuration pour la consultation des statistiques (SIGUSR1)
    sa_usr1.sa_handler = handle_sigusr1;
    sa_usr1.sa_flags = 0;
    sigemptyset(&sa_usr1.sa_mask);
    
    // On abonne notre programme à SIGUSR1
    if (sigaction(SIGUSR1, &sa_usr1, NULL) == -1) {
        perror("Erreur lors de la configuration de SIGUSR1");
        exit(EXIT_FAILURE);
    }
}