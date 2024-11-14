cmake_minimum_required (VERSION 3.20)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()


project ("llvm-tools")
set(CMAKE_CXX_STANDARD 20)


set(CLANG_DIR C:/llvm)

include_directories(${CLANG_DIR}/include)
link_directories(${CLANG_DIR}/lib)
file(GLOB CLANG_LIB_FILES "${CLANG_DIR}/lib/*.lib")
link_libraries(${CLANG_LIB_FILES} Ws2_32.lib Version.lib)

add_subdirectory(MemberNameAnalyzer)


