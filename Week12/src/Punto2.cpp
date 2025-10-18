#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <chrono>
#include <fstream>
#include <sstream>
#include <omp.h>

using namespace std;
using ll = long long;


vector<vector<int>> generate_vectors(size_t n, size_t m, int max_val = 100) {
    vector<vector<int>> V;
    V.reserve(n);
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
             << "  Generar vectores: " << argv[0] << " gen <n_vectors> <size_per_vector>\n"
             << "  Desde archivos:  " << argv[0] << " files <out_prefix> <file1> [file2 ...]\n"
             << "Controla hilos con OMP_NUM_THREADS o modifica omp_set_num_threads().\n";
        return 1;
    }

    string mode = argv[1];
    vector<vector<int>> vectors;

    if (mode == "gen") {
        if (argc < 4) {
            cerr << "Modo gen requiere n_vectors y size_per_vector\n";
            return 1;
        }
        size_t n = stoull(argv[2]);
        size_t m = stoull(argv[3]);
        cout << "Generando " << n << " vectores de " << m << " elementos...\n";
        vectors = generate_vectors(n, m);
    } else if (mode == "files") {
        if (argc < 4) {
            cerr << "Modo files requiere un prefijo de salida y al menos un fichero\n";
            return 1;
        }
       
        vector<string> files;
        for (int i = 3; i < argc; ++i) files.push_back(argv[i]);
        cout << "Cargando " << files.size() << " ficheros...\n";
        vectors = load_vectors_from_files(files);
    } else {
        cerr << "Modo desconocido: " << mode << "\n";
        return 1;
    }

    size_t nvec = vectors.size();
    if (nvec == 0) {
        cerr << "No hay vectores para procesar.\n";
        return 1;
    }

    vector<long long> sums(nvec, 0);

    double t0 = omp_get_wtime();

   
    #pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < (int)nvec; ++i) {
        
        ll s = 0;
        const auto &vec = vectors[i];


        for (size_t j = 0; j < vec.size(); ++j) s += vec[j];



        sums[i] = s;
    }

    double t1 = omp_get_wtime();

    
    cout << "Suma por vector (" << nvec << " vectores):\n";
    for (size_t i = 0; i < nvec; ++i) {
        cout << "  vec[" << i << "] (size=" << vectors[i].size() << ") -> sum = " << sums[i] << "\n";
    }

 
    long long total = 0;
    for (auto v : sums) total += v;
    cout << "Suma total verificada = " << total << "\n";
    cout << "Tiempo total (wall) = " << (t1 - t0) << " s\n";

    return 0;
}
