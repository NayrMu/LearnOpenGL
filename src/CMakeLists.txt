# Create an executable target and specify the source files
add_executable(source_files
    "main.c"
    "glad.c"
    "Shader.c"
    "Shader.h"
    "stb_image.h"
    "STBIMAGE.c"
    "vMaths.c"
    "vMaths.h"
)


# Link against GLFW
target_link_libraries(source_files PRIVATE glfw3)


if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET source_files PROPERTY CXX_STANDARD 20)
endif()