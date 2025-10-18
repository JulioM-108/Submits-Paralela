#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <ctime>
#include <vector>
#include <string>
#include <omp.h>

using namespace std;

class PNMImage {
private:
    char magic[3];
    int width;
    int height;
    int max_color;
    int* pixels;
    int pixel_count;

public:
    PNMImage() : width(0), height(0), max_color(0), pixels(nullptr), pixel_count(0) {
        magic[0] = '\0';
    }

    ~PNMImage() {
        if (pixels) free(pixels);
    }

    const char* getMagic() const {
        return magic;
    }

    int getWidth() const { return width; }
    int getHeight() const { return height; }
    int getMaxColor() const { return max_color; }
    int* getPixels() const { return pixels; }
    int getPixelCount() const { return pixel_count; }

 
    bool load(const char* filename) {
        FILE* file = fopen(filename, "r");
        if (!file) {
            cerr << "Error: no se pudo abrir " << filename << endl;
            return false;
        }

        if (fscanf(file, "%2s", magic) != 1) {
            cerr << "Error leyendo magic number" << endl;
            fclose(file);
            return false;
        }

        if (fscanf(file, "%d %d", &width, &height) != 2) {
            cerr << "Error leyendo width/height" << endl;
            fclose(file);
            return false;
        }

        if (fscanf(file, "%d", &max_color) != 1) {
            cerr << "Error leyendo max_color" << endl;
            fclose(file);
            return false;
        }

        pixel_count = width * height;
        if (strcmp(magic, "P3") == 0) pixel_count *= 3;

        pixels = (int*) malloc(pixel_count * sizeof(int));
        if (!pixels) {
            cerr << "Error reservando memoria" << endl;
            fclose(file);
            return false;
        }

        for (int i = 0; i < pixel_count; i++) {
            if (fscanf(file, "%d", &pixels[i]) != 1) {
                cerr << "Error leyendo píxeles en " << filename << " (pos " << i << ")" << endl;
                free(pixels);
                pixels = nullptr;
                fclose(file);
                return false;
            }
        }

        fclose(file);
        return true;
    }

    bool save(const char* filename) const {
        FILE* out = fopen(filename, "w");
        if (!out) {
            cerr << "Error: no se pudo abrir " << filename << " para escritura" << endl;
            return false;
        }

        fprintf(out, "%s\n%d %d\n%d\n", magic, width, height, max_color);

        for (int i = 0; i < pixel_count; i++) {
            fprintf(out, "%d ", pixels[i]);
            if ((i + 1) % 12 == 0) fprintf(out, "\n");
        }

        fprintf(out, "\n");
        fclose(out);
        return true;
    }

   
    void applyKernel(const float kernel[3][3]) {
        int* result_pixels = (int*) malloc(pixel_count * sizeof(int));
        if (!result_pixels) {
            cerr << "Error reservando memoria para el filtro" << endl;
            return;
        }

        int channels = (strcmp(magic, "P3") == 0) ? 3 : 1;

        #pragma omp parallel for collapse(2)
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                for (int c = 0; c < channels; c++) {
                    float sum = 0.0f, weight_sum = 0.0f;

                    for (int ky = -1; ky <= 1; ky++) {
                        for (int kx = -1; kx <= 1; kx++) {
                            int nx = x + kx, ny = y + ky;
                            if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                                int idx = (ny * width + nx) * channels + c;
                                sum += pixels[idx] * kernel[ky + 1][kx + 1];
                                weight_sum += kernel[ky + 1][kx + 1];
                            }
                        }
                    }

                    float value = (weight_sum != 0.0f) ? sum / weight_sum : sum;
                    int result = static_cast<int>(value + 0.5f);
                    result = max(0, min(max_color, result));

                    int idx = (y * width + x) * channels + c;
                    result_pixels[idx] = result;
                }
            }
        }

        free(pixels);
        pixels = result_pixels;
    }

   
    void applyGrayscale() {
        if (strcmp(magic, "P2") == 0) return; 

        if (strcmp(magic, "P3") != 0) return;

        int channels = 3;
        int new_count = width * height;
        int* new_pixels = (int*) malloc(new_count * sizeof(int));
        if (!new_pixels) {
            cerr << "Error reservando memoria para grayscale" << endl;
            return;
        }

        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                int idx3 = (y * width + x) * channels;
                int r = pixels[idx3 + 0];
                int g = pixels[idx3 + 1];
                int b = pixels[idx3 + 2];
               
                float lum = 0.299f * r + 0.587f * g + 0.114f * b;
                int val = static_cast<int>(lum + 0.5f);
                val = max(0, min(max_color, val));
                new_pixels[y * width + x] = val;
            }
        }

        free(pixels);
        pixels = new_pixels;
        pixel_count = new_count;
        magic[0] = 'P'; magic[1] = '2'; magic[2] = '\0';
    }

   
    void applyBW(int threshold = -1) {
       
        if (strcmp(magic, "P3") == 0) applyGrayscale();

        if (strcmp(magic, "P2") != 0) return;

        if (threshold < 0) {
            
            threshold = max_color / 2;
        }

        for (int i = 0; i < pixel_count; ++i) {
            pixels[i] = (pixels[i] >= threshold) ? max_color : 0;
        }
    }
};


