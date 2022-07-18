OUT_DIR += /src

OBJS += \
$(OUT_PATH)/src/util.o \
$(OUT_PATH)/src/app_adc.o \
$(OUT_PATH)/src/app_att.o \
$(OUT_PATH)/src/app.o \
$(OUT_PATH)/src/ble.o \
$(OUT_PATH)/src/battery.o \
$(OUT_PATH)/src/cfg.o \
$(OUT_PATH)/src/pulse.o \
$(OUT_PATH)/src/main.o

# Each subdirectory must supply rules for building sources it contributes
$(OUT_PATH)/src/%.o: $(SRC_PATH)/%.c
	@echo 'Building file: $<'
	@$(CC) $(GCC_FLAGS) $(INCLUDE_PATHS) -c -o"$@" "$<"