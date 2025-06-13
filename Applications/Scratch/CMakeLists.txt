cmake_minimum_required(VERSION 3.28)
project(Scratch LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(D "${CMAKE_BINARY_DIR}/Dependencies" CACHE STRING
  "Path to dependencies folder.")
file(TO_NATIVE_PATH "${D}" D)
set(DEFAULT_BUILD_TYPE "Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE "${DEFAULT_BUILD_TYPE}" CACHE
    STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()
if(WIN32)
  set(configure_script
    cmd /c "CALL ${CMAKE_SOURCE_DIR}/configure.bat -DD=${D}")
elseif(UNIX)
  set(configure_script "${CMAKE_SOURCE_DIR}/configure.sh" "-DD=${D}"
    "${CMAKE_BUILD_TYPE}")
endif()
execute_process(COMMAND ${configure_script}
  WORKING_DIRECTORY "${CMAKE_BINARY_DIR}" RESULT_VARIABLE configure_result
  OUTPUT_VARIABLE configure_output ERROR_VARIABLE configure_error
  OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_STRIP_TRAILING_WHITESPACE)
if(NOT configure_result EQUAL 0)
  message(FATAL_ERROR "Configuration script failed with error:\n${configure_error}\nOutput:\n${configure_output}")
endif()
include(../../Config/dependencies.cmake)
include_directories(${ASHKAL_INCLUDE_PATH})
include_directories(SYSTEM ${ASSIMP_INCLUDE_PATH})
include_directories(SYSTEM ${FREETYPE_INCLUDE_PATH})
include_directories(SYSTEM ${GLEW_INCLUDE_PATH})
include_directories(SYSTEM ${OPENCL_INCLUDE_PATH})
include_directories(SYSTEM ${SDL_INCLUDE_PATH})
include_directories(SYSTEM ${SDL_IMAGE_INCLUDE_PATH})
include_directories(SYSTEM ${SDL_TTF_INCLUDE_PATH})
include_directories(SYSTEM ${ZLIB_INCLUDE_PATH})
if(MSVC)
  add_compile_options(/bigobj /external:anglebrackets /external:W0
    $<$<CONFIG:Release>:/GL> /MP /WX)
  add_compile_definitions(_CRT_SECURE_NO_DEPRECATE NOMINMAX
    _SCL_SECURE_NO_WARNINGS WIN32_LEAN_AND_MEAN _WIN32_WINNT=0x0501)
  add_link_options($<$<CONFIG:Release>:/LTCG>)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  add_compile_options(-g $<$<CONFIG:Release>:-DNDEBUG>)
  if(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
    add_compile_options(-fsized-deallocation)
  endif()
endif()
include_directories(${PROJECT_BINARY_DIR})
file(GLOB_RECURSE header_files ${PROJECT_BINARY_DIR}/*.hpp)
file(GLOB_RECURSE source_files Source/*.cpp)
if(WIN32)
  set(app_type WIN32)
elseif(APPLE)
  set(app_type MACOSX_BUNDLE)
endif()
add_executable(Scratch ${app_type} ${header_files} ${source_files})
set_source_files_properties(${header_files} PROPERTIES HEADER_FILE_ONLY TRUE)
target_link_libraries(Scratch
  debug ${ASSIMP_LIBRARY_DEBUG_PATH}
  optimized ${ASSIMP_LIBRARY_OPTIMIZED_PATH}
  debug ${FREETYPE_LIBRARY_DEBUG_PATH}
  optimized ${FREETYPE_LIBRARY_OPTIMIZED_PATH}
  debug ${GLEW_LIBRARY_DEBUG_PATH}
  optimized ${GLEW_LIBRARY_OPTIMIZED_PATH}
  debug ${OPENCL_LIBRARY_DEBUG_PATH}
  optimized ${OPENCL_LIBRARY_OPTIMIZED_PATH}
  debug ${SDL_LIBRARY_DEBUG_PATH}
  optimized ${SDL_LIBRARY_OPTIMIZED_PATH}
  debug ${SDL_IMAGE_LIBRARY_DEBUG_PATH}
  optimized ${SDL_IMAGE_LIBRARY_OPTIMIZED_PATH}
  debug ${SDL_TTF_LIBRARY_DEBUG_PATH}
  optimized ${SDL_TTF_LIBRARY_OPTIMIZED_PATH}
  debug ${ZLIB_LIBRARY_DEBUG_PATH}
  optimized ${ZLIB_LIBRARY_OPTIMIZED_PATH})
if(WIN32)
  target_link_libraries(Scratch Imm32.lib opengl32.lib Setupapi.lib Version.lib
    winmm.lib)
endif()
install(TARGETS Scratch DESTINATION ${PROJECT_BINARY_DIR}/Application)
