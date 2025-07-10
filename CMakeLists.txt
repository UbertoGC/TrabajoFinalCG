cmake_minimum_required(VERSION 3.10)
project(OpenGL_OpenCV_Integration)

# Configurar estándar C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Configuración de OpenCV
find_package(OpenCV REQUIRED)
if(OpenCV_FOUND)
    message(STATUS "OpenCV encontrado: ${OpenCV_VERSION}")
    include_directories(${OpenCV_INCLUDE_DIRS})
else()
    message(FATAL_ERROR "OpenCV no encontrado")
endif()

# Archivos fuente
set(SOURCES
    src/calibracion_imagen.cpp
    C:/glad/src/glad.c
)

# Crear ejecutable
add_executable(OpenGL_OpenCV_Integration ${SOURCES})

# Incluir directorios
target_include_directories(OpenGL_OpenCV_Integration PRIVATE
    C:/glad/include
    C:/glfw-3.4.bin.WIN64/include
    C:/opt/glm
    ${OpenCV_INCLUDE_DIRS}
)

# Configurar directorios de librerías
link_directories(
    C:/glfw-3.4.bin.WIN64/lib-mingw-w64
)

# Enlazar librerías
target_link_libraries(OpenGL_OpenCV_Integration
    glfw3
    opengl32
    gdi32
    ${OpenCV_LIBS}
)

# Configuraciones adicionales para Windows
if(WIN32)
    add_definitions(-DGLFW_INCLUDE_NONE)
endif()