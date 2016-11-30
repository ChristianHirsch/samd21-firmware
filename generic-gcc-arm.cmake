##########################################################################
# "THE ANY BEVERAGE-WARE LICENSE" (Revision 42 - based on beer-ware
# license):
# <dev@layer128.net> wrote this file. As long as you retain this notice
# you can do whatever you want with this stuff. If we meet some day, and
# you think this stuff is worth it, you can buy me a be(ve)er(age) in
# return. (I don't like beer much.)
#
# Matthias Kleemann
##########################################################################

##########################################################################
# The toolchain requires some variables set.
#
# ARM_MCU (default: atmega8)
#     the type of ARM the application is built for
# ARM_L_FUSE (NO DEFAULT)
#     the LOW fuse value for the MCU used
# ARM_H_FUSE (NO DEFAULT)
#     the HIGH fuse value for the MCU used
# ARM_UPLOADTOOL (default: armdude)
#     the application used to upload to the MCU
#     NOTE: The toolchain is currently quite specific about
#           the commands used, so it needs tweaking.
# ARM_UPLOADTOOL_PORT (default: usb)
#     the port used for the upload tool, e.g. usb
# ARM_PROGRAMMER (default: armispmkII)
#     the programmer hardware used, e.g. armispmkII
##########################################################################

INCLUDE(CMakeForceCompiler)

##########################################################################
# options
##########################################################################
option(WITH_MCU "Add the mCU type to the target file name." ON)

##########################################################################
# executables in use
##########################################################################
find_program(ARM_CC arm-none-eabi-gcc)
find_program(ARM_CXX arm-none-eabi-g++)
find_program(ARM_OBJCOPY arm-none-eabi-objcopy)
find_program(ARM_SIZE_TOOL arm-none-eabi-size)
find_program(ARM_OBJDUMP arm-none-eabi-objdump)

##########################################################################
# toolchain starts with defining mandatory variables
##########################################################################
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm-none-eabi)
#set(CMAKE_C_COMPILER ${ARM_CC})
#set(CMAKE_CXX_COMPILER ${ARM_CXX})
# specify the cross compiler
CMAKE_FORCE_C_COMPILER(${ARM_CC} GNU)
CMAKE_FORCE_CXX_COMPILER(${ARM_CXX} GNU)

##########################################################################
# some necessary tools and variables for ARM builds, which may not
# defined yet
# - ARM_UPLOADTOOL
# - ARM_UPLOADTOOL_PORT
# - ARM_PROGRAMMER
# - ARM_MCU
# - ARM_SIZE_ARGS
##########################################################################

# default upload tool
if(NOT ARM_UPLOADTOOL)
   set(
      ARM_UPLOADTOOL openocd
      CACHE STRING "Set default upload tool: openocd"
   )
   find_program(ARM_UPLOADTOOL openocd)
endif(NOT ARM_UPLOADTOOL)

# default upload tool port
if(NOT ARM_UPLOADTOOL_PORT)
   set(
      ARM_UPLOADTOOL_PORT usb
      CACHE STRING "Set default upload tool port: usb"
   )
endif(NOT ARM_UPLOADTOOL_PORT)

# default programmer/interface (hardware)
if(NOT ARM_PROGRAMMER)
   set(
      ARM_PROGRAMMER cmsis-dap
      CACHE STRING "Set default programmer hardware model: cmsis-dap"
   )
endif(NOT ARM_PROGRAMMER)

# default MCU (chip)
if(NOT ARM_MCU)
   set(
      ARM_MCU cortex-m0plus
      CACHE STRING "Set default MCU: cortex-m0plus (see 'arm-none-eabi-gcc --target-help' for valid values)"
   )
endif(NOT ARM_MCU)

