cmake_minimum_required(VERSION 3.10)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
enable_testing()
# set the project name
project(MathServer)
include_directories(include)
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
    add_subdirectory(test)
endif()


########################################################################
# LIBZMQ dependency
########################################################################
find_package(ZeroMQ REQUIRED)
IF (ZeroMQ_FOUND)
    include_directories(${ZeroMQ_INCLUDE_DIRS})
    list(APPEND MORE_LIBRARIES ${ZeroMQ_LIBRARY})
  IF (PC_LIBZMQ_FOUND)
      set(pkg_config_names_private "${pkg_config_names_private} libzmq")
      list(APPEND OPTIONAL_LIBRARIES_STATIC ${ZeroMQ_STATIC_LIBRARY})
  ELSE (PC_LIBZMQ_FOUND)
      set(pkg_config_libs_private "${pkg_config_libs_private} -lzmq")
  ENDIF (PC_LIBZMQ_FOUND)
ELSE (ZeroMQ_FOUND)
    message( FATAL_ERROR "libzmq not found." )
ENDIF (ZeroMQ_FOUND)

find_package(czmq REQUIRED)


# add the executable
add_executable(MathServer 
    src/MathServer.c
    include/Server.h
    )
target_link_libraries(MathServer libzmq)
target_link_libraries(MathServer czmq)
add_test(NAME serverBasicTest 
COMMAND ./test_mathserver
WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test/build)