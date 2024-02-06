#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

// Función para calcular el percentil de un valor en un vector
double calcularPercentil(const std::vector<double>& valores, double valor) {
    // Copia el vector para no alterar el orden original
    std::vector<double> copiaValores = valores;

    // Ordena el vector de valores
    std::sort(copiaValores.begin(), copiaValores.end());

    // Encuentra la posición del valor en el vector ordenado
    auto it = std::lower_bound(copiaValores.begin(), copiaValores.end(), valor);

    // Calcula el percentil usando la fórmula de la distribución empírica de probabilidad
    double percentil = (it - copiaValores.begin()) / static_cast<double>(copiaValores.size()) * 100.0;

    return percentil;
}

vector<double> calcularPercentilVector(const std::vector<double>& dist, vector<double>& valores) {
    // Copia el vector para no alterar el orden original
    vector<double> percentils;

    std::vector<double> copiaValores = dist;

    // Ordena el vector de dist
    std::sort(copiaValores.begin(), copiaValores.end());

    for(double valor : valores) {
        // Encuentra la posición del valor en el vector ordenado
        auto it = std::lower_bound(copiaValores.begin(), copiaValores.end(), valor);

        // Calcula el percentil usando la fórmula de la distribución empírica de probabilidad
        double percentil = (it - copiaValores.begin()) / static_cast<double>(copiaValores.size()) * 100.0;
        percentils.push_back(percentil);
    }
    

    return percentils;
}
