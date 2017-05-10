if (WIN32)
  add_library (SerialPort STATIC
    SerialPortFactory.cpp
    SerialPort.cpp
    SerialPortWin32.cpp
    )
else()
  add_library (SerialPort STATIC
    SerialPortFactory.cpp
    SerialPort.cpp
    SerialPortUnix.cpp
    )
endif()

set_target_properties (SerialPort PROPERTIES
  CXX_STANDARD 11
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF
)
