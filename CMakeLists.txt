cmake_minimum_required(VERSION 3.8)

project(freetype_test CXX)

set(USE_FREETYPE_SOURCE ON)
set(USE_ASAN ON)

if(CMAKE_HOST_UNIX)
    set(LINUX ON)
else()
    set(LINUX OFF)
endif()

set(TEST_NAME app)

set(DEFAULT_FONTPATH "${CMAKE_CURRENT_LIST_DIR}/resources/Abberancy.ttf")
set(DEFAULT_OUTPUT "${CMAKE_BINARY_DIR}/output.tga")
configure_file(resources/config.h.in ${CMAKE_BINARY_DIR}/config.h )

if(NOT EXISTS ${DEFAULT_FONTPATH})
    message(FATAL_ERROR "Can not find font ${DEFAULT_FONTPATH}")
endif()


if(USE_FREETYPE_SOURCE)
    add_subdirectory(../../Github/cocos2d-x-external/src/freetype deps)
    SET_TARGET_PROPERTIES(freetype PROPERTIES LINKER_LANGUAGE C)
else()
    set(cocos2dx_external ${CMAKE_CURRENT_LIST_DIR}/../../Github/cocos2d-x/external)    
    add_library(freetype STATIC IMPORTED GLOBAL)
    if(LINUX)
        set_target_properties(freetype PROPERTIES
            IMPORTED_LOCATION  ${cocos2dx_external}/freetype2/prebuilt/linux/64-bit/libfreetype.a
            INTERFACE_INCLUDE_DIRECTORIES ${cocos2dx_external}/freetype2/include/linux/freetype2
        )
    else()
        set_target_properties(freetype PROPERTIES
            IMPORTED_LOCATION  ${cocos2dx_external}/freetype2/prebuilt/win32/freetype.lib
            INTERFACE_INCLUDE_DIRECTORIES ${cocos2dx_external}/freetype2/include/win32/freetype2
            INTERFACE_COMPILE_DEFINITIONS _CRT_SECURE_NO_WARNINGS
            INTERFACE_COMPILE_DEFINITIONS _SCL_SECURE_NO_WARNINGS
        )
    endif()
endif()

if(WIN32)
    add_executable(${PROJECT_NAME} main.cpp)
    target_link_libraries(${PROJECT_NAME} freetype)
    target_include_directories(${PROJECT_NAME} PUBLIC 
        ${CMAKE_BINARY_DIR}
    )
endif()

file(GLOB SRC_LIST src/* utils/*)
add_executable(${TEST_NAME} tests.cpp
    ${SRC_LIST}
)
target_link_libraries(${TEST_NAME} freetype)
target_include_directories(${TEST_NAME} PUBLIC 
    src 
    utils
    ${CMAKE_BINARY_DIR}
)
source_group(main FILES tests.cpp ${CMAKE_BINARY_DIR}/config.h)
source_group(utils REGULAR_EXPRESSION utils/*)
source_group(src REGULAR_EXPRESSION src/*)

if(WIN32)
    target_compile_definitions(${TEST_NAME} PUBLIC 
        _CRT_SECURE_NO_WARNINGS
        _SCL_SECURE_NO_WARNINGS
        WIN32
        _WIN32
        _WINDOWS
    #    _NO_CRT_STDIO_INLINE 
    )
    target_link_libraries(${TEST_NAME}
        ws2_32
        userenv
        psapi
        winmm
        Version
        Iphlpapi
    )
    set_target_properties(${TEST_NAME} PROPERTIES
        LINK_OPTIONS /SUBSYSTEM:CONSOLE
    )
    target_compile_options(${TEST_NAME} PUBLIC
        /MP)
endif()

if(LINUX)
    find_package(ZLIB REQUIRED)
    target_link_libraries(${TEST_NAME}
       ${ZLIB_LIBRARIES}
    )
    if(USE_ASAN)
        target_link_libraries(${TEST_NAME}
            asan
        )
        target_compile_options(${TEST_NAME} PUBLIC
            -fsanitize=address
            -fno-omit-frame-pointer
            -static-libsan
            #-shared-libsan
        )
        # set_target_properties(${TEST_NAME} PROPERTIES
        #     LINK_OPTIONS "-fsanitize=address -shared-libsan"
        # )
        set_property(TARGET ${TEST_NAME} APPEND_STRING
            PROPERTY LINK_OPTIONS "-fsanitize=address -static-libsan"
        )

        add_custom_command(TARGET ${TEST_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E echo "LD_PRELOAD=/usr/lib/x86_64-linux-gnu/libasan.so.4 ./app"
            COMMAND ${CMAKE_COMMAND} -E echo "#!/bin/bash\nLD_PRELOAD=/usr/lib/x86_64-linux-gnu/libasan.so.4 ./app" > start.sh   
        )
    endif()

   # add_custom_command()
endif()