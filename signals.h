#ifndef SIGNALS_H
#define SIGNALS_H

#include <signal.h>

// Variable globale pour indiquer l'arrêt du programme (1 = tourne, 0 = arrête)
// Le type 'volatile sig_atomic_t' garantit que la variable ne sera pas 
// mise en cache par le compilateur et qu'elle peut être modifiée par un signal en toute sécurité.
extern volatile sig_atomic_t keep_running;

// NOUVEAU : Drapeau pour demander l'affichage des statistiques à la demande
// 0 = Rien à signaler, 1 = L'utilisateur a envoyé SIGUSR1
extern volatile sig_atomic_t print_stats_request;

// Déclaration de la fonction qui va "brancher" nos signaux au système d'exploitation
void setup_signal_handling();

#endif