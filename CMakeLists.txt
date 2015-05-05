CMAKE_MINIMUM_REQUIRED(VERSION 3.0 FATAL_ERROR)
PROJECT("IntroductionToCppConcurrencyLiveLessons")

ADD_COMPILE_OPTIONS(-std=c++11)
SET(CMAKE_EXPORT_COMPILE_COMMANDS 1)
MESSAGE("${CMAKE_CXX_COMPILER_ID}")

# Find Boost.
IF(APPLE OR WIN32)
    set(Boost_USE_STATIC_LIBS TRUE)
ENDIF(APPLE OR WIN32)
FIND_PACKAGE(Boost 1.57 REQUIRED COMPONENTS system filesystem)
INCLUDE_DIRECTORIES(SYSTEM ${Boost_INCLUDE_DIRS})

IF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR
        "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    SET(WARNINGS "-Weverything -Wall -Wextra -Wno-c++98-compat \
            -Wno-c++98-compat-pedantic -Wno-padded -Wno-global-constructors \
            -Wno-exit-time-destructors -Werror")
ENDIF()
IF(NOT CONFIGURED_ONCE)
    SET(CMAKE_CXX_FLAGS "${WARNINGS}"
        CACHE STRING "Flags used by the compiler during all build types."
        FORCE)
ENDIF(NOT CONFIGURED_ONCE)

ADD_EXECUTABLE(Thread Thread.cpp)
ADD_EXECUTABLE(Barrier Barrier.cpp)
ADD_EXECUTABLE(FunctionObj FunctionObj.cpp)
ADD_EXECUTABLE(PassArgs PassArgs.cpp)
ADD_EXECUTABLE(Future Future.cpp)
ADD_EXECUTABLE(Async Async.cpp)
ADD_EXECUTABLE(BadPass BadPass.cpp)
ADD_EXECUTABLE(BadConst BadConst.cpp)
ADD_EXECUTABLE(FileList FileList.cpp)
TARGET_LINK_LIBRARIES(FileList ${Boost_LIBRARIES})

SET(CONFIGURED_ONCE TRUE CACHE INTERNAL
        "A flag showing that CMake has configured at least once.")
