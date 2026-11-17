#include "core/Maze.hpp"
#include <fstream>
#include <iostream>

bool Maze::loadFromFile(const std::string& filepath) {
    std::ifstream fin(filepath);
    if (!fin.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo de laberinto: " << filepath << std::endl;
        return false;
    }

    grid.clear();
    fin >> rows >> cols;

    std::string line;
    for (int i = 0; i < rows; ++i) {
        fin >> line;
        grid.push_back(line);
    }

    // Localizar 'A' (inicio) y 'B' (meta)
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            if (grid[r][c] == 'A') startPos = {r, c};
            if (grid[r][c] == 'B') endPos   = {r, c};
        }
    }

    fin.close();
    return true;
}

char Maze::getCell(int r, int c) const {
    if (r < 0 || r >= rows || c < 0 || c >= cols) return '0';
    return grid[r][c];
}

bool Maze::isValid(int r, int c) const {
    if (r < 0 || r >= rows || c < 0 || c >= cols) return false;
    // En tu laberinto.cpp '0' representa pared/obstáculo
    return grid[r][c] != '0';
}

bool Maze::isWall(int r, int c) const {
    return !isValid(r, c);
}