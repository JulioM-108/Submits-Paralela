#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <chrono>
#include <fstream>
#include <omp.h>

using namespace std;
using ll = long long;

vector<vector<int>> generate_vectors(size_t n, size_t m, int max_val = 100) {
    vector<vector<int>> V; V.reserve(n);
    std::mt19937 rng((unsigned) chrono::high_resolution_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> dist(0, max_val);
    for (size_t i = 0; i < n; ++i) {
        V.emplace_back();
        V.back().reserve(m);
        for (size_t j = 0; j < m; ++j) V.back().push_back(dist(rng));
    }
    return V;
}

vector<vector<int>> load_vectors_from_files(const vector<string>& files) {
    vector<vector<int>> V;
    for (const string &fname : files) {
        ifstream in(fname);
        if (!in) {
            cerr << "No se pudo abrir " << fname << " — se saltará\n";
            continue;
        }
        V.emplace_back();
        int x;
        while (in >> x) V.back().push_back(x);
    }
    return V;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "Uso:\n"
             << "  Generar vectores: ./search_vectors gen <n_vectors> <size_per_vector> <value>\n"
             << "  Desde archivos:  ./search_vectors files <value> <file1> [file2 ...]\n"
             << "Controla hilos con OMP_NUM_THREADS o omp_set_num_threads().\n";
        return 1;
    }

    string mode = argv[1];
    vector<vector<int>> vectors;
    long long target = 0;

    if (mode == "gen") {
        if (argc < 5) { cerr << "Modo gen requiere: n_vectors size_per_vector value\n"; return 1; }
        size_t n = stoull(argv[2]);
        size_t m = stoull(argv[3]);
        target = atoll(argv[4]);
        cout << "Generando " << n << " vectores de " << m << " elementos (valores 0..100) y buscando " << target << "...\n";
        vectors = generate_vectors(n, m);
    } else if (mode == "files") {
        if (argc < 4) { cerr << "Modo files requiere: value file1 [file2 ...]\n"; return 1; }
        target = atoll(argv[2]);
        vector<string> files;
        for (int i = 3; i < argc; ++i) files.push_back(argv[i]);
        cout << "Cargando " << files.size() << " ficheros y buscando " << target << "...\n";
        vectors = load_vectors_from_files(files);
    } else {
        cerr << "Modo desconocido: " << mode << "\n";
        return 1;
    }

    size_t nvec = vectors.size();
    if (nvec == 0) { cerr << "No hay vectores para procesar.\n"; return 1; }

    
    vector<int> found_idx(nvec, -1);

    double t0 = omp_get_wtime();


    #pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < (int)nvec; ++i) {
        const auto &vec = vectors[i];
        int idx = -1;
       
        for (int j = 0; j < (int)vec.size(); ++j) {
            if (vec[j] == target) { idx = j; break; } 
        }
        found_idx[i] = idx;
    }

    double t1 = omp_get_wtime();

    int count_found = 0;
    cout << "Resultados por vector:\n";
    for (size_t i = 0; i < nvec; ++i) {
        if (found_idx[i] >= 0) {
            cout << "  vec[" << i << "] (size=" << vectors[i].size() << ") -> Encontrado en índice " << found_idx[i] << "\n";
            ++count_found;
        } else {
            cout << "  vec[" << i << "] (size=" << vectors[i].size() << ") -> No encontrado\n";
        }
    }
    cout << "Número de vectores que contienen " << target << ": " << count_found << " / " << nvec << "\n";
    cout << "Tiempo total (wall) = " << (t1 - t0) << " s\n";

    return 0;
}
