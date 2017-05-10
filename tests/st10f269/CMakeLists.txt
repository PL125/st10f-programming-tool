include (${CMAKE_SOURCE_DIR}/tests/functions.cmake)

set (TestConfigurations st10f269)
set (SerialPortSpeed 230400)
set (TestNamespace 269_)

include (${CMAKE_SOURCE_DIR}/tests/CMakeLists.txt)

set (TestDataDir "${CMAKE_SOURCE_DIR}/tests/st10f269/erase")

add_normal_test (IgnoreFrequencyOpt "ident -f 22.1" 0)

add_erase_test (EraseWhole "" 0 ${TestDataDir}/ok_ones ${TestDataDir}/zeros)
add_erase_test (EraseBlock0 "-b 0" 0 ${TestDataDir}/ok_b0_erased ${TestDataDir}/zeros)
add_erase_test (EraseBlock1 "-b 1" 0 ${TestDataDir}/ok_b1_erased ${TestDataDir}/zeros)
add_erase_test (EraseBlock2 "-b 2" 0 ${TestDataDir}/ok_b2_erased ${TestDataDir}/zeros)
add_erase_test (EraseBlock3 "-b 3" 0 ${TestDataDir}/ok_b3_erased ${TestDataDir}/zeros)
add_erase_test (EraseBlock4 "-b 4" 0 ${TestDataDir}/ok_b4_erased ${TestDataDir}/zeros)
add_erase_test (EraseBlock5 "-b 5" 0 ${TestDataDir}/ok_b5_erased ${TestDataDir}/zeros)
add_erase_test (EraseBlock6 "-b 6" 0 ${TestDataDir}/ok_b6_erased ${TestDataDir}/zeros)
add_erase_test (EraseBlocks1246 "-b 2,4,1,6" 0 ${TestDataDir}/ok_b1246_erased ${TestDataDir}/zeros)

set (TestDataDir "${CMAKE_SOURCE_DIR}/tests/st10f269/write")

add_write_test (WriteWhole1 "" 0 ${TestDataDir}/random ${TestDataDir}/random)
add_write_test (WriteZeros "" 0 ${TestDataDir}/zeros ${TestDataDir}/zeros)
add_write_test (Write0B "-e" 6 ${TestDataDir}/ok_ones ${TestDataDir}/0B)
add_write_test (Write1B "" 0 ${TestDataDir}/ok_1B ${TestDataDir}/1B)
# Tests of automatic block erasing
add_write_test (WriteZeros2 "" 0 ${TestDataDir}/zeros ${TestDataDir}/zeros)
add_write_test (Write16K "" 0 ${TestDataDir}/ok_16K ${TestDataDir}/16K)
add_write_test (Write16K_1B "" 0 ${TestDataDir}/ok_16K_1B ${TestDataDir}/16K_1B)
add_write_test (Write24K "" 0 ${TestDataDir}/ok_24K ${TestDataDir}/24K)
add_write_test (Write24K_1B "" 0 ${TestDataDir}/ok_24K_1B ${TestDataDir}/24K_1B)
add_write_test (Write32K+Progress "-g" 0 ${TestDataDir}/ok_32K ${TestDataDir}/32K)
add_write_test (Write32K_1B "" 0 ${TestDataDir}/ok_32K_1B ${TestDataDir}/32K_1B)
add_write_test (Write64K "" 0 ${TestDataDir}/ok_64K ${TestDataDir}/64K)
add_write_test (Write64K_1B "" 0 ${TestDataDir}/ok_64K_1B ${TestDataDir}/64K_1B)
add_write_test (Write128K "" 0 ${TestDataDir}/ok_128K ${TestDataDir}/128K)
add_write_test (Write128K_1B+Progress "-g" 0 ${TestDataDir}/ok_128K_1B ${TestDataDir}/128K_1B)
add_write_test (Write192K "" 0 ${TestDataDir}/ok_192K ${TestDataDir}/192K)
add_write_test (Write192K_1B "" 0 ${TestDataDir}/ok_192K_1B ${TestDataDir}/192K_1B)
# Test of write over size and erasing blocks with mas 0x00 (no block)
add_write_test (Write256K_1B "" 6 ${TestDataDir}/ok_192K_1B ${TestDataDir}/256K_1B)

set (TestDataDir "${CMAKE_SOURCE_DIR}/tests/st10f269/read")

add_write_test (ReadWriteRandom "" 0 ${TestDataDir}/random ${TestDataDir}/random)
add_read_test (Read0B "-n 0" 6 "X")
add_read_test (Read256K_1B "-n 262145" 6 "X")
add_read_test (Read1B+Progress "-g -n 1" 0 ${TestDataDir}/ok_1B)
add_read_test (Read100003B "-n 100003" 0 ${TestDataDir}/ok_100003B)
add_read_test (Read262144B+Progress "-g -n 262144" 0 ${TestDataDir}/ok_262144B)
