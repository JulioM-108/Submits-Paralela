#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <chrono>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <omp.h>
using namespace std;

vector<double> generate_heights(size_t n, double min_h, double max_h) {
    vector<double> H;
    H.reserve(n);
    mt19937_64 rng((unsigned) chrono::high_resolution_clock::now().time_since_epoch().count());
    uniform_real_distribution<double> dist(min_h, max_h);
    for (size_t i = 0; i < n; ++i) H.push_back(dist(rng));
    return H;
}

vector<double> load_heights_from_files(const vector<string>& files) {
    vector<double> H;
    for (const auto &fname : files) {
        ifstream in(fname);
        if (!in) {
            cerr << "Warning: no se pudo abrir " << fname << " — se saltará\n";
            continue;
        }
        string line;
        while (getline(in, line)) {
            if (line.empty()) continue;
            stringstream ss(line);
            double v;
            if (ss >> v) H.push_back(v);
        }
    }
    return H;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "Uso:\n"
             << "  ./caida gen <n> <min_h> <max_h> [g]\n"
             << "  ./caida list <h1> <h2> ... [g]\n"
             << "  ./caida files [g] <file1> [file2 ...]\n";
        return 1;
    }

    string mode = argv[1];
    double g = 9.80665;
    vector<double> heights;

    if (mode == "gen") {
        if (argc < 5) return 1;
        size_t n = stoull(argv[2]);
        double min_h = stod(argv[3]);
        double max_h = stod(argv[4]);
        if (argc >= 6) g = stod(argv[5]);
        if (min_h > max_h) swap(min_h, max_h);
        heights = generate_heights(n, min_h, max_h);
    } else if (mode == "list") {
        for (int i = 2; i < argc; ++i) {
            char* endptr = nullptr;
            double val = strtod(argv[i], &endptr);
            if (endptr != nullptr && *endptr == '\0') heights.push_back(val);
        }
    } else if (mode == "files") {
        int argi = 2;
        if (argc >= 4) {
            char* endptr = nullptr;
            double maybe_g = strtod(argv[2], &endptr);
            if (endptr != nullptr && *endptr == '\0') {
                g = maybe_g;
                argi = 3;
            }
        }
        vector<string> files;
        for (int i = argi; i < argc; ++i) files.emplace_back(argv[i]);
        heights = load_heights_from_files(files);
    } else {
        return 1;
    }

    if (heights.empty()) {
        cerr << "No hay alturas para procesar.\n";
        return 1;
    }

    size_t n = heights.size();
    vector<double> times(n, -1.0);

    double t0 = omp_get_wtime();

    #pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < (int)n; ++i) {
        double h = heights[i];
        if (h < 0.0) {
            times[i] = -1.0;
            continue;
        }
        times[i] = sqrt((2.0 * h) / g);
    }

    double t1 = omp_get_wtime();

    cout << fixed << setprecision(6);
    cout << "Resultados (g = " << g << " m/s^2):\n";
    for (size_t i = 0; i < n; ++i) {
        cout << "Objeto " << i << ": altura = " << heights[i] << " m -> ";
        if (times[i] < 0.0) cout << "altura inválida\n";
        else cout << "tiempo = " << times[i] << " s\n";
    }

    cout << "Tiempo total (wall) = " << (t1 - t0) << " s\n";
    return 0;
}
