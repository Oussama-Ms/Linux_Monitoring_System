#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include "utils.h"

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