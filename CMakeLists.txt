# CMakeList.txt : CMake project for CommandEmitter, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("MOClientServer")

include_directories(include)
file(GLOB SOURCES src/*.cpp)
file(GLOB HEADERS include/*.h)
# Add source to this project's executable.
add_executable (MOClientServer 
${SOURCES}
${HEADERS}
)

set_property(TARGET MOClientServer PROPERTY CXX_STANDARD 14)


# TODO: Add tests and install targets if needed.
