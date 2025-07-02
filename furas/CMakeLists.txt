file(GLOB_RECURSE FURAS_SOURCES "src/**.c" "src/**.h" "include/**.h")
add_executable(furas ${FURAS_SOURCES})
target_include_directories(furas PUBLIC include/)
target_link_libraries(furas furlang)

target_compile_options(furas PRIVATE
  $<$<CONFIG:Debug>:
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
  >
)

set_property(TARGET furas PROPERTY C_STANDARD 11)