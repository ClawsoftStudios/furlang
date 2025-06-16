file(GLOB_RECURSE FURLANG_SOURCES "src/**.c" "src/**.h" "include/**.h")
add_library(furlang STATIC ${FURLANG_SOURCES})
target_include_directories(furlang PUBLIC include/)

target_compile_options(furlang PRIVATE
  $<$<CONFIG:Debug>:
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
  >
)

set_property(TARGET furlang PROPERTY C_STANDARD 11)