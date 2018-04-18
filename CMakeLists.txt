cmake_minimum_required( VERSION 2.8)

project( erishttp)

message( STATUS "The project directory: ${PROJECT_SOURCE_DIR}")

add_subdirectory( src)
message( STATUS "System: ${CMAKE_SYSTEM}")
message( STATUS "System name: ${CMAKE_SYSTEM_NAME}")
message( STATUS "System version: ${CMAKE_SYSTEM_VERSION}")
message( STATUS "System processor: ${CMAKE_SYSTEM_PROCESSOR}")
message( STATUS "C_compiler: ${CMAKE_C_COMPILER}")
message( STATUS "C_compiler flags: ${CMAKE_C_FLAGS}")

if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	message( STATUS "--------------------Linux-OS-----------------------")
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
	message( STATUS "--------------------Mac-OS-------------------------")
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD")
	message( STATUS "-------------------FreeBSD-------------------------")
endif ()

