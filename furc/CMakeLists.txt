file(GLOB_RECURSE FURC_SOURCES "src/**.c" "src/**.h" "include/**.h")
add_executable(furc ${FURC_SOURCES})
target_include_directories(furc PUBLIC include/)
target_link_libraries(furc furlang)

target_compile_options(furc PRIVATE
  $<$<CONFIG:Debug>:
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
  >
)

set_property(TARGET furc PROPERTY C_STANDARD 11)