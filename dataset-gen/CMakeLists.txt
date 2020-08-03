cmake_minimum_required(VERSION 3.12)
project(DATASET_GEN CXX)

option(DATASET_GEN_ENABLE_FAST_MATH_OPT "Enable fast math (-ffast-math)" ON)
option(DATASET_GEN_ENABLE_ARCH_NATIVE_OPT "Enable optimizations for native arch (-march=native)" OFF)
option(DATASET_GEN_ENABLE_LTO "Enable LTO" OFF)
option(DATASET_GEN_ENABLE_ASAN "Enable Address Sanitizer" OFF)
option(DATASET_GEN_ENABLE_MEMSAN "Enable Memory Sanitizer" OFF)
option(DATASET_GEN_ENABLE_UBSAN "Enable Undefined Behavior Sanitizer" OFF)
option(DATASET_GEN_ENABLE_ANALYZER "Enable static analyzer" OFF)

find_package(Threads REQUIRED)

set(DATASET_GEN_SOURCES
  src/BattleEngine.cpp
  src/DatasetGen.cpp
  src/Units.cpp)

add_executable(dataset-gen ${DATASET_GEN_SOURCES})
target_link_libraries(dataset-gen ${CMAKE_THREAD_LIBS_INIT})

set_property(TARGET dataset-gen PROPERTY CXX_STANDARD 20)

if(CMAKE_CXX_COMPILER_ID STREQUAL Clang OR CMAKE_COMPILER_IS_GNUCXX)
  target_link_libraries(dataset-gen m)
  target_compile_options(dataset-gen PRIVATE -fno-exceptions -fno-rtti -Wall -Wextra -Wpedantic -Wconversion -Wno-c99-designator)

  if(DATASET_GEN_ENABLE_FAST_MATH_OPT)
    target_compile_options(dataset-gen PRIVATE -ffast-math)
  endif()

  if(DATASET_GEN_ENABLE_ARCH_NATIVE_OPT)
    target_compile_options(dataset-gen PRIVATE -march=native)
  endif()

  if(DATASET_GEN_ENABLE_LTO)
    target_compile_options(dataset-gen PRIVATE -flto)
    set_property(TARGET dataset-gen APPEND_STRING PROPERTY LINK_FLAGS " -flto")
  endif()

  if(DATASET_GEN_ENABLE_ASAN)
    target_compile_options(dataset-gen PRIVATE -fsanitize=address)
    set_property(TARGET dataset-gen APPEND_STRING PROPERTY LINK_FLAGS " -fsanitize=address")
  endif()

  if(DATASET_GEN_ENABLE_MEMSAN)
    target_compile_options(dataset-gen PRIVATE -fsanitize=memory)
    set_property(TARGET dataset-gen APPEND_STRING PROPERTY LINK_FLAGS " -fsanitize=memory")
  endif()

  if(DATASET_GEN_ENABLE_UBSAN)
    target_compile_options(dataset-gen PRIVATE -fsanitize=undefined)
    set_property(TARGET dataset-gen APPEND_STRING PROPERTY LINK_FLAGS " -fsanitize=undefined")
  endif()

  if(DATASET_GEN_ENABLE_ANALYZER)
    if(CMAKE_CXX_COMPILER_ID STREQUAL Clang)
      target_compile_options(dataset-gen PRIVATE --analyze)
    else()
      target_compile_options(dataset-gen PRIVATE -fanalyzer)
    endif()
  endif()
endif()
