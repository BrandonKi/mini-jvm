cmake_minimum_required(VERSION 3.0.0)
project(mjvm VERSION 0.1.0)

set (CMAKE_CXX_STANDARD 23)

cmake_policy(SET CMP0069 NEW)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)

add_library(mjvm_lib OBJECT
    ${CMAKE_CURRENT_SOURCE_DIR}/src/class_file.cpp	
    ${CMAKE_CURRENT_SOURCE_DIR}/src/class_loader.cpp	
    
    ${CMAKE_CURRENT_SOURCE_DIR}/src/bl_interp.cpp	
)

target_include_directories(mjvm_lib
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib
)

set(cli_driver_src
	$<TARGET_OBJECTS:mjvm_lib>
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
)

add_executable(mjvm ${cli_driver_src})

target_include_directories(mjvm
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib
)

# set(test_driver_src
# 	$<TARGET_OBJECTS:mjvm_lib>
#     ${CMAKE_CURRENT_SOURCE_DIR}/tests/run_tests.cpp
# )

# add_executable(mjvm_tests ${test_driver_src})

# target_include_directories(mjvm_tests
#     PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
#     PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib
# )

if(${LTO})
    include(CheckIPOSupported)
    check_ipo_supported(RESULT result)
    if(result)
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
    endif()
endif()

if(MSVC)
    target_compile_options(mjvm PRIVATE /W3)
    # target_compile_options(mjvm_tests PRIVATE /W3)
else()
    target_compile_options(mjvm PRIVATE -Wall -Wextra -pedantic)
    # target_compile_options(mjvm_tests PRIVATE -Wall -Wextra -pedantic)
endif()
