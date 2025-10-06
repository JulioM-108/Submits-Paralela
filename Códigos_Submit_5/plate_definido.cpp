#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <random>
#include <iomanip>
#include <algorithm>
#include <cstdlib>

using namespace std;

class HotPlate {
private:
    int SIZE;
    static constexpr double THRESHOLD = 0.1;
    
    vector<vector<double>> plate;
    vector<vector<double>> newPlate;
    vector<vector<double>> convergenceMap;
    
    int randX, randY;
    int iterations;
    
public:
    HotPlate(int size) : SIZE(size),
                         plate(size, vector<double>(size, 0.0)),
                         newPlate(size, vector<double>(size, 0.0)),
                         convergenceMap(size, vector<double>(size, 0.0)),
                         randX(0), randY(0), iterations(0) {}
    
    void initialize() {
        random_device rd;
        mt19937 gen(rd());
        uniform_real_distribution<> disVal(0.0, 1.0);
        
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                plate[i][j] = disVal(gen);
            }
        }
        
        uniform_int_distribution<> disPos(1, SIZE - 2);
        randX = disPos(gen);
        randY = disPos(gen);
        
        plate[randX][randY] = 1.0;
        
        cout << "Posicion aleatoria: (" << randX << ", " << randY << ")" << endl;
        cout << "Valor: 1.000000" << endl;
        cout << "Tamano de la matriz: " << SIZE << "x" << SIZE << endl;
    }
    
    void update() {
        for (int i = 1; i < SIZE - 1; i++) {
            for (int j = 1; j < SIZE - 1; j++) {
                if (i == randX && j == randY) {
                    newPlate[i][j] = 1.0;
                } else {
                    newPlate[i][j] = (plate[i+1][j] + plate[i-1][j] + 
                                     plate[i][j+1] + plate[i][j-1] + 
                                     4.0 * plate[i][j]) / 8.0;
                }
            }
        }
        plate = newPlate;
    }
    
    bool checkConvergence() {
        bool hasChanged = false;
        
        for (int i = 1; i < SIZE - 1; i++) {
            for (int j = 1; j < SIZE - 1; j++) {
                double avg = (plate[i+1][j] + plate[i-1][j] + 
                             plate[i][j+1] + plate[i][j-1]) / 4.0;
                
                if (abs(plate[i][j] - avg) > THRESHOLD) {
                    convergenceMap[i][j] = 1.0;
                    hasChanged = true;
                } else {
                    convergenceMap[i][j] = 0.0;
                }
            }
        }
        
        return hasChanged;
    }
    
    void simulate(int maxIterations = 10000) {
        bool converged = false;
        iterations = 0;
        
        while (!converged && iterations < maxIterations) {
            update();
            converged = !checkConvergence();
            iterations++;
        }
        
        cout << "Convergencia alcanzada en " << iterations << " iteraciones" << endl;
    }
    
    int countHotCells() const {
        int count = 0;
        for (int i = 1; i < SIZE - 1; i++) {
            for (int j = 1; j < SIZE - 1; j++) {
                if (convergenceMap[i][j] == 1.0) {
                    count++;
                }
            }
        }
        return count;
    }
    
    void saveToFile(const string& filename) const {
        ofstream file(filename);
        if (!file.is_open()) {
            cerr << "Error al abrir el archivo " << filename << endl;
            return;
        }
        
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                file << fixed << setprecision(6) << plate[i][j];
                if (j < SIZE - 1) file << " ";
            }
            file << "\n";
        }
        
        file.close();
        cout << "Matriz guardada en: " << filename << endl;
    }
    
    void saveConvergenceMap(const string& filename) const {
        ofstream file(filename);
        if (!file.is_open()) {
            cerr << "Error al abrir el archivo " << filename << endl;
            return;
        }
        
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                file << static_cast<int>(convergenceMap[i][j]);
                if (j < SIZE - 1) file << " ";
            }
            file << "\n";
        }
        
        file.close();
        cout << "Mapa de convergencia guardado en: " << filename << endl;
    }
    
    void generateHeatmap(const string& filename) const {
        ofstream file(filename);
        if (!file.is_open()) {
            cerr << "Error al abrir el archivo " << filename << endl;
            return;
        }
        
        double minVal = plate[0][0], maxVal = plate[0][0];
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                minVal = min(minVal, plate[i][j]);
                maxVal = max(maxVal, plate[i][j]);
            }
        }
        
        file << "P3\n" << SIZE << " " << SIZE << "\n255\n";
        
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                double normalized = (plate[i][j] - minVal) / (maxVal - minVal + 1e-10);
                
                int r = static_cast<int>(255 * normalized);
                int g = 0;
                int b = static_cast<int>(255 * (1.0 - normalized));
                
                file << r << " " << g << " " << b << " ";
            }
            file << "\n";
        }
        
        file.close();
        cout << "Heatmap generado en: " << filename << endl;
    }
    
    void displayResults() const {
        int hotCells = countHotCells();
        cout << "Numero de celdas hot: " << hotCells << endl;
    }
    
    int getIterations() const {
        return iterations;
    }
};

int main(int argc, char* argv[]) {
    int size = 256;
    
    if (argc > 1) {
        size = atoi(argv[1]);
        if (size < 3) {
            cerr << "Error: El tamano debe ser al menos 3" << endl;
            return 1;
        }
    }
    
    HotPlate hotPlate(size);
    
    hotPlate.initialize();
    hotPlate.simulate();
    hotPlate.displayResults();
    hotPlate.saveToFile("plate_matrix.txt");
    hotPlate.saveConvergenceMap("convergence_map.txt");
    hotPlate.generateHeatmap("heatmap.ppm");
    
    return 0;
}