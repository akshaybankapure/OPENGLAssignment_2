cmake_minimum_required(VERSION 3.14)

project(voxel_engine)

# Options
option(DEBUG "Enter debug mode" OFF)
option(SHOW_FPS "Show FPS mode" OFF)

add_compile_definitions(GLEW_STATIC)
if (SHOW_FPS)
	add_compile_definitions(SHOW_FPS)
endif()
if (DEBUG)
	add_compile_definitions(DEBUG)
endif()
# Check packages

# GET OpenGL
find_package(OpenGL REQUIRED)
set(OPENGL_LIBRARY "opengl32.lib")

# GET glew
# find_package(GLEW REQUIRED)
set(GLEW_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/vendor/glew/include")
if (DEBUG)
	set(GLEW_LIBRARIES "${PROJECT_SOURCE_DIR}/vendor/glew/lib/Debug/glew32ds.lib")
else()
	set(GLEW_LIBRARIES "${PROJECT_SOURCE_DIR}/vendor/glew/lib/Release/glew32s.lib")
endif()

# GET glfw
# find_package(glfw3 REQUIRED)
set(GLFW_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/vendor/glfw/include")
if (DEBUG)
	set(GLFW_LIBRARIES "${PROJECT_SOURCE_DIR}/vendor/glfw/lib/Debug/glfw3.lib")
else()
	set(GLFW_LIBRARIES "${PROJECT_SOURCE_DIR}/vendor/glfw/lib/Release/glfw3.lib")
endif()

# GET glm
# find_package(glm REQUIRED)
set(GLM_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/vendor/glm/include")

# ADD Internal libraries
add_library(chunk "${PROJECT_SOURCE_DIR}/src/chunk/chunk.cc")
add_library(event_handler "${PROJECT_SOURCE_DIR}/src/event_handler/opengl_event_handler.cc")
add_library(graphics_library "${PROJECT_SOURCE_DIR}/src/graphics_library/opengl_graphics_library.cc")
add_library(renderer "${PROJECT_SOURCE_DIR}/src/renderer/opengl_renderer.cc")
add_library(shader "${PROJECT_SOURCE_DIR}/src/shader/opengl_shader.cc")
add_library(window "${PROJECT_SOURCE_DIR}/src/window/opengl_window.cc")

# Internal libs
list(APPEND INTERNAL_LIBS chunk
	event_handler
	graphics_library
	renderer
	shader
	window
)

# Include
include_directories(
	${OPENGL_INCLUDE_DIRS}
	${GLEW_INCLUDE_DIRS}
	${GLFW_INCLUDE_DIRS}
	${GLM_INCLUDE_DIRS}
)

# External libs
list(APPEND EXTRA_LIBS ${OPENGL_opengl_LIBRARY}
	${OPENGL_LIBRARY}
	${GLEW_LIBRARIES}
	${GLFW_LIBRARIES}
)

add_executable(voxel_engine main.cc)

target_link_libraries(voxel_engine ${INTERNAL_LIBS} ${EXTRA_LIBS})
