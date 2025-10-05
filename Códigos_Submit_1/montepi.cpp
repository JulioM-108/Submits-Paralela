#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <random>
#include <ctime>
#include <cmath>   

int main(int argc, char* argv[]) {
    long long num_tosses = std::atoll(argv[1]);

    // Iniciar tiempos
    clock_t cpu_start = clock();
    time_t wall_start = time(NULL);

    long long in_circle = 0;
    std::mt19937_64 rng(time(NULL));
    std::uniform_real_distribution<double> dist(-1.0, 1.0);

    for (long long i = 0; i < num_tosses; i++) {
        double x = dist(rng);
        double y = dist(rng);
        if (x*x + y*y <= 1.0) in_circle++;
    }

    double pi_estimate = 4.0 * (double) in_circle / (double) num_tosses;

    // Terminar tiempos
    clock_t cpu_end = clock();
    time_t wall_end = time(NULL);

    // Calcular métricas
    double cpu_time = double(cpu_end - cpu_start) / CLOCKS_PER_SEC;
    double wall_time = wall_end - wall_start;
    double error = fabs(pi_estimate - M_PI);

    // Mostrar resultados
    std::cout << std::setprecision(17);
    std::cout << "PI estimate = " << pi_estimate << "\n";
    std::cout << "Error       = " << error << "\n";
    std::cout << "CPU time    = " << cpu_time << " s\n";
    std::cout << "Wall time   = " << wall_time << " s\n";

    return 0;
}

