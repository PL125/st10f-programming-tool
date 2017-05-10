cmake_minimum_required (VERSION 3.3)
project (Main VERSION 1.0.0)

add_compile_options (-Wall -Werror -pedantic -g)

# Path to common firmware binary files
set (FwPrefix ${CMAKE_CURRENT_SOURCE_DIR}/firmware)
set (FwCommonPrefix ${FwPrefix})
set(FwCommon
  ${FwCommonPrefix}/stage_1/Objects/stage_1.bin
  ${FwCommonPrefix}/ident/Objects/ident.bin
)

# Define program name and version
add_definitions (-DPROGRAM_NAME="${CMAKE_PROJECT_NAME}")
add_definitions (-DPROGRAM_VERSION="${PROJECT_VERSION}")

# Platform specific options
if (WIN32)
  add_definitions (-DWIN32)
  set (DefaultSerialPortName COM1)
else()
  add_definitions (-DUNIX)
  set (DefaultSerialPortName /dev/ttyUSB0)
endif()
add_definitions(-DDEFAULT_SERIAL_PORT_NAME="${DefaultSerialPortName}")

# Utility, to dump binary file as a C declaration
add_subdirectory (${CMAKE_CURRENT_SOURCE_DIR}/utils/xd)

# Generate firmware header files
include_directories (${CMAKE_CURRENT_BINARY_DIR})

# Function generates header files containing arrays with binary
# files data. Name transformation is: <name>.bin -> fw_<name>.hpp
#
# Arguments:
#    GenCmd  - path to the generator command
#    NameSpc - namespace for C++ definitions in the header file
#    FwList  - list of binary files
#    OutDir  - directory where the header files will be placed
function (generate_headers_from_binaries GenCmd NameSpc FwList OutDir)
  foreach (FILE ${FwList})
    get_filename_component(NAME "${FILE}" NAME_WE)
    set (OutHeader ${OutDir}/fw_${NAME}.hpp)
    # set (NameSpc FwCommon)
    add_custom_command (
      OUTPUT ${OutHeader}
      COMMAND ${GenCmd} -dfw_${NAME} -p${NameSpc} ${FILE} ${OutHeader}
      DEPENDS ${GenCmd} ${FILE}
      )
  endforeach()
endfunction()

generate_headers_from_binaries (
  "$<TARGET_FILE:xd>"
  FwCommon
  "${FwCommon}"
  "${CMAKE_CURRENT_BINARY_DIR}"
)

# Generate header with help message
add_custom_command (
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/help_message.hpp
  COMMAND $<TARGET_FILE:xd> -thelp_message -pHelpMessage ${CMAKE_SOURCE_DIR}/HelpMessage.txt ${CMAKE_CURRENT_BINARY_DIR}/help_message.hpp
  DEPENDS $<TARGET_FILE:xd> ${CMAKE_SOURCE_DIR}/HelpMessage.txt
  )

include_directories (
  ${CMAKE_CURRENT_SOURCE_DIR} 
  ${CMAKE_CURRENT_SOURCE_DIR}/McuSt10f269 
  ${CMAKE_CURRENT_SOURCE_DIR}/McuSt10f168 
  ${CMAKE_CURRENT_SOURCE_DIR}/SerialPort 
  )
add_subdirectory (McuSt10f269)
add_subdirectory (McuSt10f168)
add_subdirectory (SerialPort)
add_executable (main
  ExitException.cpp
  Logger.cpp
  UserConfig.cpp
  Mcu.cpp
  main.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/fw_stage_1.hpp
  ${CMAKE_CURRENT_BINARY_DIR}/fw_ident.hpp
  ${CMAKE_CURRENT_BINARY_DIR}/help_message.hpp
  )
target_link_libraries (main McuSt10f269 McuSt10f168 SerialPort)

set_target_properties (main PROPERTIES
              	      CXX_STANDARD 11
	              CXX_STANDARD_REQUIRED ON
		      CXX_EXTENSIONS OFF
                      )

# Tests
enable_testing ()

include (${CMAKE_SOURCE_DIR}/tests/st10f168/CMakeLists.txt) 
include (${CMAKE_SOURCE_DIR}/tests/st10f269/CMakeLists.txt) 
