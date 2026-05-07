cmake_minimum_required(VERSION 3.16)

project(MLFBDPlugin LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall)

find_package(Qt6 REQUIRED COMPONENTS Core LinguistTools)

find_package(MLPlugin REQUIRED)

find_package(Threads REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(IMAGEMAGICK REQUIRED IMPORTED_TARGET Magick++)

add_library(MLFBDPlugin SHARED)

add_subdirectory(include)
add_subdirectory(src)

qt_add_translations(MLFBDPlugin
  TS_FILES translations/MLFBDPlugin_ru_RU.ts
)

qt_add_resources(MLFBDPlugin "mlfbd_styles"
    PREFIX "/"
    FILES "styles/mlfbd_styles.css"
)

target_include_directories(MLFBDPlugin
  PRIVATE
    include
)

target_link_libraries(MLFBDPlugin
  PRIVATE
    MLPlugin::MLPlugin
    MLBookProc::MLBookProc
    XMLParserCPP::XMLParserCPP
    Threads::Threads
    PkgConfig::IMAGEMAGICK
)

include(GNUInstallDirs)

install(TARGETS MLFBDPlugin EXPORT
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/${PROJECT_NAME}
)