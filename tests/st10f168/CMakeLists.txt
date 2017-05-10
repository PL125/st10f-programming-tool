include (${CMAKE_SOURCE_DIR}/tests/functions.cmake)

set (TestConfigurations st10f168)
set (SerialPortSpeed 57600)
set (McuCpuFrequency 22.1)
set (TestNamespace 168_)

add_without_config_test (MissingFrequencyOptionForIdent "ident -s ${SerialPortSpeed}" 0)
add_without_config_test (MissingFrequencyOption "erase -s ${SerialPortSpeed}" 6)

include (${CMAKE_SOURCE_DIR}/tests/CMakeLists.txt)

set (TestDataDir "${CMAKE_SOURCE_DIR}/tests/st10f168/erase")

add_erase_test (EraseWhole "" 0 ${TestDataDir}/ok_ones ${TestDataDir}/zeros)
add_erase_test (EraseBlock0 "-b 0" 0 ${TestDataDir}/ok_b0_erased ${TestDataDir}/zeros)
add_erase_test (EraseBlock1 "-b 1" 0 ${TestDataDir}/ok_b1_erased ${TestDataDir}/zeros)
add_erase_test (EraseBlock2 "-b 2" 0 ${TestDataDir}/ok_b2_erased ${TestDataDir}/zeros)
add_erase_test (EraseBlock3 "-b 3" 0 ${TestDataDir}/ok_b3_erased ${TestDataDir}/zeros)
add_erase_test (EraseBlocks023 "-b 3,0,2" 0 ${TestDataDir}/ok_b023_erased ${TestDataDir}/zeros)

set (TestDataDir "${CMAKE_SOURCE_DIR}/tests/st10f168/write")

add_write_test (WriteWhole1 "" 0 ${TestDataDir}/random ${TestDataDir}/random)
add_write_test (WriteZeros "" 0 ${TestDataDir}/zeros ${TestDataDir}/zeros)
add_write_test (Write0B "-e" 6 ${TestDataDir}/ok_ones ${TestDataDir}/0B)
add_write_test (Write1B "" 0 ${TestDataDir}/ok_1B ${TestDataDir}/1B)
# Tests of automatic block erasing
add_write_test (WriteZeros2 "" 0 ${TestDataDir}/zeros ${TestDataDir}/zeros)
add_write_test (Write16K+Progress "-g" 0 ${TestDataDir}/ok_16K ${TestDataDir}/16K)
add_write_test (Write16K_1B "" 0 ${TestDataDir}/ok_16K_1B ${TestDataDir}/16K_1B)
add_write_test (Write64K "" 0 ${TestDataDir}/ok_64K ${TestDataDir}/64K)
add_write_test (Write64K_1B "" 0 ${TestDataDir}/ok_64K_1B ${TestDataDir}/64K_1B)
add_write_test (Write160K "" 0 ${TestDataDir}/ok_160K ${TestDataDir}/160K)
add_write_test (Write160K_1B+Progress "-g" 0 ${TestDataDir}/ok_160K_1B ${TestDataDir}/160K_1B)
# Test of write over size and erasing blocks with mas 0x00 (no block)
add_write_test (Write256K_1B "" 6 ${TestDataDir}/ok_160K_1B ${TestDataDir}/256K_1B)

set (TestDataDir "${CMAKE_SOURCE_DIR}/tests/st10f168/read")

add_write_test (ReadWriteRandom "" 0 ${TestDataDir}/random ${TestDataDir}/random)
add_read_test (Read0B "-n 0" 6 "X")
add_read_test (Read256K_1B "-n 262145" 6 "X")
add_read_test (Read1B+Progress "-g -n 1" 0 ${TestDataDir}/ok_1B)
add_read_test (Read100003B "-n 100003" 0 ${TestDataDir}/ok_100003B)
add_read_test (Read262144B+Progress "-g -n 262144" 0 ${TestDataDir}/ok_262144B)
