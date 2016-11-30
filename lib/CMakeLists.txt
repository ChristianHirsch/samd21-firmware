include_directories(
  ../src/config
  FreeRTOS/include
  FreeRTOS/portable/GCC/ARM_CM0

  ASF
  ASF/common/boards
  ASF/common/boards/user_board
  ASF/common/boards/user_board/board_config
  ASF/common/services/serial
  ASF/common/services/serial/sam0_usart
  ASF/common/utils
  ASF/common/utils/interrupt
  ASF/sam0/boards/dummy
  ASF/sam0/drivers/port
  ASF/sam0/drivers/port/quick_start
  ASF/sam0/drivers/sercom
  ASF/sam0/drivers/sercom/usart
  ASF/sam0/drivers/sercom/usart/quick_start
  ASF/sam0/drivers/sercom/usart/quick_start_callback
  ASF/sam0/drivers/system
  ASF/sam0/drivers/system/clock
  ASF/sam0/drivers/system/clock/clock_samd21_r21_da/
  ASF/sam0/drivers/system/clock/clock_samd21_r21_da/module_config/
  ASF/sam0/drivers/system/clock/quick_start_clock
  ASF/sam0/drivers/system/clock/quick_start_gclk
  ASF/sam0/drivers/system/interrupt
  ASF/sam0/drivers/system/interrupt/quick_start
  ASF/sam0/drivers/system/interrupt/system_interrupt_samd21
  ASF/sam0/drivers/system/pinmux
  ASF/sam0/drivers/system/pinmux/quick_start
  ASF/sam0/drivers/system/power/power_sam_d_r
  ASF/sam0/drivers/system/reset/reset_sam_d_r
  ASF/sam0/utils
  ASF/sam0/utils/cmsis/samd21/include
  ASF/sam0/utils/cmsis/samd21/include/component
  ASF/sam0/utils/cmsis/samd21/include/instance
  ASF/sam0/utils/cmsis/samd21/include/pio
  ASF/sam0/utils/cmsis/samd21/source
  ASF/sam0/utils/header_files
  ASF/sam0/utils/preprocessor
  ASF/thirdparty/CMSIS/Include
)

set(ARM_LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/sam0/utils/linker_scripts/samd21/gcc/samd21g18au_flash.ld)

#add_arm_library( freertos
#  FreeRTOS/croutine.c
#  FreeRTOS/event_groups.c
#  FreeRTOS/list.c
#  FreeRTOS/portable/GCC/ARM_CM0/port.c
#  FreeRTOS/portable/MemMang/heap_1.c
#  FreeRTOS/portable/MemMang/heap_2.c
#  FreeRTOS/portable/MemMang/heap_3.c
#  FreeRTOS/portable/MemMang/heap_4.c
#  FreeRTOS/portable/MemMang/heap_5.c
#  FreeRTOS/queue.c
#  FreeRTOS/tasks.c
#  FreeRTOS/timers.c  
#)

add_arm_library( asf
    ASF/sam0/drivers/port/port.c
    #    ASF/sam0/drivers/system/clock/clock_samd21_r21_da/clock.c
    #    ASF/sam0/drivers/system/clock/clock_samd21_r21_da/gclk.c
    ASF/sam0/drivers/system/interrupt/system_interrupt.c
    ASF/sam0/drivers/system/pinmux/pinmux.c
    ASF/sam0/drivers/system/system.c
    #   ASF/sam0/utils/cmsis/samd21/source/gcc/startup_samd21.c
    )
