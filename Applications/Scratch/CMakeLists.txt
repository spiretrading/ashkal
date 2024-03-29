cmake_minimum_required(VERSION 3.8)
project(Scratch)
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
  execute_process(COMMAND cmd /c
    "CALL ${CMAKE_SOURCE_DIR}\\configure.bat -DD=${D}"
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")
elseif(UNIX)
  execute_process(COMMAND "${CMAKE_SOURCE_DIR}/configure.sh" "-DD=${D}"
    "${CMAKE_BUILD_TYPE}" WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")
endif()
include(../../Config/dependencies.cmake)
include_directories(${ASHKAL_INCLUDE_PATH})
include_directories(${BOOST_INCLUDE_PATH})
include_directories(${FREETYPE_INCLUDE_PATH})
include_directories(${GLEW_INCLUDE_PATH})
include_directories(${OPENCL_INCLUDE_PATH})
include_directories(${SDL_INCLUDE_PATH})
include_directories(${SDL_TTF_INCLUDE_PATH})
link_directories(${BOOST_DEBUG_PATH})
link_directories(${BOOST_OPTIMIZED_PATH})
add_definitions(-DCL_TARGET_OPENCL_VERSION=300)
add_definitions(-DBOOST_COMPUTE_USE_CPP11)
add_definitions(-DGLEW_STATIC)
if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX /bigobj /std:c++20")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /GL")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")
  set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /LTCG")
  add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
  add_definitions(-DNOMINMAX)
  add_definitions(-D_SCL_SECURE_NO_WARNINGS)
  add_definitions(-D_WIN32_WINNT=0x0501)
  add_definitions(-DWIN32_LEAN_AND_MEAN)
  add_definitions(/external:anglebrackets)
  add_definitions(/external:W0)
endif()
if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR
    ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=gnu++20")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_RELEASE} -O2 -DNDEBUG")
endif()
if(CYGWIN)
  add_definitions(-D__USE_W32_SOCKETS)
endif()
if(${CMAKE_SYSTEM_NAME} STREQUAL "SunOS")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_RELEASE} -pthreads")
endif()
include_directories(${PROJECT_BINARY_DIR})
file(GLOB header_files ${PROJECT_BINARY_DIR}/*.hpp)
file(GLOB source_files Source/*.cpp)
if(WIN32)
  set(app_type WIN32)
elseif(APPLE)
  set(app_type MACOSX_BUNDLE)
endif()
add_executable(Scratch ${app_type} ${header_files} ${source_files})
set_source_files_properties(${header_files} PROPERTIES HEADER_FILE_ONLY TRUE)
target_link_libraries(Scratch
  debug ${FREETYPE_LIBRARY_DEBUG_PATH}
  optimized ${FREETYPE_LIBRARY_OPTIMIZED_PATH}
  debug ${GLEW_LIBRARY_DEBUG_PATH}
  optimized ${GLEW_LIBRARY_OPTIMIZED_PATH}
  debug ${OPENCL_LIBRARY_DEBUG_PATH}
  optimized ${OPENCL_LIBRARY_OPTIMIZED_PATH}
  debug ${SDL_LIBRARY_DEBUG_PATH}
  optimized ${SDL_LIBRARY_OPTIMIZED_PATH}
  debug ${SDL_TTF_LIBRARY_DEBUG_PATH}
  optimized ${SDL_TTF_LIBRARY_OPTIMIZED_PATH})
if(WIN32)
  target_link_libraries(Scratch Imm32.lib opengl32.lib Setupapi.lib Version.lib
    winmm.lib)
endif()
install(TARGETS Scratch DESTINATION ${PROJECT_BINARY_DIR}/Application)
