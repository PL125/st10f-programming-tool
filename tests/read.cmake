include (${TestFunctions})

set (TestArgs "read ${TestConfigOptions} ${TestArgs}")
if (NOT "${TestFile}" STREQUAL "")
  set (TestArgs "${TestArgs} read.bin")
endif ()

exec_test ("${TestArgs}" ${TestExitCode})

if (NOT "${TestFile}" STREQUAL "" AND  ${RETVAL} EQUAL 0)
  compare_files (read.bin ${TestFile})
endif ()
