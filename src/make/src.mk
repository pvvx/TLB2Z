
OUT_DIR += $(SRC_DIR) \
$(SRC_DIR)/patch_sdk \
$(SRC_DIR)/custom_zcl

OBJS += \
$(OUT_PATH)$(SRC_DIR)/patch_sdk/irq_handler.o \
$(OUT_PATH)$(SRC_DIR)/patch_sdk/flash.o \
$(OUT_PATH)$(SRC_DIR)/patch_sdk/flash_drv.o \
$(OUT_PATH)$(SRC_DIR)/patch_sdk/adc_drv.o \
$(OUT_PATH)$(SRC_DIR)/patch_sdk/random.o \
$(OUT_PATH)$(SRC_DIR)/patch_sdk/hw_drv.o \
$(OUT_PATH)$(SRC_DIR)/patch_sdk/i2c_drv.o \
$(OUT_PATH)$(SRC_DIR)/patch_sdk/cstartup_8258.o \
$(OUT_PATH)$(SRC_DIR)/patch_sdk/drv_nv.o \
$(OUT_PATH)$(SRC_DIR)/sws_printf.o \
$(OUT_PATH)$(SRC_DIR)/app_BLESlave.o \
$(OUT_PATH)$(SRC_DIR)/app_EpCfg.o \
$(OUT_PATH)$(SRC_DIR)/app_main.o \
$(OUT_PATH)$(SRC_DIR)/app_pm.o \
$(OUT_PATH)$(SRC_DIR)/app_ui.o \
$(OUT_PATH)$(SRC_DIR)/battery.o \
$(OUT_PATH)$(SRC_DIR)/ble_cmd_parser.o \
$(OUT_PATH)$(SRC_DIR)/ble_scaning.o \
$(OUT_PATH)$(SRC_DIR)/ccm.o \
$(OUT_PATH)$(SRC_DIR)/drv_uart.o \
$(OUT_PATH)$(SRC_DIR)/flash_eep.o \
$(OUT_PATH)$(SRC_DIR)/main.o \
$(OUT_PATH)$(SRC_DIR)/zb_appCb.o \
$(OUT_PATH)$(SRC_DIR)/zb_ble_switch.o \
$(OUT_PATH)$(SRC_DIR)/zb_reporting.o \
$(OUT_PATH)$(SRC_DIR)/zcl_onOffCb.o \
$(OUT_PATH)$(SRC_DIR)/zcl_EpCb.o \
$(OUT_PATH)$(SRC_DIR)/custom_zcl/zcl_relative_humidity.o \
$(OUT_PATH)$(SRC_DIR)/custom_zcl/zcl_thermostat_ui_cfg.o

#$(OUT_PATH)$(SRC_DIR)/app_bleCmdHandle.o 

# Each subdirectory must supply rules for building sources it contributes
$(OUT_PATH)$(SRC_DIR)/%.o: $(PROJECT_PATH)$(SRC_DIR)/%.c
	@echo 'Building file: $<'
	@$(TC32_PATH)tc32-elf-gcc $(GCC_FLAGS) $(INCLUDE_PATHS) -c -o"$@" "$<"
	
$(OUT_PATH)$(SRC_DIR)/%.o: $(PROJECT_PATH)$(SRC_DIR)/%.S
	@echo 'Building file: $<'
	@$(TC32_PATH)tc32-elf-gcc $(GCC_FLAGS) $(ASM_FLAGS) $(INCLUDE_PATHS) -c -o"$@" "$<"