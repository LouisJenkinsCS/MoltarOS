# Include directories
INCLUDE := include include/sys stdio stdlib string

# Sources, Headers, and Objects
SOURCES := $(shell find $(INCLUDE) -type f -name \*.c)
HEADERS := $(shell find $(INCLUDE) -type f -name \*.h)
OBJECTS := $(patsubst %.c, %.o, $(SOURCES))

# Extremely long list of compiler warnings...
COMPILER_WARNINGS := \
	-Wall -Wextra -Wshadow -Wpointer-arith -Wcast-align \
	-Wnested-externs -Wwrite-strings -Wredundant-decls  -Winline -Wconversion

# Compiler Flags
CFLAGS := -O2 -g $(COMPILER_WARNINGS) -ffreestanding -fbuiltin -Iinclude -D__IS_MOLTAROS

C_COMPILER := clang -target -i686-elf -march=i686

# List of dependency files generated for each source (.c)
DEPENDENCIES := $(patsubst %.c, %.d, $(C_SOURCES))

-include $(DEPENDENCIES)

.PHONY: all clean

all: libk.a

# Kernel implementation for libc
libk.a: $(OBJECTS) Makefile
	@echo "Building $@..."
	@$(AR) rcs $@ $(OBJECTS)

%.o: %.c Makefile
	@echo "Compiling $<..."
	@$(C_COMPILER) -c $< -o $@ $(CFLAGS) -std=gnu11

clean:
	@echo "Cleaning libc..."
	-@$(RM) libk.a $(OBJECTS) *.o */*.o */*/*.o