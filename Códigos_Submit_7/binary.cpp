#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iomanip>
using namespace std;

int binary_search_serial(const vector<int>& arr, int x) {
    int low = 0, high = arr.size() - 1;
    while (low <= high) {
        int mid = low + (high - low) / 2;
        if (arr[mid] == x)
            return mid;
        else if (arr[mid] < x)
            low = mid + 1;
        else
            high = mid - 1;
    }
    return -1;
}

int main(int argc, char* argv[]) {
    int n = 1000000;
    if (argc > 1)
        n = atoi(argv[1]);

    vector<int> data(n);
    srand(time(nullptr));
    for (int i = 0; i < n; i++)
        data[i] = rand() % (n * 10);

    int target = data[rand() % n];

    cout << "Tamaño del arreglo: " << n << endl;
    cout << "Valor a buscar: " << target << endl << endl;

    vector<int> sorted_data = data;
    sort(sorted_data.begin(), sorted_data.end());

    cout << " Caso 1: Arreglo ordenado" << endl;
    clock_t start_ord = clock();
    int pos_ord = binary_search_serial(sorted_data, target);
    clock_t end_ord = clock();
    double time_ord = double(end_ord - start_ord) / CLOCKS_PER_SEC;
    if (pos_ord != -1)
        cout << "Elemento encontrado en la posición " << pos_ord << endl;
    else
        cout << "Elemento no encontrado" << endl;
    cout << fixed << setprecision(6);
    cout << "Tiempo (ordenado): " << time_ord << " segundos" << endl << endl;

    cout << " Caso 2: Arreglo no ordenado" << endl;
    clock_t start_unord = clock();
    int pos_unord = binary_search_serial(data, target);
    clock_t end_unord = clock();
    double time_unord = double(end_unord - start_unord) / CLOCKS_PER_SEC;
    if (pos_unord != -1)
        cout << "Elemento encontrado (resultado posiblemente incorrecto)" << endl;
    else
        cout << "Elemento no encontrado (búsqueda binaria falla sin orden)" << endl;
    cout << "Tiempo (no ordenado): " << time_unord << " segundos" << endl;

    return 0;
}
