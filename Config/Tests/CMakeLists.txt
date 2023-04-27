include_directories(SYSTEM ${DOCTEST_INCLUDE_PATH})
file(GLOB source_files ${ASHKAL_SOURCE_PATH}/Tests/*.cpp)
if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif()
add_executable(ashkal_tester ${source_files})
target_link_libraries(ashkal_tester
  debug ${OPENCL_LIBRARY_DEBUG_PATH}
  optimized ${OPENCL_LIBRARY_OPTIMIZED_PATH})
if(UNIX)
  target_link_libraries(ashkal_tester pthread)
endif()
add_custom_command(TARGET ashkal_tester POST_BUILD COMMAND ashkal_tester)
install(TARGETS ashkal_tester CONFIGURATIONS Debug
  DESTINATION ${TEST_INSTALL_DIRECTORY}/Debug)
install(TARGETS ashkal_tester CONFIGURATIONS Release RelWithDebInfo
  DESTINATION ${TEST_INSTALL_DIRECTORY}/Release)
