#include "core/MazeSolver.hpp"
#include <algorithm>

static const int dr[] = { 1, 0, -1, 0 };
static const int dc[] = { 0, -1, 0, 1 };

void MazeSolver::reset() {
    stepsLog.clear();
    solutionPath.clear();
    pathFound = false;
}

void MazeSolver::solve(const Maze& maze, AlgorithmType algorithm) {
    reset();
    if (algorithm == AlgorithmType::NONE) return;

    if (algorithm == AlgorithmType::PREORDER) {
        runPreorder(maze);
    } else if (algorithm == AlgorithmType::POSTORDER) {
        runPostorder(maze);
    } else if (algorithm == AlgorithmType::DFS) {
        runDFS(maze);
    } else if (algorithm == AlgorithmType::BFS) {
        runBFS(maze);
    }
}

// 1. PREORDEN
void MazeSolver::preorderRecursive(const Maze& maze, std::pair<int, int> coor,
                                    std::set<std::pair<int, int>>& visited, int& stepCount) {
    visited.insert(coor);
    stepsLog.push_back({ coor, StepType::EXPLORED, ++stepCount });

    for (int i = 0; i < 4; ++i) {
        int nr = coor.first + dr[i];
        int nc = coor.second + dc[i];
        std::pair<int, int> next = { nr, nc };

        if (maze.isValid(nr, nc) && visited.find(next) == visited.end()) {
            preorderRecursive(maze, next, visited, stepCount);
        }
    }
}

void MazeSolver::runPreorder(const Maze& maze) {
    std::set<std::pair<int, int>> visited;
    int stepCount = 0;
    preorderRecursive(maze, maze.getStart(), visited, stepCount);
}

// 2. POSTORDEN
void MazeSolver::postorderRecursive(const Maze& maze, std::pair<int, int> coor,
                                    std::set<std::pair<int, int>>& visited, int& stepCount) {
    visited.insert(coor);

    for (int i = 0; i < 4; ++i) {
        int nr = coor.first + dr[i];
        int nc = coor.second + dc[i];
        std::pair<int, int> next = { nr, nc };

        if (maze.isValid(nr, nc) && visited.find(next) == visited.end()) {
            postorderRecursive(maze, next, visited, stepCount);
        }
    }

    stepsLog.push_back({ coor, StepType::EXPLORED, ++stepCount });
}

void MazeSolver::runPostorder(const Maze& maze) {
    std::set<std::pair<int, int>> visited;
    int stepCount = 0;
    postorderRecursive(maze, maze.getStart(), visited, stepCount);
}

// 3. DFS
void MazeSolver::dfsRecursive(const Maze& maze, std::pair<int, int> coor,
                                std::set<std::pair<int, int>>& visited,
                                std::map<std::pair<int, int>, std::pair<int, int>>& parent,
                                int& stepCount) {
    visited.insert(coor);
    stepsLog.push_back({ coor, StepType::EXPLORED, ++stepCount });

    if (coor == maze.getEnd()) {
        pathFound = true;
        return;
    }

    for (int i = 0; i < 4; ++i) {
        int nr = coor.first + dr[i];
        int nc = coor.second + dc[i];
        std::pair<int, int> next = { nr, nc };

        if (maze.isValid(nr, nc) && visited.find(next) == visited.end()) {
            parent[next] = coor;
            dfsRecursive(maze, next, visited, parent, stepCount);
            if (pathFound) return;
        }
    }
}

void MazeSolver::runDFS(const Maze& maze) {
    std::set<std::pair<int, int>> visited;
    std::map<std::pair<int, int>, std::pair<int, int>> parent;
    int stepCount = 0;

    dfsRecursive(maze, maze.getStart(), visited, parent, stepCount);

    if (pathFound) {
        std::pair<int, int> curr = maze.getEnd();
        while (curr != maze.getStart()) {
            solutionPath.push_back(curr);
            curr = parent[curr];
        }
        solutionPath.push_back(maze.getStart());
        std::reverse(solutionPath.begin(), solutionPath.end());
    }

    // Registrar estadísticas para comparativa
    statsDFS = { stepCount, (int)solutionPath.size(), true };
}

// 4. BFS
void MazeSolver::runBFS(const Maze& maze) {
    std::queue<std::pair<int, int>> q;
    std::set<std::pair<int, int>> visited;
    std::map<std::pair<int, int>, std::pair<int, int>> parent;
    int stepCount = 0;

    q.push(maze.getStart());
    visited.insert(maze.getStart());
    stepsLog.push_back({ maze.getStart(), StepType::EXPLORED, ++stepCount });

    while (!q.empty()) {
        std::pair<int, int> coor = q.front();
        q.pop();

        if (coor == maze.getEnd()) {
            pathFound = true;
            break;
        }

        for (int i = 0; i < 4; ++i) {
            int nr = coor.first + dr[i];
            int nc = coor.second + dc[i];
            std::pair<int, int> next = { nr, nc };

            if (maze.isValid(nr, nc) && visited.find(next) == visited.end()) {
                visited.insert(next);
                parent[next] = coor;
                q.push(next);
                stepsLog.push_back({ next, StepType::EXPLORED, ++stepCount });
            }
        }
    }

    if (pathFound) {
        std::pair<int, int> curr = maze.getEnd();
        while (curr != maze.getStart()) {
            solutionPath.push_back(curr);
            curr = parent[curr];
        }
        solutionPath.push_back(maze.getStart());
        std::reverse(solutionPath.begin(), solutionPath.end());
    }

    // Registrar estadísticas para comparativa
    statsBFS = { stepCount, (int)solutionPath.size(), true };
}