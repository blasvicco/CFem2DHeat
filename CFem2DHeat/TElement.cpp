//
//  TElement.cpp
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

#include "TElement.hpp"

using namespace std;

void TElement::ini(map<size_t, SNode> ninput, std::map<size_t, SCondition> cinput, size_t minput) {
    nodes       = ninput;
    conditions  = cinput;
    materialId  = minput;
    calculateCentroid();
    calculateArea();
    calculateB();
}

long double TElement::getArea() {
    return area;
}

long double TElement::getEdgeLength(size_t i) {
    return diffs[i].hyp;
}

SNode TElement::getCenter() {
    return center;
}

SDiff TElement::getDiff(size_t i) {
    return diffs[i];
}

SDiff TElement::getCenterDiff(size_t i) {
    return centerDiffs[i];
}

gsl_matrix * TElement::getB() {
    return B;
}

gsl_matrix * TElement::getBt() {
    return Bt;
}

vector<size_t> TElement::getNodeIds() {
    vector<size_t> ids;
    map<size_t, SNode>::iterator it;
    for (it = nodes.begin(); it != nodes.end(); it++) ids.push_back(it->first);
    return ids;
}

size_t TElement::getMaterialId() {
    return materialId;
}
