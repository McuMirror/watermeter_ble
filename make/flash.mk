################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 

OUT_DIR += \
/drivers/8258/flash


OBJS += \
$(OUT_PATH)/drivers/8258/flash/flash_mid1060c8.o \
$(OUT_PATH)/drivers/8258/flash/flash_mid1360c8.o \
$(OUT_PATH)/drivers/8258/flash/flash_mid1360eb.o \
$(OUT_PATH)/drivers/8258/flash/flash_mid1460c8.o \
$(OUT_PATH)/drivers/8258/flash/flash_mid011460c8.o \
$(OUT_PATH)/drivers/8258/flash/flash_mid13325e.o \
$(OUT_PATH)/drivers/8258/flash/flash_mid14325e.o \
$(OUT_PATH)/drivers/8258/flash/flash_mid134051.o \
$(OUT_PATH)/drivers/8258/flash/flash_mid136085.o

# Each subdirectory must supply rules for building sources it contributes
$(OUT_PATH)/drivers/8258/flash/%.o: $(SDK_PATH)/drivers/8258/flash/%.c
	@echo 'Building file: $<'
	@$(CC) $(GCC_FLAGS) $(INCLUDE_PATHS) -c -o"$@" "$<"

