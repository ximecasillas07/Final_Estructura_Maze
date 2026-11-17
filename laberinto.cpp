#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <queue>
#include <map>
#include <algorithm>


using namespace std;


int rows, cols;
vector<string> maze;


//                 A        B
pair<int, int> startPos, endPos;


//abajo, izq, arriba, der
int dr[] = {1, 0, -1, 0}; //eje vertical
int dc[] = {0, -1, 0, 1}; //eje horizontal


bool isValid(int r, int c) {
    if (r < 0 || r >= rows || c < 0 || c >= cols) return false; //verifica si sale de los limites
    if (maze[r][c] == '0') return false; //pared
    return true; //si es valdia
}


// Preorden: primero el nodo, luego sus hijos
void preorder(pair<int, int> coor, set<pair<int, int>>& visited) {
    visited.insert(coor); //marcar la coordenada actual como visitada para no repetir
    cout << "(" << coor.first << "," << coor.second << ") "; // Imprimir al entrar


    for (int i = 0; i < 4; i++) {
        int nr = coor.first + dr[i]; //la nueva fila
        int nc = coor.second + dc[i]; //la nueva columna
        pair<int, int> next = {nr, nc}; //forma el par de la coord vecina


        if (isValid(nr, nc) && visited.find(next) == visited.end()) { //si la casilla vecina es valid y aun no se ha visitado
            preorder(next, visited);
        }
    }
}


//Postorden: primero hijos, luego yo
void postorder(pair<int, int> coor, set<pair<int, int>>& visited) {
    visited.insert(coor);


    for (int i = 0; i < 4; i++) {
        int nr = coor.first + dr[i];
        int nc = coor.second + dc[i];
        pair<int, int> next = {nr, nc};


        if (isValid(nr, nc) && visited.find(next) == visited.end()) {
            postorder(next, visited);
        }
    }


    cout << "(" << coor.first << "," << coor.second << ") "; //imprimir al salir
}




//Solucion dfs
void dfsExplore(pair<int, int> coor, set<pair<int, int>>& visited, map<pair<int, int>, pair<int, int>>& parent) {
    visited.insert(coor);


    for (int i = 0; i < 4; i++) {
        int nr = coor.first + dr[i];
        int nc = coor.second + dc[i];
        pair<int, int> next = {nr, nc};


        if (isValid(nr, nc) && visited.find(next) == visited.end()) {
            parent[next] = coor; // Guardar la relación de ancestro/padre
            dfsExplore(next, visited, parent);
        }
    }
}


void dfs() {
    set<pair<int, int>> visited;
    map<pair<int, int>, pair<int, int>> parent;
   
    dfsExplore(startPos, visited, parent);


    //reconstruir la ruta usando la función de ancestros (desde B hacia A)
    pair<int, int> coor = endPos;
    vector<pair<int, int>> path;
   
    while (coor != startPos) {
        path.push_back(coor); //guarda la coordenada actual en la ruta
        coor = parent[coor]; //mueve la coordenada a su padre/ancestro
    }
    path.push_back(startPos); //incluye A


    //imprimir el recorrido de la solución de B a A
    for (auto p : path) {
        cout << "(" << p.first << "," << p.second << ") ";
    }
    cout << endl;
}


// BFS
void bfs() {
    queue<pair<int, int>> q; //queue para procesar nivel por nivel
    set<pair<int, int>> visited;
    map<pair<int, int>, pair<int, int>> parent; //map de ancestros
    vector<pair<int, int>> orderBFS; //vector para registrar el orden completo de expansion


    q.push(startPos);
    visited.insert(startPos);


    while (!q.empty()) {
        pair<int, int> coor = q.front();
        q.pop();
        orderBFS.push_back(coor); //guardar coordenada en la secuencia del recorrido


        for (int i = 0; i < 4; i++) {
            int nr = coor.first + dr[i];
            int nc = coor.second + dc[i];
            pair<int, int> next = {nr, nc};


            if (isValid(nr, nc) && visited.find(next) == visited.end()) {
                visited.insert(next);
                parent[next] = coor;
                q.push(next);
            }
        }
    }


    //imprimir el recorrido completo de BFS
    cout << "Recorrido BFS: " << endl;
    for (auto p : orderBFS) {
        cout << "(" << p.first << "," << p.second << ") ";
    }
    cout << endl << endl;


    //reconstruir e imprimir la solución BFS usando ancestros
    cout << "Solucion BFS: " << endl;
    pair<int, int> coor = endPos; //empezar desde B
    vector<pair<int, int>> path; //guardar la ruta solución


    while (coor != startPos) { //retroceder paso a paso desde B hasta A
        path.push_back(coor);
        coor = parent[coor];
    }
    path.push_back(startPos);


    for (auto p : path) {
        cout << "(" << p.first << "," << p.second << ") ";
    }
    cout << endl;
}


int main() {
    string filename;
    cout<<”Write the file’s name: “<<endl;
    cin>>filename;
    ifstream fin(filename);


    if (!fin.is_open()) {
        cout << "No se pudo abrir el archivo " << filename << endl;
        return 1;
    }


    fin >> rows;
    fin >> cols;
    cout << "El archivo tiene " << rows << " filas y " << cols << " columnas" << endl << endl;


    string line;
    for (int i = 0; i < rows; i++) {
        fin >> line;
        maze.push_back(line);
    }


    //buscar coordenadas de A y B
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            if (maze[r][c] == 'A') startPos = {r, c};
            if (maze[r][c] == 'B') endPos = {r, c};
        }
    }


    cout << "Preorden" << endl;
    set<pair<int, int>> vPre;
    preorder(startPos, vPre);
    cout << endl << endl;


    cout << "Postorden" << endl;
    set<pair<int, int>> vPost;
    postorder(startPos, vPost);
    cout << endl << endl;


    cout << "Solucion DFS" << endl;
    dfs();
    cout << endl;


    cout << "BFS recorrido y solucion" << endl;
    bfs();
    cout << endl;


    return 0;
}