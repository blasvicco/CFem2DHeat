//
//  TSGsl.cpp
//  CFem2DHeat
//
//  Created by Blas Eugenio Vicco on 11/7/17.
//  Copyright © 2017 Blas Eugenio Vicco. All rights reserved.
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <https://www.gnu.org/licenses/>.
//

#include "TSGsl.hpp"

using namespace std;

void TSGsl::gsl_show_matrix(gsl_matrix matrix) {
    for (size_t i = 0; i < matrix.size1; i++) {
        for (size_t j = 0; j < matrix.size2; j++) {
            cout << TSString::ftos(gsl_matrix_get(&matrix,i,j)) << "  ";
        }
        cout << endl;
    }
}

void TSGsl::gsl_show_vector(gsl_vector vect) {
    for (size_t i = 0; i < vect.size; i++)
        cout << TSString::ftos(gsl_vector_get(&vect,i)) << "  ";
    cout << endl;
}
