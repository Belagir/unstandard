
# Gabi's Makefile v2.1

# ---------------- Configuration -----------------------------------------------

## Name of the project. This will be the name of the executable placed in the
## executable directory.
PROJECT_NAME = ustd-test
## Root source directory. Contains the c implementation files.
SRC_DIR = src
## Root include directory. Contains the c header files. Passed with -I to
## the compiler.
INC_DIR = inc
## Build diectory. Will contain object and binary files linked in the final
## executable
OBJ_DIR = build
## Executable directory. Contains the final binary file.
EXC_DIR = bin
## resources directory
RESDIR = res

## compiler
CC = gcc
## archiver
AR = ar

## compilation flags
CFLAGS += -Wall -Wextra -Wconversion -Wdangling-pointer -Wparentheses -Wpedantic -Wstringop-overflow -Wnonnull -g --std=c2x
## linker flags
LFLAGS += -lm
## archiver flags
ARFLAGS = rvcs

# additional flags for defines
DFLAGS += -DDEBUG

# --------------- Internal variables -------------------------------------------

## absolute path to executable name
TARGET = $(EXC_DIR)/$(PROJECT_NAME)
## list of all c files without their path
SRC := $(notdir $(shell find $(SRC_DIR) -name *.c))
## list of all duplicate c files to enforce uniqueness of filenames
DUPL_SRC := $(strip $(shell echo $(SRC) | tr ' ' '\n' | sort | uniq -d))
## list of all target object files with their path
OBJ := $(addprefix $(OBJ_DIR)/, $(patsubst %.c, %.o, $(SRC)))

## makefile-managed directories
BUILD_DIRS = $(EXC_DIR) $(OBJ_DIR)

## additional compilation option for includes
ARGS_INCL = -I$(INC_DIR)

# --------------- Make directories searching -----------------------------------

## where to find c files : all unique directories in SRC_DIR which contain a c
## file
vpath %.c $(sort $(dir $(shell find $(SRC_DIR) -name *.c)))

# --------------- Rules --------------------------------------------------------

.PHONY: all check clean count_lines run

# -------- compilation -----------------

all: check $(BUILD_DIRS) $(TARGET) | count_lines

run: all
	./$(TARGET)

$(TARGET): $(OBJ) $(RES_BIN)
	$(CC) $^ -o $@ $(LFLAGS)

$(OBJ_DIR)/%.o: %.c
	$(CC) -c $? -o $@ $(ARGS_INCL) $(CFLAGS) $(DFLAGS)

# -------- dir spawning ----------------

$(BUILD_DIRS):
	mkdir -p $@

# -------- check -----------------------

check:
ifdef DUPL_SRC
	$(error duplicate source filenames: $(DUPL_SRC))
endif

# -------- cleaning --------------------

clean:
	rm -Rf $(BUILD_DIRS)

# -------- luxury ----------------------

count_lines:
ifneq (, $(shell which cloc))
	@cloc --hide-rate --quiet $(SRC_DIR) $(INC_DIR)
endif

# ----  ----  ----  ----  ----  ----

 # Senteurs d'été doux
 # Claquement brefs des touches
 # Segmentation fault
