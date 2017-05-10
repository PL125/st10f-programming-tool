# Path to firmware binary files for ST10F269
set (FwSt10f269Prefix ${FwPrefix})
set(FwSt10f269 
  ${FwSt10f269Prefix}/st10f269/Objects/st10f269.bin
  )

# Generate firmware header files
include_directories (${CMAKE_CURRENT_BINARY_DIR})

generate_headers_from_binaries (
  "$<TARGET_FILE:xd>"
  FwSt10f269
  "${FwSt10f269}"
  "${CMAKE_CURRENT_BINARY_DIR}"
)

add_library (McuSt10f269 STATIC
  McuSt10f269.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/fw_st10f269.hpp
)

set_target_properties (McuSt10f269 PROPERTIES
  CXX_STANDARD 11
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF
)
