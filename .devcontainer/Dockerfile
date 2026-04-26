FROM ubuntu:22.04

# Désactiver les prompts interactifs pendant l'installation
ENV DEBIAN_FRONTEND=noninteractive

# Installation de la chaîne de compilation C et des outils de débogage
RUN apt-get update && apt-get install -y \
    build-essential \
    gdb \
    valgrind \
    git \
    make \
    && rm -rf /var/lib/apt/lists/*

# Création d'un utilisateur non-root pour la sécurité
RUN useradd -m -s /bin/bash devuser
USER devuser