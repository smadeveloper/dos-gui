# ============================================================================
# Allegro 4 GUI - Makefile
# ============================================================================

# Compiler
CC = gcc

# Directories
SRCDIR = src
INCDIR = include
OBJDIR_R = obj/release
OBJDIR_D = obj/debug

# Target
TARGET_R = gui_release
TARGET_D = gui_debug

# Source files
SRCS = $(wildcard $(SRCDIR)/*.c)

# Object files
OBJS_R = $(patsubst $(SRCDIR)/%.c, $(OBJDIR_R)/%.o, $(SRCS))
OBJS_D = $(patsubst $(SRCDIR)/%.c, $(OBJDIR_D)/%.o, $(SRCS))

# Flags
CFLAGS_COMMON = -I$(INCDIR) -Wall -Wextra
CFLAGS_R = $(CFLAGS_COMMON) -O2 -DNDEBUG
CFLAGS_D = $(CFLAGS_COMMON) -g -O0 -DDEBUG

# Allegro 4 libraries
LIBS = -lalleg -lm

# ============================================================================
# Default target
# ============================================================================
.PHONY: all release debug clean

all: release

# ============================================================================
# Release build
# ============================================================================
release: $(TARGET_R)

$(TARGET_R): $(OBJS_R)
	$(CC) $(CFLAGS_R) -o $@ $^ $(LIBS)

$(OBJDIR_R)/%.o: $(SRCDIR)/%.c | $(OBJDIR_R)
	$(CC) $(CFLAGS_R) -c $< -o $@

$(OBJDIR_R):
	mkdir -p $(OBJDIR_R)

# ============================================================================
# Debug build
# ============================================================================
debug: $(TARGET_D)

$(TARGET_D): $(OBJS_D)
	$(CC) $(CFLAGS_D) -o $@ $^ $(LIBS)

$(OBJDIR_D)/%.o: $(SRCDIR)/%.c | $(OBJDIR_D)
	$(CC) $(CFLAGS_D) -c $< -o $@

$(OBJDIR_D):
	mkdir -p $(OBJDIR_D)

# ============================================================================
# Clean
# ============================================================================
clean:
	rm -rf obj $(TARGET_R) $(TARGET_D)
