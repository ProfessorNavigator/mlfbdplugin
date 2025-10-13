cmake_minimum_required(VERSION 3.16)

project(MLFBDPlugin VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(MLPluginIfc REQUIRED)
find_package(Intl REQUIRED)
find_package(Gettext)

pkg_check_modules(IMAGEMAGICK REQUIRED IMPORTED_TARGET Magick++)

find_package(Threads QUIET)

add_library(mlfbdplugin SHARED)

set_target_properties(mlfbdplugin PROPERTIES POSITION_INDEPENDENT_CODE True)

set_target_properties(mlfbdplugin PROPERTIES
  VERSION ${PROJECT_VERSION}
  SOVERSION ${PROJECT_VERSION_MAJOR}
)

add_subdirectory(include)
add_subdirectory(src)

target_include_directories(mlfbdplugin
    PRIVATE include
)

target_link_libraries(mlfbdplugin
    PRIVATE MLPluginIfc::mlpluginifc
    PRIVATE MLBookProc::mlbookproc
    PRIVATE PkgConfig::IMAGEMAGICK
)

if(Threads_FOUND)
  target_link_libraries(mlfbdplugin
      PRIVATE Threads::Threads
  )
endif()

include(GNUInstallDirs)

install(TARGETS mlfbdplugin EXPORT "${PROJECT_NAME}Targets"
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

if(Gettext_FOUND)
   GETTEXT_PROCESS_PO_FILES("ru" ALL
    INSTALL_DESTINATION "${CMAKE_INSTALL_LOCALEDIR}"
    PO_FILES "po/${PROJECT_NAME}.po"
  )
endif()
