RISCV_FLAGS ?=
LIBS ?=

# Allow users to override the number of time to run a benchmark.
RUNS ?= 1

# Allow users to override the UART's baud rate.
UART_BAUD_RATE ?= 115200

ifeq ($(CHERI),1)
TOOLCHAIN:=LLVM
endif

ifeq ($(TOOLCHAIN),LLVM)
CC      := clang
OBJDUMP := llvm-objdump
OBJCOPY := llvm-objcopy
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
endif

# Make sure user explicitly defines the target GFE platform.
ifeq ($(GFE_TARGET),P1)
ifeq ($(TOOLCHAIN),LLVM)
ifeq ($(CHERI),1)
	RISCV_FLAGS += -target riscv32-unknown-elf -march=rv32imacxcheri -mabi=il32pc64
else
	RISCV_FLAGS += -target riscv32-unknown-elf -march=rv32imac -mabi=ilp32
endif
	LIBS += -lclang_rt.builtins-riscv32
else
	RISCV_FLAGS += -march=rv32imac -mabi=ilp32
endif
	# 50 MHz clock
	CLOCKS_PER_SEC := 50000000
else ifeq ($(GFE_TARGET),P2)
ifeq ($(TOOLCHAIN),LLVM)
ifeq ($(CHERI),1)
	RISCV_FLAGS += -target riscv64-unknown-elf -march=rv64imafdcxcheri -mabi=l64pc128d
else
	RISCV_FLAGS += -target riscv64-unknown-elf -march=rv64imafdc -mabi=lp64d
endif
	LIBS += -lclang_rt.builtins-riscv64
else
	RISCV_FLAGS += -march=rv64imafdc -mabi=lp64d
endif
	# 50 MHz clock
	CLOCKS_PER_SEC := 50000000
else ifeq ($(GFE_TARGET),P3)
$(error P3 target has not been tested yet, use P1 or P2)
else
$(error Please define GFE_TARGET to P1, P2, or P3 (e.g. make GFE_TARGET=P1))
endif

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
	-static \
	-nostdlib \
	-nostartfiles \
	-lm \
	-lc \
	$(LIBS) \
	-T $(LINKER_SCRIPT)

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
