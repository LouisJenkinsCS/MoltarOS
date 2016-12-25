# Include directories
INCLUDE := kernel x86 drivers

# List of all headers from the included directories
HEADERS := $(shell find $(INCLUDE) -type f -name \*.h)

# List of all sources (.c and .asm) from the included directories
C_SOURCES := $(shell find $(INCLUDE) -type f -name \*.c)
ASM_SOURCES := $(shell find $(INCLUDE) -type f -name \*.asm)
SOURCES := $(C_SOURCES) $(ASM_SOURCES)

# List of the output (.o) files from the compiled sources (.c and .asm)
C_OBJECTS := $(patsubst %.c, %.o, $(C_SOURCES))
ASM_OBJECTS := $(patsubst %.asm, %.o, $(ASM_SOURCES))
OBJECTS := $(C_OBJECTS) $(ASM_OBJECTS)

# List of dependency files generated for each source (.c)
DEPENDENCIES := $(patsubst %.c, %.d, $(C_SOURCES))

# Linker to be used
LINKER := x86/linker.ld

# Compilers used during specific build sections
LINKER_COMPILER := i686-elf-gcc -static
C_COMPILER := clang -target -i686-elf -march=i686
NASM_COMPILER := nasm -felf32

# List of all files (.h, .c, .asm) that will be distributed when compressed (.tar)
ALL_FILES := $(HEADERS) $(SOURCES)

COMPILER_WARNINGS := \
	-Wall -Wextra -Wshadow -Wpointer-arith -Wcast-align \
	-Wnested-externs -Wwrite-strings -Wredundant-decls  -Winline -Wconversion

CFLAGS := -g -std=gnu11 $(COMPILER_WARNINGS) -fbuiltin -Iinclude -D__IS_MOLTAROS
LINKER_FLAGS := -ffreestanding -O2 -nostdlib -lgcc $(COMPILER_WARNINGS) -fbuiltin -D__IS_MOLTAROS
LIBC := ../libc/libk.a

-include $(DEPENDENCIES)


# Target to build all of the kernel
all: MoltarOS.kernel

# Target to build the kernel
MoltarOS.kernel: $(LINKER) $(OBJECTS) $(LIBC) Makefile
	@echo "Building Kernel image..."
	@$(LINKER_COMPILER) $(LINKER_FLAGS) -T $(LINKER) -o $@ $(OBJECTS) $(LIBC)

# Converts source file (.c) into object file (.o)
# MMD - Generate depedency file (.d) to make sensitive to changes in files it depends on
# MP - Avoids errors when header files are removed from filesystem
# $< - First dependency in the list... I.E, for Kernel.o, $< = Kernel.c
# $@ - The name of the target... I.E, for Kernel.o, $@ = Kernel.o
%.o: %.c Makefile
	@echo "Compiling $<..."
	@$(C_COMPILER) -I$(shell pwd) $(CFLAGS) -c $< -o $@

%.o: %.asm Makefile
	@echo "Compiling $<..."
	@$(NASM_COMPILER) $< -o $@

# Disable checking for files with names of targets we declare to prevent collisions
.PHONY: all clean

clean:
	@echo "Cleaning up Kernel files and image..."
	-@$(RM) $(wildcard $(OBJECTS) $(DEPENDENCIES) MoltarOS.kernel)