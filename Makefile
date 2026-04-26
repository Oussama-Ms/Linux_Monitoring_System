CC = gcc
CFLAGS = -Wall -Wextra -pthread -g -O2
INCLUDES = -I./

# Liste des fichiers sources
SRCS = main.c monitor.c logger.c analyzer.c stats.c signals.c utils.c
# Fichiers objets correspondants
OBJS = $(SRCS:.c=.o)

# Nom de l'exécutable
TARGET = securebox

# Règle par défaut
all: $(TARGET)

# Compilation de l'exécutable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET) $(OBJS)

# Compilation des fichiers objets
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Nettoyage
clean:
	rm -f $(OBJS) $(TARGET)

# Pour éviter les conflits avec des fichiers nommés "clean" ou "all"
.PHONY: all clean