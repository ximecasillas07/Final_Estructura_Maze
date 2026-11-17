cmake_minimum_required(VERSION 3.20)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5 CACHE STRING "" FORCE)
project(Maze3DVisualizer CXX)

# Estándar de C++ moderno
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Incluir módulo para descargar dependencias automáticamente
include(FetchContent)

# 1. Descarga y configuración de Raylib (v5.0)
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG master
)

# Desactivar ejemplos y pruebas de Raylib para acelerar la compilación
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(raylib)

# 2. Descarga de raygui (librería de interfaz gráfica header-only)
FetchContent_Declare(
    raygui
    GIT_REPOSITORY https://github.com/raysan5/raygui.git
    GIT_TAG master
)
FetchContent_MakeAvailable(raygui)

# 3. Directorios de encabezados del proyecto y librerías
include_directories(
    include
    ${raygui_SOURCE_DIR}/src
)

# 4. Declaración del ejecutable
# En esta etapa inicial usaremos solo main.cpp para verificar el enlace
file(GLOB_RECURSE PROJECT_SOURCES "src/*.cpp")

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

# 5. Enlace con Raylib
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)

# 6. Copiar la carpeta assets automáticamente al directorio de compilación
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
)