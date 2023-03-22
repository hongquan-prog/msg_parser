cmake_minimum_required(VERSION  3.20)

# select c compiler
set(CMAKE_C_COMPILER gcc)

# target
project(module LANGUAGES C)

# c sources
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/component/crc C_SOURCES)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/component/msg_parser C_SOURCES)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/component/rt-ctrl C_SOURCES)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src C_SOURCES)

# c includes
set(C_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/component/crc
    ${CMAKE_CURRENT_SOURCE_DIR}/component/msg_parser
    ${CMAKE_CURRENT_SOURCE_DIR}/component/rt-ctrl
)

# ld
set(LD_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/link.ld)

# executable
add_executable(${CMAKE_PROJECT_NAME} ${C_SOURCES})

# compile option
target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE 
$<$<CONFIG:DEBUG>:-g>
$<$<CONFIG:RELEASE>:-O3>
)

# link option
target_link_options(${CMAKE_PROJECT_NAME} PRIVATE 
$<$<CONFIG:DEBUG>:-fsanitize=address>
LINKER:-Map,${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}.map)

# include path
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${C_INCLUDES})