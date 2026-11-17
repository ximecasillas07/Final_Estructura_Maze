# Guía de Compilación y Ejecución del Proyecto: Laberinto 3D

Este proyecto es una aplicación interactiva en C++ y OpenGL desarrollada con **Raylib** y **raygui** para la visualización 3D y resolución de laberintos mediante algoritmos de estructuras de datos (**Preorden**, **Postorden**, **DFS** y **BFS**).

---

## 1. Requisitos Previos

El proyecto utiliza CMake con el módulo estándar `FetchContent`, por lo que **descarga y compila automáticamente Raylib y raygui** durante la primera compilación. No es necesario instalar librerías externas manualmente.

Solo se requiere:
1. **CMake** versión 3.20 o superior.
2. **Git** (para que CMake pueda descargar Raylib automáticamente).
3. **Compilador C++17**:
   * **Linux:** `g++` o `clang++` (GCC 9+ / Clang 10+).
   * **Windows:** **MSVC** (Visual Studio con herramientas de C++) o **MinGW-w64**.

---

## 2. Instrucciones para Linux (Ubuntu / Debian / Mint / WSL2)

### 2.1 Instalar dependencias del sistema (librerías gráficas de X11 / OpenGL)
Abre una terminal y ejecuta el siguiente comando:

```bash
sudo apt update
sudo apt install -y build-essential cmake git \
    libasound2-dev libx11-dev libxrandr-dev libxi-dev \
    libgl1-mesa-dev libglu1-mesa-dev libxcursor-dev \
    libxinerama-dev libwayland-dev libxkbcommon-dev
```

### 2.2 Configurar y Compilar
Estando en la carpeta raíz del proyecto (`Final_Interfaz`):

```bash
# 1. Configurar la compilación
cmake -B build -DCMAKE_BUILD_TYPE=Release

# 2. Compilar en paralelo
cmake --build build -j$(nproc)
```

### 2.3 Ejecutar
```bash
./build/Maze3DVisualizer
```

---

## 3. Instrucciones para Windows

### Opción A: Usando Visual Studio / Developer PowerShell (Recomendado)
Abre PowerShell en la carpeta del proyecto y ejecuta:

```powershell
# 1. Configurar la compilación
cmake -B build

# 2. Compilar en modo Release
cmake --build build --config Release

# 3. Ejecutar la aplicación
.\build\Release\Maze3DVisualizer.exe
```

> **Nota:** Si `cmake` no está en tu variable de entorno `PATH`, puedes usar directamente el ejecutable de Visual Studio:
> ```powershell
> & "C:\Program Files\Microsoft Visual Studio\18\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" --build build --config Release
> ```

### Opción B: Usando MinGW / Ninja
```powershell
cmake -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build build
.\build\Maze3DVisualizer.exe
```

---

## 4. Guía Rápida de Uso de la Aplicación

### Controles de Navegación y Cámara:
* **`1` (o botón Cenital):** Cámara aérea superior fija (vista panorámica de todo el laberinto). En esta vista puedes usar la **rueda del ratón** para hacer zoom y alejarte todo lo que desees.
* **`2` (o botón 3ra Persona):** Cámara en tercera persona que sigue al personaje suavemente.
* **`3` (o botón 1ra Persona):** Perspectiva en primera persona (POV del personaje); usa el ratón para rotar la mirada.
* **`W, A, S, D`:** Caminar manualmente por los pasillos celda a celda.

### Opciones de Visualización (Panel Izquierdo):
* **Rayos-X:** Hace semitransparentes los muros para ver los caminos interiores (disponible en 3ra y 1ra persona).
* **Velocidad de construcción:** Controla la velocidad con la que se elevan los muros al iniciar.
* **Botones Mapa 1 a 6:** Carga cualquiera de los laberintos predeterminados (`laberinto_1.in` a `laberinto_6.in`).
* **Cargar Archivo Propio:** 
  1. Puedes hacer clic en el botón `[ 📁 Cargar Archivo Propio ]` para buscar un archivo `.in` o `.txt`.
  2. O simplemente **arrastrar cualquier archivo de laberinto desde tu explorador de archivos directamente sobre la ventana**.

### Algoritmos de Búsqueda y Solución (Panel Derecho):
1. **Selección de Algoritmo:**
   * **Preorden:** Recorrido recursivo en preorden (registra casillas al entrar).
   * **Postorden:** Recorrido recursivo en postorden (registra casillas al salir).
   * **DFS:** Búsqueda en profundidad hasta localizar la meta $B$.
   * **BFS:** Búsqueda en anchura por niveles (garantiza la ruta más corta a $B$).
2. **Controles de Reproducción:**
   * **`Play / Pausa`:** Inicia o detiene la animación cronológica de exploración (marcada en color ámbar).
   * **`Reset`:** Reinicia la búsqueda al estado original.
   * **`Paso +1`:** Avanza exactamente un paso para inspección detallada.
   * **`Fin`:** Completa la búsqueda al instante.
3. **Recorrido Autónomo del Personaje:**
   * Una vez finalizada la búsqueda con DFS o BFS, se iluminará la ruta solución en **verde esmeralda**.
   * Presiona el botón **`[ 🚶 Recorrer Solucion ]`** para que el personaje comience a caminar automáticamente por la ruta hasta la meta $B$.
   * Al llegar a la meta, celebrará su llegada.
