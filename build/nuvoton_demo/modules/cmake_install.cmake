# Install script for directory: C:/Andy/nuvoton/afr/amazon-freertos-share_up_to_date/cmake/amazon-freertos/modules

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/amazon-freertos")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "TRUE")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("C:/Andy/nuvoton/afr/amazon-freertos-share_up_to_date/cmake/amazon-freertos/build/nuvoton_demo/modules/libraries/standard/common/cmake_install.cmake")
  include("C:/Andy/nuvoton/afr/amazon-freertos-share_up_to_date/cmake/amazon-freertos/build/nuvoton_demo/modules/libraries/standard/crypto/cmake_install.cmake")
  include("C:/Andy/nuvoton/afr/amazon-freertos-share_up_to_date/cmake/amazon-freertos/build/nuvoton_demo/modules/libraries/standard/freertos_plus_posix/cmake_install.cmake")
  include("C:/Andy/nuvoton/afr/amazon-freertos-share_up_to_date/cmake/amazon-freertos/build/nuvoton_demo/modules/libraries/standard/freertos_plus_tcp/cmake_install.cmake")
  include("C:/Andy/nuvoton/afr/amazon-freertos-share_up_to_date/cmake/amazon-freertos/build/nuvoton_demo/modules/libraries/standard/metrics/cmake_install.cmake")
  include("C:/Andy/nuvoton/afr/amazon-freertos-share_up_to_date/cmake/amazon-freertos/build/nuvoton_demo/modules/libraries/standard/mqtt/cmake_install.cmake")
  include("C:/Andy/nuvoton/afr/amazon-freertos-share_up_to_date/cmake/amazon-freertos/build/nuvoton_demo/modules/libraries/standard/provisioning/cmake_install.cmake")
  include("C:/Andy/nuvoton/afr/amazon-freertos-share_up_to_date/cmake/amazon-freertos/build/nuvoton_demo/modules/libraries/standard/serializer/cmake_install.cmake")
  include("C:/Andy/nuvoton/afr/amazon-freertos-share_up_to_date/cmake/amazon-freertos/build/nuvoton_demo/modules/libraries/standard/tls/cmake_install.cmake")
  include("C:/Andy/nuvoton/afr/amazon-freertos-share_up_to_date/cmake/amazon-freertos/build/nuvoton_demo/modules/libraries/standard/utils/cmake_install.cmake")
  include("C:/Andy/nuvoton/afr/amazon-freertos-share_up_to_date/cmake/amazon-freertos/build/nuvoton_demo/modules/libraries/aws/defender/cmake_install.cmake")
  include("C:/Andy/nuvoton/afr/amazon-freertos-share_up_to_date/cmake/amazon-freertos/build/nuvoton_demo/modules/libraries/aws/greengrass/cmake_install.cmake")
  include("C:/Andy/nuvoton/afr/amazon-freertos-share_up_to_date/cmake/amazon-freertos/build/nuvoton_demo/modules/libraries/aws/ota/cmake_install.cmake")
  include("C:/Andy/nuvoton/afr/amazon-freertos-share_up_to_date/cmake/amazon-freertos/build/nuvoton_demo/modules/libraries/aws/shadow/cmake_install.cmake")
  include("C:/Andy/nuvoton/afr/amazon-freertos-share_up_to_date/cmake/amazon-freertos/build/nuvoton_demo/modules/ports/ble/cmake_install.cmake")
  include("C:/Andy/nuvoton/afr/amazon-freertos-share_up_to_date/cmake/amazon-freertos/build/nuvoton_demo/modules/ports/pkcs11/cmake_install.cmake")
  include("C:/Andy/nuvoton/afr/amazon-freertos-share_up_to_date/cmake/amazon-freertos/build/nuvoton_demo/modules/ports/posix/cmake_install.cmake")
  include("C:/Andy/nuvoton/afr/amazon-freertos-share_up_to_date/cmake/amazon-freertos/build/nuvoton_demo/modules/ports/secure_sockets/cmake_install.cmake")
  include("C:/Andy/nuvoton/afr/amazon-freertos-share_up_to_date/cmake/amazon-freertos/build/nuvoton_demo/modules/ports/wifi/cmake_install.cmake")
  include("C:/Andy/nuvoton/afr/amazon-freertos-share_up_to_date/cmake/amazon-freertos/build/nuvoton_demo/modules/libraries/3rdparty/cmake_install.cmake")

endif()