string getBasename(const string& path) {
    size_t slash1 = path.find_last_of('/');
    size_t slash2 = path.find_last_of('\\');
    size_t slash = string::npos;
    if (slash1 != string::npos && slash2 != string::npos) slash = max(slash1, slash2);
    else if (slash1 != string::npos) slash = slash1;
    else if (slash2 != string::npos) slash = slash2;

    size_t start = (slash == string::npos) ? 0 : slash + 1;
    size_t dot = path.find_last_of('.');
    if (dot == string::npos || dot < start) dot = path.length();
    return path.substr(start, dot - start);
}

string getExtFromMagic(const char* magic) {
    if (strcmp(magic, "P3") == 0) return ".ppm";
    return ".pgm";
}

void processFile(const string& inpath, const string& outprefix, const string& filter) {
    PNMImage img;
    if (!img.load(inpath.c_str())) {
        #pragma omp critical
        cerr << "Fallo cargando " << inpath << endl;
        return;
    }

   
    if (filter == "grayscale") {
        img.applyGrayscale();
    } else if (filter == "bw" || filter == "blackwhite" || filter == "binary") {
        img.applyBW(); 
    } else if (filter == "blur") {
        const float kernel[3][3] = {
            {1.0f / 9, 1.0f / 9, 1.0f / 9},
            {1.0f / 9, 1.0f / 9, 1.0f / 9},
            {1.0f / 9, 1.0f / 9, 1.0f / 9}
        };
        img.applyKernel(kernel);
    } else if (filter == "laplace") {
        const float kernel[3][3] = {
            {0, -1, 0},
            {-1, 4, -1},
            {0, -1, 0}
        };
        img.applyKernel(kernel);
    } else if (filter == "sharpen") {
        const float kernel[3][3] = {
            {0, -1, 0},
            {-1, 5, -1},
            {0, -1, 0}
        };
        img.applyKernel(kernel);
    } else {
        #pragma omp critical
        cerr << "Filtro desconocido: " << filter << " para archivo " << inpath << endl;
        return;
    }


    string base = getBasename(inpath);
    string ext = getExtFromMagic(img.getMagic());
    string outname = outprefix + "_" + base + "_" + filter + ext;

    if (!img.save(outname.c_str())) {
        #pragma omp critical
        cerr << "Error guardando " << outname << endl;
    } else {
        #pragma omp critical
        cout << "Procesado: " << inpath << " -> " << outname << " (" << filter << ")\n";
    }
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        cout << "Uso: " << argv[0] << " <filtro> <output_prefix> <input1> [input2 ... inputN]\n";
        cout << "Filtros soportados: grayscale, bw, blur, laplace, sharpen\n";
        return 1;
    }

    string filter = argv[1];
    string outprefix = argv[2];

    vector<string> files;
    for (int i = 3; i < argc; ++i) files.push_back(argv[i]);

    double t0 = omp_get_wtime();


    #pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < (int)files.size(); ++i) {
        processFile(files[i], outprefix, filter);
    }

    double t1 = omp_get_wtime();
    cout << "Tiempo total (wall): " << (t1 - t0) << " s\n";

    return 0;
}
