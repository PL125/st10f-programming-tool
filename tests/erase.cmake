include (${TestFunctions})

if (NOT "${TestFile}" STREQUAL "")
  set (WriteArgs "write ${TestConfigOptions} ${TestFile}")
  exec_test ("${WriteArgs}" 0)
endif ()

set (TestArgs "erase ${TestConfigOptions} ${TestArgs}")
exec_test ("${TestArgs}" ${TestExitCode})

if (NOT "${TestFile}" STREQUAL "" AND NOT "${TestBlessedFile}" STREQUAL "")
  set (ReadArgs "read ${TestConfigOptions} erase_read.bin")
  exec_test ("${ReadArgs}" 0)
  compare_files (erase_read.bin ${TestBlessedFile})
endif ()
