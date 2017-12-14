//
//  TElement.hpp
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

#ifndef TElement_hpp
#define TElement_hpp

#include <stdio.h>
#include <string>
#include <vector>
#include <map>

#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_linalg.h>

struct SCondition {
    std::string type;
    long double temperature;
    long double flux;
    long double ambient;
};

struct SNode {
    long double x;
    long double y;
};

struct SDiff {
    long double x;
    long double y;
    long double hyp;
};

class TElement {
    protected:
        size_t materialId;
        long double area;
        gsl_matrix * B;
        gsl_matrix * Bt;
        gsl_vector * f;
        gsl_vector * fc;
        gsl_vector * ff;
    
        SNode center;
    
        std::map<size_t, SCondition> conditions;
    
        std::vector<SDiff> diffs;
        std::vector<SDiff> centerDiffs;
    
        virtual void calculateCentroid() = 0;
        virtual void calculateArea() = 0;
        virtual void calculateB() = 0;
    
    public:
        std::map<size_t, SNode> nodes;
        void ini(std::map<size_t, SNode> ninput, std::map<size_t, SCondition> cinput, size_t minput);
        long double getArea();
        long double getPerimeter();
        long double getEdgeLength(size_t i);
        SNode getCenter();
        SDiff getDiff(size_t i);
        SDiff getCenterDiff(size_t i);
        gsl_matrix * getB();
        gsl_matrix * getBt();
        std::vector<size_t> getNodeIds();
        size_t getMaterialId();
    
        virtual size_t getEdgeIndex(size_t i, size_t j) = 0;
        virtual gsl_matrix * getKd(long double conductivity) = 0;
        virtual gsl_matrix * getKm(long double convectivity) = 0;
        virtual gsl_vector * getF() = 0;
        virtual gsl_vector * getFConvection(long double convectivity) = 0;
        virtual gsl_vector * getFFlux(long double convectivity) = 0;
};

#endif /* TElement_hpp */
