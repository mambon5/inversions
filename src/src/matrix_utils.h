#include <algorithm>
#include <vector>

using namespace std; 

bool vD_sortCrit_2col(const vector<double>& v1, const vector<double>& v2)
{
    int col1 = 3; // columna 3
    int col2 = 1; // columna 2
    if(v1[col1] == v2[col1]) return v1[col2] > v2[col2]; // si la columna 3 és igual en les dues files, 
    return v1[col1] > v2[col1];                             // ordena doncs per la columna 2. Sino, torna el que tingui la columna 3 més gran
}

void vD_sortBy2Col(vector<vector<double>>& vect) {
    sort(vect.begin(), vect.end(), vD_sortCrit_2col);
}


#include <cmath>

double guanymaxim(vector<double>& valors) {

    if(valors.size() < 2) return -1; // NAN

    // busca últim valor vàlid (>0)
    double val_actual = -1;
    for(int i = valors.size()-1; i >= 0; i--) {
        if(std::isfinite(valors[i]) && valors[i] > 0) {
            val_actual = valors[i];
            break;
        }
    }

    if(val_actual <= 0) return -1; // evita divisió per zero

    // busca màxim vàlid
    double maxi = -1;
    for(double v : valors) {
        if(std::isfinite(v) && v > maxi)
            maxi = v;
    }

    if(maxi <= 0) return -1;

    double guany = 100 * (maxi / val_actual - 1);

    // evita infinits o números absurds
    if(!std::isfinite(guany)) return -1;

    return guany;
}


// antic codi amb errors de INF
// double guanymaxim(vector<double>& valors) {
//     // per calcular el % de guany esperat si el valor actual puja al màxim
//     double maxi = *max_element(valors.begin(), valors.end());
//     double val_actual = valors.back();
//     if(val_actual == 0) val_actual = valors[valors.size()-2] ; // pilla el penúltim valor si l'últim valor és 0
//     double guany = 100*( (maxi)/val_actual -1 );
//     // cout << "maxim : " << maxi << ", valor actual: " << val_actual << ", guany: " << guany << endl;
//     // cout << "valors: " << endl;
//     // OutputVectorDouble(valors);
//     return guany;
// }



