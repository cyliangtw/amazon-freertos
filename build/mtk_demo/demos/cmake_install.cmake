# Install script for directory: C:/Andy/nuvoton/afr/amazon-freertos-share_up_to_date/cmake/amazon-freertos/demos

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
  include("C:/Andy/nuvoton/afr/amazon-freertos-share_up_to_date/cmake/amazon-freertos/build/mtk_demo/demos/ble/cmake_install.cmake")
  include("C:/Andy/nuvoton/afr/amazon-freertos-share_up_to_date/cmake/amazon-freertos/build/mtk_demo/demos/defender/cmake_install.cmake")
  include("C:/Andy/nuvoton/afr/amazon-freertos-share_up_to_date/cmake/amazon-freertos/build/mtk_demo/demos/greengrass_connectivity/cmake_install.cmake")
  include("C:/Andy/nuvoton/afr/amazon-freertos-share_up_to_date/cmake/amazon-freertos/build/mtk_demo/demos/mqtt/cmake_install.cmake")
  include("C:/Andy/nuvoton/afr/amazon-freertos-share_up_to_date/cmake/amazon-freertos/build/mtk_demo/demos/ota/cmake_install.cmake")
  include("C:/Andy/nuvoton/afr/amazon-freertos-share_up_to_date/cmake/amazon-freertos/build/mtk_demo/demos/posix/cmake_install.cmake")
  include("C:/Andy/nuvoton/afr/amazon-freertos-share_up_to_date/cmake/amazon-freertos/build/mtk_demo/demos/shadow/cmake_install.cmake")
  include("C:/Andy/nuvoton/afr/amazon-freertos-share_up_to_date/cmake/amazon-freertos/build/mtk_demo/demos/tcp/cmake_install.cmake")
  include("C:/Andy/nuvoton/afr/amazon-freertos-share_up_to_date/cmake/amazon-freertos/build/mtk_demo/demos/wifi_provisioning/cmake_install.cmake")

endif()

