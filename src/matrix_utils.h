#include <algorithm>
#include <vector>

using namespace std; 

bool vD_sortCrit_col(const vector<double>& v1, const vector<double>& v2)
{
    int col = 0;
    return v1[col] < v2[col];
}

void vD_sortByCol(vector<vector<double>>& vect) {
    sort(vect.begin(), vect.end(), vD_sortCrit_col);
}


