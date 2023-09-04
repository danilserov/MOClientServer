# CMakeList.txt : CMake project for CommandEmitter, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("moclientserver")

include_directories(include)
file(GLOB SOURCES src/*.cpp)
file(GLOB HEADERS include/*.h)
file(GLOB PROGRAM program/*.h program/*.cpp)

# Add source to this project's executable.
add_executable (moclientserver 
${SOURCES}
${HEADERS}
${PROGRAM}
)

set_property(TARGET moclientserver PROPERTY CXX_STANDARD 14)

# Add tests and install targets if needed.


find_package(GTest)

IF (${GTest_FOUND})
   MESSAGE(STATUS "Found GTest.")
   include_directories(${GTest_INCLUDE_DIRS})
ELSE ()
   MESSAGE(STATUS "Could not locate GTest. Try download")

    include(FetchContent)
    FetchContent_Declare(
      googletest
      URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
ENDIF ()

enable_testing()

file(GLOB TESTS tests/*.cpp)
add_executable(motests 
${TESTS}
${SOURCES}
${HEADERS}
)

target_link_libraries(motests GTest::gtest_main)

add_test(NAME motests COMMAND motests)


