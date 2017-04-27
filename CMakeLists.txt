cmake_minimum_required (VERSION 3.2)
project(grimp)

SET(BUILD_SHARED_LIBS ON)

option(debug "Build with debugging symbols." ON)

set (CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_STANDARD_REQUIRED)
#set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-missing-field-initializers -Werror")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC ")
#set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lboost_program_options")
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/bin)

if (debug)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
else ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -DNDEBUG")
endif ()

# add a target to generate API documentation with Doxygen
if(doxygen)
	find_package(Doxygen)
	if (NOT DOXYGEN_FOUND)
	    message(FATAL_ERROR 
	      "Doxygen is needed to build the documentation. Please install it correctly")
	endif()
	configure_file(${PROJECT_SOURCE_DIR}/kernel/Doxyfile.in 
		${PROJECT_BINARY_DIR}/Doxyfile @ONLY )
	add_custom_target(doc COMMAND ${DOXYGEN_EXECUTABLE}
			${PROJECT_BINARY_DIR}/Doxyfile
 			SOURCES ${PROJECT_BINARY_DIR}/Doxyfile)
endif()

add_subdirectory(modules)
