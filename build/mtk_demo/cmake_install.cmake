# Install script for directory: C:/Andy/nuvoton/afr/amazon-freertos-share_up_to_date/cmake/amazon-freertos

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
  include("C:/Andy/nuvoton/afr/amazon-freertos-share_up_to_date/cmake/amazon-freertos/build/mtk_demo/vendors/mediatek/boards/mt7697hx-dev-kit/cmake_install.cmake")
  include("C:/Andy/nuvoton/afr/amazon-freertos-share_up_to_date/cmake/amazon-freertos/build/mtk_demo/modules/cmake_install.cmake")
  include("C:/Andy/nuvoton/afr/amazon-freertos-share_up_to_date/cmake/amazon-freertos/build/mtk_demo/demos/cmake_install.cmake")
  include("C:/Andy/nuvoton/afr/amazon-freertos-share_up_to_date/cmake/amazon-freertos/build/mtk_demo/test/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "C:/Andy/nuvoton/afr/amazon-freertos-share_up_to_date/cmake/amazon-freertos/build/mtk_demo/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
