
# Gabi's Makefile v2.1

# ---------------- Configuration -----------------------------------------------

## Root source directory. Contains the c implementation files.
SRC_DIR = src
## Root include directory. Contains the c header files. Passed with -I to
## the compiler.
INC_DIR = inc
## Build diectory. Will contain object and binary files linked in the final
## executable
OBJ_DIR = build
## resources directory
RESDIR = res

## compiler
CC = gcc
## resource packer
RESPACKER = ld

## compilation flags
CFLAGS += -Wextra -Wconversion -Wdangling-pointer -Wparentheses -Wpedantic -Wstringop-overflow -Wnonnull -g -std=c2x
## linker flags
LFLAGS += -lm
# resource packing flags
RESFLAGS = -r -b binary -z noexecstack

# additional flags for defines
DFLAGS +=

# --------------- Internal variables -------------------------------------------

## list of all c files without their path
SRC := $(notdir $(shell find $(SRC_DIR) -name *.c))
## list of all duplicate c files to enforce uniqueness of filenames
DUPL_SRC := $(strip $(shell echo $(SRC) | tr ' ' '\n' | sort | uniq -d))
## list of all target object files with their path
OBJ := $(addprefix $(OBJ_DIR)/, $(patsubst %.c, %.o, $(SRC)))
## list of all resources files without their directory
RES := $(notdir $(shell find $(RESDIR)/ -type f))
## list of all target binaries resource files to include in the binary
RES_BIN := $(addprefix $(OBJ_DIR)/, $(addsuffix .resbin, $(RES)))
## list of all duplicate resource files to enforce uniqueness of filenames
DUPL_RES := $(strip $(shell echo $(RES) | tr ' ' '\n' | sort | uniq -d))

## makefile-managed directories
BUILD_DIRS = $(OBJ_DIR)

## additional compilation option for includes
ARGS_INCL = -I$(INC_DIR)

# --------------- Make directories searching -----------------------------------

## where to find c files : all unique directories in SRC_DIR which contain a c
## file
vpath %.c $(sort $(dir $(shell find $(SRC_DIR) -name *.c)))

# --------------- Rules --------------------------------------------------------

.PHONY: all check clean count_lines run

# -------- compilation -----------------

all: check $(BUILD_DIRS) $(OBJ) | count_lines

$(OBJ_DIR)/%.o: %.c
	$(CC) -c $? -o $@ $(ARGS_INCL) $(CFLAGS) $(DFLAGS)

$(OBJ_DIR)/%.resbin: $(RESDIR)/%
	$(RESPACKER) $(RESFLAGS) $? -o $@

# -------- dir spawning ----------------

$(BUILD_DIRS):
	mkdir -p $@

# -------- check -----------------------

check:
ifdef DUPL_SRC
	$(error duplicate source filenames: $(DUPL_SRC))
endif
ifdef DUPL_RES
	$(error duplicate resource filenames: $(DUPL_RES))
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
