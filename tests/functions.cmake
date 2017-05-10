macro (M_SET_CONFIG_OPTIONS)
  if (${SerialPortSpeed})
    set (CONFIG_OPTIONS "${CONFIG_OPTIONS} -s ${SerialPortSpeed}")
  endif ()
  if (${DefaultSerialPortName})
    set (CONFIG_OPTIONS "${CONFIG_OPTIONS} -p ${DefaultSerialPortName}")
  endif ()
  if (NOT "${McuCpuFrequency}" STREQUAL "")
    set (CONFIG_OPTIONS "${CONFIG_OPTIONS} -f ${McuCpuFrequency}")
  endif ()
endmacro ()

macro (M_ADD_TEST SCRIPT)
  add_test(NAME ${TestNamespace}${NAME}
    CONFIGURATIONS ${TestConfigurations}
    COMMAND ${CMAKE_COMMAND}
    -DTestFunctions=${CMAKE_SOURCE_DIR}/tests/functions.cmake
    -DTestArgs=${ARGS}
    -DTestConfigOptions=${CONFIG_OPTIONS}
    -DTestExitCode=${EXITCODE}
    -DTestBlessedFile=${BLESSEDFILE}
    -DTestFile=${FILE}
    -P ${SCRIPT}
    )
endmacro ()


function (ADD_WITHOUT_CONFIG_TEST NAME ARGS EXITCODE)
  m_add_test (${CMAKE_SOURCE_DIR}/tests/other_operations.cmake)
endfunction ()

function (ADD_NORMAL_TEST NAME ARGS EXITCODE)
  m_set_config_options ()
  m_add_test (${CMAKE_SOURCE_DIR}/tests/other_operations.cmake)
endfunction ()

function (ADD_ERASE_TEST NAME ARGS EXITCODE BLESSEDFILE FILE)
  m_set_config_options ()
  m_add_test (${CMAKE_SOURCE_DIR}/tests/erase.cmake)
endfunction ()

function (ADD_WRITE_TEST NAME ARGS EXITCODE BLESSEDFILE FILE)
  m_set_config_options ()
  m_add_test (${CMAKE_SOURCE_DIR}/tests/write.cmake)
endfunction ()

function (ADD_READ_TEST NAME ARGS EXITCODE FILE)
  m_set_config_options ()
  m_add_test (${CMAKE_SOURCE_DIR}/tests/read.cmake)
endfunction ()


# -------------------------------------------------------


function (EXEC_TEST ARGS EXITCODE)
  string (REPLACE " " ";" ARGS_LIST "${ARGS}")
  execute_process (
    COMMAND ${CMAKE_BINARY_DIR}/main ${ARGS_LIST}
    RESULT_VARIABLE MAIN_RESULT
    TIMEOUT 120
    )
  if (NOT ${MAIN_RESULT} EQUAL ${EXITCODE})
    message(FATAL_ERROR "Unexpected exit code ${MAIN_RESULT}, expected ${EXITCODE}")
  endif()
  set (RETVAL ${MAIN_RESULT} PARENT_SCOPE)
endfunction ()


function(COMPARE_FILES F1 F2)
  execute_process (
    COMMAND ${CMAKE_COMMAND} -E compare_files ${F1} ${F2}
    RESULT_VARIABLE CMP_RESULT
    )
  if(CMP_RESULT)
    message(FATAL_ERROR "Files do not match: ${F1} ${F2}")
  endif()
endfunction()
