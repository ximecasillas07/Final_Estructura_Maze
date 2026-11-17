#pragma once
#include <string>
#include <vector>
#include <utility>

class Maze {
private:
    int rows = 0;
    int cols = 0;
    std::vector<std::string> grid;
    std::pair<int, int> startPos = {-1, -1}; // Coordenada de 'A' (fila, columna)
    std::pair<int, int> endPos   = {-1, -1}; // Coordenada de 'B' (fila, columna)

public:
    Maze() = default;

    // Carga el archivo .in y detecta dimensiones, paredes, A y B
    bool loadFromFile(const std::string& filepath);

    // Getters
    int getRows() const { return rows; }
    int getCols() const { return cols; }
    std::pair<int, int> getStart() const { return startPos; }
    std::pair<int, int> getEnd() const { return endPos; }

    // Consultas de celdas
    char getCell(int r, int c) const;
    bool isValid(int r, int c) const;
    bool isWall(int r, int c) const; // En tu laberinto.cpp: '0' es pared
};