#set(ARM_LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/lib/ASF/sam0/utils/linker_scripts/samd21/gcc/samd21g18a_flash.ld")
set(ARM_LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/bootloader_samd21x18.ld")
set(GCC_ARM_LINKER_SCRIPT "")
if(ARM_LINKER_SCRIPT)
   set(
       GCC_ARM_LINKER_SCRIPT -T${ARM_LINKER_SCRIPT}
       CACHE STRING "Setting linker script to ${ARM_LINKER_SCRIPT}"
       )
endif(ARM_LINKER_SCRIPT)

#default arm-size args
if(NOT ARM_SIZE_ARGS)
   if(APPLE)
      set(ARM_SIZE_ARGS -B)
   else(APPLE)
       set(ARM_SIZE_ARGS -B)
       #      set(ARM_SIZE_ARGS -B;--mcu=${ARM_MCU})
   endif(APPLE)
endif(NOT ARM_SIZE_ARGS)

##########################################################################
# check build types:
# - Debug
# - Release
# - RelWithDebInfo
#
# Release is chosen, because of some optimized functions in the
# ARM toolchain, e.g. _delay_ms().
##########################################################################
if(NOT ((CMAKE_BUILD_TYPE MATCHES Release) OR
        (CMAKE_BUILD_TYPE MATCHES RelWithDebInfo) OR
        (CMAKE_BUILD_TYPE MATCHES Debug) OR
        (CMAKE_BUILD_TYPE MATCHES MinSizeRel)))
   set(
      CMAKE_BUILD_TYPE Release
      CACHE STRING "Choose cmake build type: Debug Release RelWithDebInfo MinSizeRel"
      FORCE
   )
endif(NOT ((CMAKE_BUILD_TYPE MATCHES Release) OR
           (CMAKE_BUILD_TYPE MATCHES RelWithDebInfo) OR
           (CMAKE_BUILD_TYPE MATCHES Debug) OR
           (CMAKE_BUILD_TYPE MATCHES MinSizeRel)))

##########################################################################

##########################################################################
# target file name add-on
##########################################################################
if(WITH_MCU)
   set(MCU_TYPE_FOR_FILENAME "-${ARM_MCU}")
else(WITH_MCU)
   set(MCU_TYPE_FOR_FILENAME "")
endif(WITH_MCU)

##########################################################################
# add_arm_executable
# - IN_VAR: EXECUTABLE_NAME
#
# Creates targets and dependencies for ARM toolchain, building an
# executable. Calls add_executable with ELF file as target name, so
# any link dependencies need to be using that target, e.g. for
# target_link_libraries(<EXECUTABLE_NAME>-${ARM_MCU}.elf ...).
##########################################################################
function(add_arm_executable EXECUTABLE_NAME)
   if(NOT ARGN)
      message(FATAL_ERROR "No source files given for ${EXECUTABLE_NAME}.")
   endif(NOT ARGN)

   # set file names
   set(elf_file ${EXECUTABLE_NAME}${MCU_TYPE_FOR_FILENAME}.elf)
   set(bin_file ${EXECUTABLE_NAME}${MCU_TYPE_FOR_FILENAME}.bin)
   set(hex_file ${EXECUTABLE_NAME}${MCU_TYPE_FOR_FILENAME}.hex)
   set(map_file ${EXECUTABLE_NAME}${MCU_TYPE_FOR_FILENAME}.map)
   set(eeprom_image ${EXECUTABLE_NAME}${MCU_TYPE_FOR_FILENAME}-eeprom.hex)

   # elf file
   add_executable(${elf_file} EXCLUDE_FROM_ALL ${ARGN})

   set_target_properties(
      ${elf_file}
      PROPERTIES
      COMPILE_FLAGS "-mcpu=${ARM_MCU} -mthumb -Wall -c -std=gnu99 -ffunction-sections -fdata-sections -nostdlib -nostartfiles --param max-inline-insns-single=500 -Os -DDEBUG=0 -D__SAMD21G18A__ -DBOARD=USER_BOARD -DUSB_PID_HIGH=0x00 -DUSB_PID_LOW=0x4D -DUSB_VID_LOW=0x41 -DUSB_VID_HIGH=0x23"
      LINK_FLAGS "-mcpu=${ARM_MCU} -mthumb ${GCC_ARM_LINKER_SCRIPT} -Wall -Wl,--cref -Wl,--check-sections -Wl,--gc-sections -save-temps -Wl,--unresolved-symbols=report-all -Wl,--warn-common -Wl,--warn-section-align -Wl,--warn-unresolved-symbols --specs=nano.specs --specs=nosys.specs -Wl,-Map,\"${map_file}\" -Wl,--start-group -lm"
         LINK_DEPENDS "${ARM_LINKER_SCRIPT}"
   )

   # bin file
   add_custom_command(
      OUTPUT ${bin_file}
      COMMAND
         ${ARM_OBJCOPY} -O binary ${elf_file} ${bin_file}
      COMMAND
         ${ARM_SIZE_TOOL} ${ARM_SIZE_ARGS} ${elf_file}
      DEPENDS ${elf_file}
   )

   # hex file
   add_custom_command(
      OUTPUT ${hex_file}
      COMMAND
         ${ARM_OBJCOPY} -O ihex ${elf_file} ${hex_file}
      COMMAND
         ${ARM_SIZE_TOOL} ${ARM_SIZE_ARGS} ${elf_file}
      DEPENDS ${elf_file}
   )

   # eeprom
   add_custom_command(
      OUTPUT ${eeprom_image}
      COMMAND
         ${ARM_OBJCOPY} -j .eeprom --set-section-flags=.eeprom=alloc,load
            --change-section-lma .eeprom=0 --no-change-warnings
            -O ihex ${elf_file} ${eeprom_image}
      DEPENDS ${elf_file}
   )

   add_custom_target(
      ${EXECUTABLE_NAME}
      ALL
      DEPENDS ${hex_file} ${eeprom_image}
   )

   set_target_properties(
      ${EXECUTABLE_NAME}
      PROPERTIES
         OUTPUT_NAME "${elf_file}"
   )

   # clean
   get_directory_property(clean_files ADDITIONAL_MAKE_CLEAN_FILES)
   set_directory_properties(
      PROPERTIES
         ADDITIONAL_MAKE_CLEAN_FILES "${map_file}"
   )

   # upload - with openocd
   add_custom_target(
      upload_${EXECUTABLE_NAME}
      ${ARM_UPLOADTOOL}
         -c "interface ${ARM_PROGRAMMER}"
         -c "transport select swd"
         -c "set CHIPNAME at91samd21g18"
         -c "source [find target/at91samdXX.cfg]"
         -c "reset_config  srst_nogate"
         -c "adapter_nsrst_delay 100"
         -c "adapter_nsrst_assert_width 100"
         -c "init"
         -c "targets"
         -c "reset halt"
         -c "at91samd bootloader 0"
         -c "program ${bin_file}"
         -c "at91samd bootloader 8192"
         -c "reset"
         -c "shutdown"
      DEPENDS
        ${bin_file}
        ${hex_file}
      COMMENT "Uploading ${bin_file} to ${ARM_MCU} using ${ARM_PROGRAMMER}"
   )

   # upload eeprom only - with armdude
   # see also bug http://savannah.nongnu.org/bugs/?40142
   #add_custom_target(
   #   upload_eeprom
   #   ${ARM_UPLOADTOOL} -p ${ARM_MCU} -c ${ARM_PROGRAMMER} ${ARM_UPLOADTOOL_OPTIONS}
   #      -U eeprom:w:${eeprom_image}
   #      -P ${ARM_UPLOADTOOL_PORT}
   #   DEPENDS ${eeprom_image}
   #   COMMENT "Uploading ${eeprom_image} to ${ARM_MCU} using ${ARM_PROGRAMMER}"
   #)

   # get status
   #add_custom_target(
   #   get_status
   #   ${ARM_UPLOADTOOL} -p ${ARM_MCU} -c ${ARM_PROGRAMMER} -P ${ARM_UPLOADTOOL_PORT} -n -v
   #   COMMENT "Get status from ${ARM_MCU}"
   #)

   # get fuses
   #add_custom_target(
   #   get_fuses
   #   ${ARM_UPLOADTOOL} -p ${ARM_MCU} -c ${ARM_PROGRAMMER} -P ${ARM_UPLOADTOOL_PORT} -n
   #      -U lfuse:r:-:b
   #      -U hfuse:r:-:b
   #   COMMENT "Get fuses from ${ARM_MCU}"
   #)

   # set fuses
   #add_custom_target(
   #   set_fuses
   #   ${ARM_UPLOADTOOL} -p ${ARM_MCU} -c ${ARM_PROGRAMMER} -P ${ARM_UPLOADTOOL_PORT}
   #      -U lfuse:w:${ARM_L_FUSE}:m
   #      -U hfuse:w:${ARM_H_FUSE}:m
   #      COMMENT "Setup: High Fuse: ${ARM_H_FUSE} Low Fuse: ${ARM_L_FUSE}"
   #)

   # get oscillator calibration
   #add_custom_target(
   #   get_calibration
   #      ${ARM_UPLOADTOOL} -p ${ARM_MCU} -c ${ARM_PROGRAMMER} -P ${ARM_UPLOADTOOL_PORT}
   #      -U calibration:r:${ARM_MCU}_calib.tmp:r
   #      COMMENT "Write calibration status of internal oscillator to ${ARM_MCU}_calib.tmp."
   #)

   # set oscillator calibration
   #add_custom_target(
   #   set_calibration
   #   ${ARM_UPLOADTOOL} -p ${ARM_MCU} -c ${ARM_PROGRAMMER} -P ${ARM_UPLOADTOOL_PORT}
   #      -U calibration:w:${ARM_MCU}_calib.hex
   #      COMMENT "Program calibration status of internal oscillator from ${ARM_MCU}_calib.hex."
   #)

   # disassemble
   add_custom_target(
      disassemble_${EXECUTABLE_NAME}
      ${ARM_OBJDUMP} -h -S ${elf_file} > ${EXECUTABLE_NAME}.lst
      DEPENDS ${elf_file}
   )

endfunction(add_arm_executable)

##########################################################################
# add_arm_library
# - IN_VAR: LIBRARY_NAME
#
# Calls add_library with an optionally concatenated name
# <LIBRARY_NAME>${MCU_TYPE_FOR_FILENAME}.
# This needs to be used for linking against the library, e.g. calling
# target_link_libraries(...).
##########################################################################
function(add_arm_library LIBRARY_NAME)
   if(NOT ARGN)
      message(FATAL_ERROR "No source files given for ${LIBRARY_NAME}.")
   endif(NOT ARGN)

   set(lib_file ${LIBRARY_NAME}${MCU_TYPE_FOR_FILENAME})

   add_library(${lib_file} STATIC ${ARGN})

   set_target_properties(
      ${lib_file}
      PROPERTIES
         COMPILE_FLAGS "-mcpu=${ARM_MCU} -mthumb -Wall -c -std=gnu99 -ffunction-sections -fdata-sections -nostdlib -nostartfiles --param max-inline-insns-single=500 -Os -DDEBUG=0 -D__SAMD21G18A__ -DBOARD=USER_BOARD -DUSB_PID_HIGH=0x00 -DUSB_PID_LOW=0x4D -DUSB_VID_LOW=0x41 -DUSB_VID_HIGH=0x23"
         OUTPUT_NAME "${lib_file}"
   )

   if(NOT TARGET ${LIBRARY_NAME})
      add_custom_target(
         ${LIBRARY_NAME}
         ALL
         DEPENDS ${lib_file}
      )

      set_target_properties(
         ${LIBRARY_NAME}
         PROPERTIES
            OUTPUT_NAME "${lib_file}"
      )
   endif(NOT TARGET ${LIBRARY_NAME})

endfunction(add_arm_library)

##########################################################################
# arm_target_link_libraries
# - IN_VAR: EXECUTABLE_TARGET
# - ARGN  : targets and files to link to
#
# Calls target_link_libraries with ARM target names (concatenation,
# extensions and so on.
##########################################################################
function(arm_target_link_libraries EXECUTABLE_TARGET)
   if(NOT ARGN)
      message(FATAL_ERROR "Nothing to link to ${EXECUTABLE_TARGET}.")
   endif(NOT ARGN)

   get_target_property(TARGET_LIST ${EXECUTABLE_TARGET} OUTPUT_NAME)

   foreach(TGT ${ARGN})
      if(TARGET ${TGT})
         get_target_property(ARG_NAME ${TGT} OUTPUT_NAME)
         list(APPEND TARGET_LIST ${ARG_NAME})
      else(TARGET ${TGT})
         list(APPEND NON_TARGET_LIST ${TGT})
      endif(TARGET ${TGT})
   endforeach(TGT ${ARGN})

   target_link_libraries(${TARGET_LIST} ${NON_TARGET_LIST})
endfunction(arm_target_link_libraries EXECUTABLE_TARGET)
