# List of all headers.
set(HEADER_LIST
    xml
    aligned_storage
    allocator
    array
    atomic
    binary_file
    bitpack
    buffer
    byte_vector
    color
    common
    config
    file
    file_view
    function
    geometry
    http
    indexed_value
    int24_t
    iterator
    math
    memory
    memory_pool
    memory_range
    memory_utils
    meta
    mutex
    number
    object_wrapper
    pair
    path
    pointer
    profiler
    reference_wrapper
    simd
    stack_string
    small_vector
    stack_vector
    status_code
    stream
    string
    string_view
    sutils
    time
    trace
    traits
    tuple
    unicode
    unordered_map
    utility
    vector)

foreach(header_name ${HEADER_LIST})
    set(proj_name header_test_${header_name})
    set(header_str "fst/${header_name}.h")
    set(TEST_NAME ${proj_name})

    file(GLOB_RECURSE FST_TESTS_HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
    file(GLOB_RECURSE FST_TESTS_SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

    add_executable(${TEST_NAME} ${FST_TESTS_HEADER_FILES} ${FST_TESTS_SOURCE_FILES})
    target_link_libraries(${TEST_NAME} PUBLIC fst)

    target_include_directories(${TEST_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
    fst_set_compile_options(${TEST_NAME} PUBLIC)
    target_compile_definitions(${TEST_NAME} PUBLIC -DFST_HEADER="${header_str}")

    fst_include_config(${TEST_NAME})

    set_target_properties(${TEST_NAME} PROPERTIES
        EXCLUDE_FROM_ALL TRUE
        EXCLUDE_FROM_DEFAULT_BUILD TRUE
        FOLDER tests/feature_tests
    )

    add_test(
        NAME ${TEST_NAME} 
        COMMAND ${CMAKE_COMMAND} --build . --target ${TEST_NAME} --config $<CONFIG>
    )

    set_tests_properties(${TEST_NAME} PROPERTIES WILL_FAIL FALSE)
endforeach()
 

