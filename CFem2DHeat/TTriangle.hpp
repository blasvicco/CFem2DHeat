//
//  TTriangle.hpp
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

#ifndef TTriangle_hpp
#define TTriangle_hpp

#include <stdio.h>
#include <math.h>

#include "TElement.hpp"

class TTriangle : public TElement {
    private:
        std::vector<long double> b;
        std::vector<long double> c;
        void calculateCentroid();
        void calculateArea();
        void calculateB();
        void setBC();
    
    public:
        TTriangle();
        virtual ~TTriangle();
    
        size_t getEdgeIndex(size_t i, size_t j);
        gsl_matrix * getKd(long double conductivity);
        gsl_matrix * getKm(long double convectivity);
        gsl_vector * getF();
        gsl_vector * getFConvection(long double convectivity);
        gsl_vector * getFFlux(long double convectivity);
};

#endif /* TTriangle_hpp */
