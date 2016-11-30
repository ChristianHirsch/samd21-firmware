include_directories(
  ../lib/FreeRTOS/include
  ../lib/FreeRTOS/portable/GCC/ARM_CM0
  config
  .

  ../lib/ASF
  ../lib/ASF/common/boards
  ../lib/ASF/common/boards/user_board
  ../lib/ASF/common/boards/user_board/board_config
  ../lib/ASF/common/services/serial
  ../lib/ASF/common/services/serial/sam0_usart
  ../lib/ASF/common/utils
  ../lib/ASF/common/utils/interrupt
  ../lib/ASF/sam0/boards/dummy
  ../lib/ASF/sam0/drivers/port
  ../lib/ASF/sam0/drivers/port/quick_start
  ../lib/ASF/sam0/drivers/sercom
  ../lib/ASF/sam0/drivers/sercom/usart
  ../lib/ASF/sam0/drivers/sercom/usart/quick_start
  ../lib/ASF/sam0/drivers/sercom/usart/quick_start_callback
  ../lib/ASF/sam0/drivers/system
  ../lib/ASF/sam0/drivers/system/clock
  ../lib/ASF/sam0/drivers/system/clock/clock_samd21_r21_da/
  ../lib/ASF/sam0/drivers/system/clock/clock_samd21_r21_da/module_config/
  ../lib/ASF/sam0/drivers/system/clock/quick_start_clock
  ../lib/ASF/sam0/drivers/system/clock/quick_start_gclk
  ../lib/ASF/sam0/drivers/system/interrupt
  ../lib/ASF/sam0/drivers/system/interrupt/quick_start
  ../lib/ASF/sam0/drivers/system/interrupt/system_interrupt_samd21
  ../lib/ASF/sam0/drivers/system/pinmux
  ../lib/ASF/sam0/drivers/system/pinmux/quick_start
  ../lib/ASF/sam0/drivers/system/power/power_sam_d_r
  ../lib/ASF/sam0/drivers/system/reset/reset_sam_d_r
  ../lib/ASF/sam0/utils
  ../lib/ASF/sam0/utils/cmsis/samd21/include
  ../lib/ASF/sam0/utils/cmsis/samd21/include/component
  ../lib/ASF/sam0/utils/cmsis/samd21/include/instance
  ../lib/ASF/sam0/utils/cmsis/samd21/include/pio
  ../lib/ASF/sam0/utils/cmsis/samd21/source
  ../lib/ASF/sam0/utils/header_files
  ../lib/ASF/sam0/utils/preprocessor
  ../lib/ASF/thirdparty/CMSIS/Include
)

add_arm_executable(
  firmware
  boards/board_init.c
  boards/board_startup.c
  init/main.c
)

#arm_target_link_libraries(firmware asf freertos)
arm_target_link_libraries(firmware asf)
