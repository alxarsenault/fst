

set(TEST_NAME config_test)

file(GLOB_RECURSE FST_TESTS_HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
file(GLOB_RECURSE FST_TESTS_SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_executable(${TEST_NAME} ${FST_TESTS_HEADER_FILES} ${FST_TESTS_SOURCE_FILES})
target_link_libraries(${TEST_NAME} PUBLIC fst)

target_include_directories(${TEST_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
fst_set_compile_options(${TEST_NAME} PUBLIC)
target_compile_definitions(${TEST_NAME} PUBLIC -DFST_TEST_RESOURCES_DIRECTORY="${CMAKE_CURRENT_SOURCE_DIR}/resources")

fst_include_config(${TEST_NAME})
set_target_properties(${TEST_NAME} PROPERTIES FOLDER tests/feature_tests)

add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
