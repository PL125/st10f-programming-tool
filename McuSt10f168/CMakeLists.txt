# Path to firmware binary files for ST10F168
set (FwSt10f168Prefix ${FwPrefix})
set(FwSt10f168 
  ${FwSt10f168Prefix}/st10f168/Objects/st10f168.bin
  )

# Generate firmware header files
include_directories (${CMAKE_CURRENT_BINARY_DIR})

generate_headers_from_binaries (
  "$<TARGET_FILE:xd>" 
  FwSt10f168 
  "${FwSt10f168}" 
  "${CMAKE_CURRENT_BINARY_DIR}"
  )

add_library (McuSt10f168 STATIC
  McuSt10f168.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/fw_st10f168.hpp
)

set_target_properties (McuSt10f168 PROPERTIES
  CXX_STANDARD 11
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF
)
