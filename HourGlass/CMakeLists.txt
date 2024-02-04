cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

### Project ##################################################################

project(HourGlass)

### Modules ##################################################################

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(FormatOutputName)

### Dependencies #############################################################

find_package(Qt5Widgets 5.12 REQUIRED)
find_package(Qt5Xml 5.12 REQUIRED)

### Project ##################################################################

list(APPEND HourGlass_FORMS
  forms/WMainWindow.ui
  forms/WProjects.ui
  forms/WWorkHours.ui
)

list(APPEND HourGlass_HEADERS
  include/Global.h
  include/Hours.h
  include/Item.h
  include/Month.h
  include/MonthModel.h
  include/Project.h
  include/ProjectModel.h
  include/WMainWindow.h
  include/WProjects.h
  include/WWorkHours.h
  include/XML_io.h
  include/XML_tags.h
)

list(APPEND HourGlass_SOURCES
  src/Global.cpp
  src/Item.cpp
  src/main.cpp
  src/Month.cpp
  src/MonthModel.cpp
  src/Project.cpp
  src/ProjectModel.cpp
  src/WMainWindow.cpp
  src/WProjects.cpp
  src/WWorkHours.cpp
  src/XML_io.cpp
)

### Target ###################################################################

add_executable(HourGlass
  ${HourGlass_FORMS}
  ${HourGlass_HEADERS}
  ${HourGlass_SOURCES}
)

if(WIN32)
  set_target_properties(HourGlass PROPERTIES
    WIN32_EXECUTABLE ON
  )
endif()

format_output_name(HourGlass "HourGlass")

set_target_properties(HourGlass PROPERTIES
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED ON
)

target_include_directories(HourGlass
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Qt Dependency

set_target_properties(HourGlass PROPERTIES
  AUTOMOC ON
  AUTORCC ON
  AUTOUIC ON
  AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/forms
)

target_compile_definitions(HourGlass
  PRIVATE QT_NO_CAST_FROM_ASCII
  PRIVATE QT_NO_CAST_TO_ASCII
)

target_link_libraries(HourGlass
  PRIVATE Qt5::Widgets Qt5::Xml
)
