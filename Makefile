# Kompilator
CC = gcc

# Flagi kompilatora
CFLAGS = -I./include -Wall -Wextra -g

# Flagi do generowania zależności
DEPFLAGS = -MMD -MP

# Foldery
SRC_DIR = src
BIN_DIR = bin
OBJ_DIR = obj
INCLUDE_DIR = include

# Źródła
SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES = $(SRC_FILES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
DEP_FILES = $(OBJ_FILES:.o=.d)

# Binarki
KIBIC_BIN = $(BIN_DIR)/kibic
PRACOWNIK_TECH_BIN = $(BIN_DIR)/pracownik_techniczny

# Cel domyślny
all: $(BIN_DIR) $(OBJ_DIR) $(KIBIC_BIN) $(PRACOWNIK_TECH_BIN)

# Tworzenie folderów bin i obj, jeśli nie istnieją
$(BIN_DIR) $(OBJ_DIR):
	@mkdir -p $@

# Reguła budowania binarki kibic
$(KIBIC_BIN): $(OBJ_DIR)/kibic.o $(OBJ_DIR)/pamiec_dzielona.o $(OBJ_DIR)/semafory.o
	$(CC) $(CFLAGS) -o $@ $^

# Reguła budowania binarki pracownik_techniczny
$(PRACOWNIK_TECH_BIN): $(OBJ_DIR)/pracownik_techniczny.o $(OBJ_DIR)/pamiec_dzielona.o $(OBJ_DIR)/semafory.o
	$(CC) $(CFLAGS) -o $@ $^

# Reguła kompilacji plików .c na .o z generowaniem zależności
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(DEPFLAGS) -c $< -o $@

# Ładowanie zależności
-include $(DEP_FILES)

# Cel clean - usuwa pliki binarne, obiektowe i zależności
clean:
	rm -rf $(BIN_DIR) $(OBJ_DIR)

# Cel uruchamiania binarek
run_k: $(KIBIC_BIN)
	./$(KIBIC_BIN)

run_pt: $(PRACOWNIK_TECH_BIN)
	./$(PRACOWNIK_TECH_BIN)

# Cel phony - zapobiega konfliktom z plikami o takich nazwach
.PHONY: all clean run_k run_pt
