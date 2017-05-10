include (${TestFunctions})

set (TestArgs "write ${TestConfigOptions} ${TestArgs} ${TestFile}")
set (ReadArgs "read ${TestConfigOptions} write_read.bin")

exec_test ("${TestArgs}" ${TestExitCode})

if (NOT "${TestFile}" STREQUAL "" AND NOT "${TestBlessedFile}" STREQUAL "")
  exec_test ("${ReadArgs}" 0)
  compare_files (write_read.bin ${TestBlessedFile})
endif ()
