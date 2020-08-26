cmake_minimum_required(VERSION 3.10)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# set the project name
project(test_mathserver)
include_directories(../include)

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
add_executable(test_mathserver 
    CuTest.c
    ../include/CuTest.h
    ../include/Server.h
    ../include/debugFuncWrappers.h
    AllTests.c
    testWithSleeps.c
    )
target_link_libraries(test_mathserver libzmq)
target_link_libraries(test_mathserver czmq))