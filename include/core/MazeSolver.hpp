#pragma once
#include "core/Maze.hpp"
#include <vector>
#include <utility>
#include <set>
#include <map>
#include <queue>

enum class AlgorithmType {
    NONE,
    PREORDER,
    POSTORDER,
    DFS,
    BFS
};

enum class StepType {
    EXPLORED,
    FRONTIER,
    SOLUTION
};

struct SearchStep {
    std::pair<int, int> coord;
    StepType type;
    int stepNumber;
};

// Estadísticas para la comparativa de eficiencia
struct AlgorithmStats {
    int nodesExplored = 0;
    int pathLength = 0;
    bool calculated = false;
};

class MazeSolver {
private:
    std::vector<SearchStep> stepsLog;
    std::vector<std::pair<int, int>> solutionPath;
    bool pathFound = false;

    // Estadísticas guardadas de DFS y BFS para comparar
    AlgorithmStats statsDFS;
    AlgorithmStats statsBFS;

    void runPreorder(const Maze& maze);
    void runPostorder(const Maze& maze);
    void runDFS(const Maze& maze);
    void runBFS(const Maze& maze);

    void preorderRecursive(const Maze& maze, std::pair<int, int> coor,
                            std::set<std::pair<int, int>>& visited, int& stepCount);
    void postorderRecursive(const Maze& maze, std::pair<int, int> coor,
                            std::set<std::pair<int, int>>& visited, int& stepCount);
    void dfsRecursive(const Maze& maze, std::pair<int, int> coor,
                        std::set<std::pair<int, int>>& visited,
                        std::map<std::pair<int, int>, std::pair<int, int>>& parent,
                        int& stepCount);

public:
    MazeSolver() = default;

    void solve(const Maze& maze, AlgorithmType algorithm);
    void reset();
    void resetStats() { statsDFS = {}; statsBFS = {}; }

    const std::vector<SearchStep>& getSteps() const { return stepsLog; }
    const std::vector<std::pair<int, int>>& getSolutionPath() const { return solutionPath; }
    bool hasSolution() const { return pathFound; }
    int getTotalSteps() const { return static_cast<int>(stepsLog.size()); }

    // Getters de comparativa
    AlgorithmStats getDFSStats() const { return statsDFS; }
    AlgorithmStats getBFSStats() const { return statsBFS; }
};