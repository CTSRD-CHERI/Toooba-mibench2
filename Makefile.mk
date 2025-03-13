\RISCV_FLAGS ?=
LIBS ?=

# Allow users to override the number of time to run a benchmark.
RUNS ?= 1

# Allow users to override the UART's baud rate.
UART_BAUD_RATE ?= 115200

ifeq ($(CHERI),1)
TOOLCHAIN:=LLVM
endif

#CCDIR   ?= /Users/jonathanwoodruff/cheribuild/llvm-build/bin
ifeq ($(TOOLCHAIN),LLVM)
CC      := CCDIR/clang
OBJDUMP := CCDIR/llvm-objdump
OBJCOPY := CCDIR/llvm-objcopy
TOOLCHAIN_LINKER_FLAGS := -fuse-ld=lld
ifeq ($(GFE_TARGET),P1)
SYSROOT_DIR=/opt/riscv-llvm/riscv32-unknown-elf/
else
SYSROOT_DIR=/opt/riscv-llvm/riscv64-unknown-elf/
endif # sysroot set


RISCV_FLAGS += -mcmodel=medium -mno-relax --sysroot=$(SYSROOT_DIR)
ifndef SYSROOT_DIR
$(error PLEASE define SYSROOT_DIR to where libc and run-time libs are installed)
endif
else # GCC
CC      := riscv64-unknown-elf-gcc
OBJDUMP := riscv64-unknown-elf-objdump
OBJCOPY := riscv64-unknown-elf-objcopy
RISCV_FLAGS += -mcmodel=medany
LIBS := -lgcc
TOOLCHAIN_LINKER_FLAGS =
endif

# Make sure user explicitly defines the target GFE platform.
ifeq ($(TOOLCHAIN),LLVM)
ifeq ($(CHERI),1)
  RISCV_FLAGS += -target riscv64 -march=rv64imafdcxcheri -mabi=l64pc128d
else
  RISCV_FLAGS += -target riscv64 -march=rv64imafdc -mabi=lp64
endif
  LIBS += -lc -lclang_rt.builtins-riscv64
else
  RISCV_FLAGS += -march=rv64imafdc -mabi=lp64d
endif
# 25 MHz clock
CLOCKS_PER_SEC := 25000000

# Define sources and compilation outputs.
COMMON_DIR := ..
LINKER_SCRIPT := $(COMMON_DIR)/test.ld
COMMON_ASM_SRCS := \
	$(COMMON_DIR)/crt.S
COMMON_C_SRCS := \
	$(COMMON_DIR)/syscalls.c \
	$(COMMON_DIR)/uart_16550.c \
	$(COMMON_DIR)/cvt.c \
	$(COMMON_DIR)/ee_printf.c
COMMON_OBJS := \
	$(patsubst %.c,%.o,$(notdir $(COMMON_C_SRCS))) \
	$(patsubst %.S,%.o,$(notdir $(COMMON_ASM_SRCS)))
OBJS := $(COMMON_OBJS) $(OBJS)

# Define compile and load/link flags.
CFLAGS := \
	$(RISCV_FLAGS) \
	-DBARE_METAL \
	-DCLOCKS_PER_SEC=$(CLOCKS_PER_SEC) \
	-DHAS_FLOAT=1 \
	-DRUNS=$(RUNS) \
	-DUART_BAUD_RATE=$(UART_BAUD_RATE) \
	-Dprintf=ee_printf \
	-O2 \
	-Wall \
	-static \
	-std=gnu99 \
	-ffast-math \
	-fno-common \
	-fno-builtin-printf \
	-I$(COMMON_DIR)
ASFLAGS := $(CFLAGS)
LDFLAGS := \
	-v \
	-static \
	-nostdlib \
	-nostartfiles \
	-lm \
	-lc \
	$(LIBS) \
	-T $(LINKER_SCRIPT) \
	$(TOOLCHAIN_LINKER_FLAGS)

all: main.elf

%.o: %.s
	$(CC) $(ASFLAGS) -c -o $@ $<

%.o: ../%.S
	$(CC) $(ASFLAGS) -c -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: ../%.c
	$(CC) $(CFLAGS) -c -o $@ $<

main.elf: $(OBJS) $(COMMON_C_SRCS) $(COMMON_ASM_SRCS)
	$(CC) $(CFLAGS) $(OBJS) -o main.elf $(LDFLAGS)
	$(OBJDUMP) --disassemble-all main.elf > main.lst
	$(OBJCOPY) main.elf main.bin -O binary

clean: more_clean
	rm -rf *.o *.elf output* *.lst *.bin *~
