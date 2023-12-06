cmake_minimum_required(VERSION 3.20)

project(pixie)

set(
  PIXIE_INCLUDES
  ${PROJECT_SOURCE_DIR}/core.h
  ${PROJECT_SOURCE_DIR}/font.h
  ${PROJECT_SOURCE_DIR}/imgui.h
  ${PROJECT_SOURCE_DIR}/pixie.h)

include_directories(
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_BINARY_DIR})

set(FONT_BMP_PATH ${PROJECT_SOURCE_DIR}/font.bmp)
configure_file(${PROJECT_SOURCE_DIR}/config.h.in pixie_config.h)

set(
  COMMON_SRC_FILES
  ${PROJECT_SOURCE_DIR}/imgui.cpp
  ${PROJECT_SOURCE_DIR}/font.cpp
  ${PROJECT_SOURCE_DIR}/pixie.cpp)

if (WIN32)
  set(
    PLATFORM_SRC_FILES
    ${PROJECT_SOURCE_DIR}/pixie_win.cpp)
elseif(APPLE)
  set(
    PLATFORM_SRC_FILES
    ${PROJECT_SOURCE_DIR}/pixie_osx.mm)
endif()

option(BUILD_PIXIE_DEMO "Build demo for pixie window." ON)

add_library(${PROJECT_NAME} ${COMMON_SRC_FILES} ${PLATFORM_SRC_FILES})

if (${BUILD_PIXIE_DEMO})
  add_executable(pixie_test ${PROJECT_SOURCE_DIR}/main.cpp)
  target_link_libraries(pixie_test PRIVATE ${PROJECT_NAME})
endif()
