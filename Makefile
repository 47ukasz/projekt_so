# Kompilator
CC = gcc

# Flagi kompilatora
CFLAGS = -I./include -Wall -Wextra -g

# Foldery
SRC_DIR = src
BIN_DIR = bin
OBJ_DIR = obj
INCLUDE_DIR = include

# Źródła wspólne
COMMON_SRC = $(SRC_DIR)/pamiec_dzielona.c

# Obiekty wspólne
COMMON_OBJ = $(OBJ_DIR)/pamiec_dzielona.o

# Binarki
KIBIC_BIN = $(BIN_DIR)/kibic
PRACOWNIK_TECH_BIN = $(BIN_DIR)/pracownik_techniczny

# Źródła i obiekty dla kibic
KIBIC_SRC = $(SRC_DIR)/kibic.c
KIBIC_OBJ = $(OBJ_DIR)/kibic.o

# Źródła i obiekty dla pracownik_techniczny
PRACOWNIK_TECH_SRC = $(SRC_DIR)/pracownik_techniczny.c
PRACOWNIK_TECH_OBJ = $(OBJ_DIR)/pracownik_techniczny.o

# Cel domyślny
all: $(BIN_DIR) $(OBJ_DIR) $(KIBIC_BIN) $(PRACOWNIK_TECH_BIN)

# Tworzenie folderów bin i obj, jeśli nie istnieją
$(BIN_DIR) $(OBJ_DIR):
	@mkdir -p $@

# Reguła budowania binarki kibic
$(KIBIC_BIN): $(KIBIC_OBJ) $(COMMON_OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# Reguła budowania binarki pracownik_techniczny
$(PRACOWNIK_TECH_BIN): $(PRACOWNIK_TECH_OBJ) $(COMMON_OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# Reguła kompilacji kibic.o (bez zależności na kibic.h)
$(OBJ_DIR)/kibic.o: $(KIBIC_SRC) $(INCLUDE_DIR)/pamiec_dzielona.h
	$(CC) $(CFLAGS) -c $< -o $@

# Reguła kompilacji pracownik_techniczny.o (bez zależności na pracownik_techniczny.h)
$(OBJ_DIR)/pracownik_techniczny.o: $(PRACOWNIK_TECH_SRC) $(INCLUDE_DIR)/pamiec_dzielona.h
	$(CC) $(CFLAGS) -c $< -o $@

# Reguła kompilacji wspólnego pamiec_dzielona.o
$(COMMON_OBJ): $(COMMON_SRC) $(INCLUDE_DIR)/pamiec_dzielona.h
	$(CC) $(CFLAGS) -c $< -o $@

# Cel clean - usuwa pliki binarne i obiekty
clean:
	rm -rf $(BIN_DIR) $(OBJ_DIR)

# Cel phony - zapobiega konfliktom z plikami o takich nazwach
.PHONY: all clean
