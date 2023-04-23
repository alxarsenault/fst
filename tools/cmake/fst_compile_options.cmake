#
macro(fst_remove_exceptions)
    if(MSVC)
        set(COMPILER_OPTIONS_TO_REMOVE_LIST
            "/EHsc"
            "/GR"
            "/Gm-"
            "-fms-extensions"
            "-fms-compatibility"
            "-frtti"
            "-fexceptions"
            "-gline-tables-only"
            "-fno-inline")

        set(COMPILER_FLAG_LIST
            CMAKE_C_FLAGS
            CMAKE_C_FLAGS_DEBUG
            CMAKE_C_FLAGS_RELEASE
            CMAKE_C_FLAGS_MINSIZEREL
            CMAKE_C_FLAGS_RELWITHDEBINFO
            CMAKE_CXX_FLAGS
            CMAKE_CXX_FLAGS_DEBUG
            CMAKE_CXX_FLAGS_RELEASE
            CMAKE_CXX_FLAGS_MINSIZEREL
            CMAKE_CXX_FLAGS_RELWITHDEBINFO)

        foreach(flags ${COMPILER_FLAG_LIST})
            foreach(item ${COMPILER_OPTIONS_TO_REMOVE_LIST})
                string(REPLACE "${item}" "" ${flags} "${${flags}}")
            endforeach()
        endforeach()

        string(REPLACE "/EHsc" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
    endif()
endmacro()

#
function(fst_set_compile_options TARGET EXPOSURE)

    if(WIN32)
        target_compile_options(${TARGET} ${EXPOSURE} -D_CRT_SECURE_NO_WARNINGS=1 -D_ITERATOR_DEBUG_LEVEL=0)
    endif()

    # Clang
    if(CMAKE_CXX_COMPILER_ID MATCHES Clang)
        message(STATUS "${TARGET} : Detected Clang compiler.")

        set(FST_CLANG_COMMON_OPTIONS
            -Wall
            -Wextra
            -Wpedantic
            -Wzero-as-null-pointer-constant
            -Wno-reserved-id-macro
            -Wno-c++98-compat-pedantic
            -Wno-gnu-zero-variadic-macro-arguments
            -Wno-c++98-compat
            -Wno-old-style-cast
            -Wno-global-constructors
            -Wno-switch-enum
            -Wno-format-nonliteral
            -Wno-missing-prototypes
            -Wno-covered-switch-default
            -Wno-float-equal
            -Wno-unused-member-function)

        set(FST_CLANG_DEBUG_OPTIONS ${FST_CLANG_COMMON_OPTIONS})
        set(FST_CLANG_RELEASE_OPTIONS ${FST_CLANG_COMMON_OPTIONS})

        target_compile_options(${TARGET} ${EXPOSURE} "$<$<CONFIG:DEBUG>:${FST_CLANG_DEBUG_OPTIONS}>")
        target_compile_options(${TARGET} ${EXPOSURE} "$<$<CONFIG:RELEASE>:${FST_CLANG_RELEASE_OPTIONS}>")

        # GNU
    elseif(CMAKE_CXX_COMPILER_ID MATCHES GNU)
        message(STATUS "${TARGET} : Detected GNU compiler.")

        set(FST_GCC_COMMON_OPTIONS -Wall -Wextra -Wpedantic -Wno-parentheses -Wno-unused-but-set-variable)

        set(FST_GCC_DEBUG_OPTIONS ${FST_GCC_COMMON_OPTIONS})
        set(FST_GCC_RELEASE_OPTIONS ${FST_GCC_COMMON_OPTIONS})

        target_compile_options(${TARGET} ${EXPOSURE} "$<$<CONFIG:DEBUG>:${FST_GCC_DEBUG_OPTIONS}>")
        target_compile_options(${TARGET} ${EXPOSURE} "$<$<CONFIG:RELEASE>:${FST_GCC_RELEASE_OPTIONS}>")

        # MSVC
    elseif(CMAKE_CXX_COMPILER_ID MATCHES MSVC)
        message(STATUS "${TARGET} : Detected MSVC compiler.")

        set(FST_MSVC_COMMON_OPTIONS
            /Zc:__cplusplus # compiler option enables the __cplusplus preprocessor macro to report an updated value for
                            # recent C++ language standards support
            /Zc:alignedNew # Enable support for C++17 over-aligned new
            /permissive- # Standards conformance
            /W4 #
            # /WX # Treat linker warnings as errors
            /utf-8 # Set source and execution character sets to UTF-8
            /MP # Build with multiple processes
            /Zc:hiddenFriend- # Enforce Standard C++ hidden friend rules
            /FAs
            /GR- # Disable Run-Time Type Information
            /EHs-c- # Disable exceptions
            # /we26800
        )

        set(FST_MSVC_DEBUG_OPTIONS ${FST_MSVC_COMMON_OPTIONS})

        set(FST_MSVC_RELEASE_OPTIONS
            ${FST_MSVC_COMMON_OPTIONS}
            /MT # Causes the application to use the multithread, static version of the run-time library
            /Zp16 # Packs structures on 16-byte boundaries
            /GS- # Remove Buffer Security Check
            /fp:fast # Specify floating-point behavior
            /fp:except- # Disable floating-point exceptions
            /Ox # Enable Most Speed Optimizations
            /Ot # Favor Fast Code
            # /Zl # Omit Default Library Name
            /Zc:threadSafeInit-
            /Oy # Frame-Pointer Omission
        )

        set(FST_MSVC_PRIVATE_COMMON_OPTIONS
            /Wall #
            /wd4514 # Unreferenced inline function has been
            /wd4577 # noexcept used with no exception handling mode specified
            /wd4355 # this used in base member initializer list
            /wd5045 # Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified
            /wd4820 # bytes padding added after struct
            /wd4868 # compiler may not enforce left-to-right evaluation order in braced initializer list
            /wd4866 # compiler may not enforce left-to-right evaluation order for call to operator_name
        )

        set(FST_MSVC_PRIVATE_DEBUG_OPTIONS ${FST_MSVC_PRIVATE_COMMON_OPTIONS})
        set(FST_MSVC_PRIVATE_RELEASE_OPTIONS ${FST_MSVC_PRIVATE_COMMON_OPTIONS})

        target_compile_options(${TARGET} ${EXPOSURE} "$<$<CONFIG:DEBUG>:${FST_MSVC_DEBUG_OPTIONS}>")
        target_compile_options(${TARGET} ${EXPOSURE} "$<$<CONFIG:RELEASE>:${FST_MSVC_RELEASE_OPTIONS}>")

        target_compile_options(${TARGET} PRIVATE "$<$<CONFIG:DEBUG>:${FST_MSVC_PRIVATE_DEBUG_OPTIONS}>")
        target_compile_options(${TARGET} PRIVATE "$<$<CONFIG:RELEASE>:${FST_MSVC_PRIVATE_RELEASE_OPTIONS}>")

    else()
        message(FATAL_ERROR "${TARGET} : Compiler unsupported, aborting.\n")
    endif()

    # target_compile_features(${TARGET} PRIVATE cxx_std_20)
    target_compile_definitions(${TARGET} PUBLIC -D_HAS_EXCEPTIONS=0)

endfunction()

#
macro(fst_generate_config_file)
    set(FST_CONFIG_FILE_INPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/tools/config.h.in")
    set(FST_CONFIG_FILE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/fst/config")
    set(FST_CONFIG_FILE_OUTPUT_PATH "${FST_CONFIG_FILE_OUTPUT_DIRECTORY}/config.h")
    configure_file(${FST_CONFIG_FILE_INPUT_PATH} ${FST_CONFIG_FILE_OUTPUT_PATH})
endmacro()

#
macro(fst_include_config TARGET_NAME)
    if(FST_DEFAULT_CONFIG_DEFAULT)
        target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
        target_sources(${TARGET_NAME} PUBLIC ${FST_CONFIG_FILE_OUTPUT_PATH})
        source_group(TREE ${FST_CONFIG_FILE_OUTPUT_DIRECTORY} PREFIX config FILES ${FST_CONFIG_FILE_OUTPUT_PATH})
    endif()
endmacro()

macro(fst_link_dependencies TARGET_NAME)

    if(WIN32)
        target_link_libraries(${TARGET_NAME} PUBLIC winhttp)
    endif()
endmacro()
