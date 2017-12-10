//
//  TSGsl.hpp
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

#ifndef TSGsl_hpp
#define TSGsl_hpp

#include <stdio.h>
#include <iostream>

#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>

#include "TSString.hpp"

class TSGsl {
    public:
        static void gsl_show_matrix(gsl_matrix matrix);
        static void gsl_show_vector(gsl_vector vect);
};

#endif /* TSGsl_hpp */
