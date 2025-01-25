CC = gcc

CFLAGS = -I./include -Wall -Wextra -g -pthread

LDFLAGS = -pthread -lm

DEPFLAGS = -MMD -MP

SRC_DIR = src
BIN_DIR = bin
OBJ_DIR = obj
INCLUDE_DIR = include

SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES = $(SRC_FILES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
DEP_FILES = $(OBJ_FILES:.o=.d)

KIBIC_BIN = $(BIN_DIR)/kibic
PRACOWNIK_TECH_BIN = $(BIN_DIR)/pracownik_techniczny
KIEROWNIK_BIN = $(BIN_DIR)/kierownik

all: $(BIN_DIR) $(OBJ_DIR) $(KIBIC_BIN) $(PRACOWNIK_TECH_BIN) $(KIEROWNIK_BIN)

$(BIN_DIR) $(OBJ_DIR):
	@mkdir -p $@

$(KIBIC_BIN): $(OBJ_DIR)/kibic.o $(OBJ_DIR)/pamiec_dzielona.o $(OBJ_DIR)/semafory.o $(OBJ_DIR)/watki.o $(OBJ_DIR)/kolejka_komunikatow.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

$(PRACOWNIK_TECH_BIN): $(OBJ_DIR)/pracownik_techniczny.o $(OBJ_DIR)/pamiec_dzielona.o $(OBJ_DIR)/semafory.o $(OBJ_DIR)/watki.o $(OBJ_DIR)/kolejka_komunikatow.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

$(KIEROWNIK_BIN): $(OBJ_DIR)/kierownik.o $(OBJ_DIR)/pamiec_dzielona.o $(OBJ_DIR)/semafory.o $(OBJ_DIR)/watki.o $(OBJ_DIR)/kolejka_komunikatow.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(DEPFLAGS) -c $< -o $@

-include $(DEP_FILES)

clean:
	rm -rf $(BIN_DIR) $(OBJ_DIR)

run_k: $(KIBIC_BIN)
	./$(KIBIC_BIN)

run_pt: $(PRACOWNIK_TECH_BIN)
	./$(PRACOWNIK_TECH_BIN)

run_ki: $(KIEROWNIK_BIN)
	./$(KIEROWNIK_BIN)

.PHONY: all clean run_k run_pt run_ki
