include_directories(SYSTEM ${DOCTEST_INCLUDE_PATH})
file(GLOB source_files ${ASHKAL_SOURCE_PATH}/Tests/*.cpp)
add_executable(ashkal_tester ${source_files})
if(UNIX)
  target_link_libraries(ashkal_tester pthread)
endif()
add_custom_command(TARGET ashkal_tester POST_BUILD COMMAND ashkal_tester)
install(TARGETS ashkal_tester CONFIGURATIONS Debug
  DESTINATION ${TEST_INSTALL_DIRECTORY}/Debug)
install(TARGETS ashkal_tester CONFIGURATIONS Release RelWithDebInfo
  DESTINATION ${TEST_INSTALL_DIRECTORY}/Release)
