//
//  TTriangle.cpp
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

#include "TTriangle.hpp"

using namespace std;

TTriangle::TTriangle() { }

TTriangle::~TTriangle() { }

/**
 * Here we calculate each edge length of the element
 * but also the centroid position and the distance (x, y)
 * to each node on the element
 *
 * IMPORTANT:
 * In distance calculation between nodes some values were
 * inversed [(a - b) to (b - a)] because we need them in that
 * way for the strain matrix B and because it doesn't affect
 * any other calculation.
 **/
void TTriangle::calculateCentroid() {
    map<size_t, SNode>::iterator it = nodes.begin();
    SNode n1 = it->second; it++;
    SNode n2 = it->second; it++;
    SNode n3 = it->second;
    
    // From node 1 to node 2
    SDiff nDiff;
    nDiff.x     = n2.x - n1.x;
    nDiff.y     = n1.y - n2.y;
    nDiff.hyp   = pow(pow(nDiff.x, 2) + pow(nDiff.y, 2), 0.5);
    diffs.push_back(nDiff);
    
    // From node 2 to node 3
    nDiff.x     = n3.x - n2.x;
    nDiff.y     = n2.y - n3.y;
    nDiff.hyp   = pow(pow(nDiff.x, 2) + pow(nDiff.y, 2), 0.5);
    diffs.push_back(nDiff);
    
    // From node 3 to node 1
    nDiff.x     = n1.x - n3.x;
    nDiff.y     = n3.y - n1.y;
    nDiff.hyp   = pow(pow(nDiff.x, 2) + pow(nDiff.y, 2), 0.5);
    diffs.push_back(nDiff);
    
    // Getting the center of the element
    center.x = (n1.x + n2.x + n3.x) / 3;
    center.y = (n1.y + n2.y + n3.y) / 3;
    
    // Calculating center distances to each node
    // From center to node 1
    nDiff.x     = center.x - n1.x;
    nDiff.y     = center.y - n1.y;
    nDiff.hyp   = 0; //pow(pow(nDiff.x, 2) + pow(nDiff.y, 2), 0.5); We don't need this value so we avoid extra calcs
    centerDiffs.push_back(nDiff);
    
    // From center to node 2
    nDiff.x     = center.x - n2.x;
    nDiff.y     = center.y - n2.y;
    nDiff.hyp   = 0; //pow(pow(nDiff.x, 2) + pow(nDiff.y, 2), 0.5); We don't need this value so we avoid extra calcs
    centerDiffs.push_back(nDiff);
    
    // From center to node 3
    nDiff.x     = center.x - n3.x;
    nDiff.y     = center.y - n3.y;
    nDiff.hyp   = 0; //pow(pow(nDiff.x, 2) + pow(nDiff.y, 2), 0.5); We don't need this value so we avoid extra calcs
    centerDiffs.push_back(nDiff);
}

/**
 * Getting the area of the element
 **/
void TTriangle::calculateArea() {
    area = fabs((diffs[0].y * diffs[2].x) + (-diffs[0].x * diffs[2].y)) / 2;
}

/**
 * Getting the strain matrix B and its traspose
 * B is 2 rows and 3 cols and it came from
 * | dN/dx |
 * | dN/dy |
 * where N is the shape function
 **/
void TTriangle::calculateB() {
    setBC();
    B = gsl_matrix_alloc(2, 3);
    Bt = gsl_matrix_alloc(3, 2);
    gsl_matrix_set_all(B, 0);
    gsl_matrix_set(B, 0, 0, b[0]); gsl_matrix_set(B, 0, 1, b[1]); gsl_matrix_set(B, 0, 2, b[2]);
    gsl_matrix_set(B, 1, 0, c[0]); gsl_matrix_set(B, 1, 1, c[1]); gsl_matrix_set(B, 1, 2, c[2]);
    gsl_matrix_transpose_memcpy(Bt, B);
}

/**
 * Helper for strain matrix B calculation
 **/
void TTriangle::setBC() {
    b.push_back(diffs[1].y); b.push_back(diffs[2].y); b.push_back(diffs[0].y);
    c.push_back(diffs[1].x); c.push_back(diffs[2].x); c.push_back(diffs[0].x);
}

/**
 * It returns the edge index from element nodes i j
 * Edge 1 from Node 1 to Node 2 | [0] = [0, 1]
 * Edge 2 from Node 2 to Node 3 | [1] = [1, 2] || [2, 1]
 * Edge 3 from Node 3 to Node 1 | [2] = [0, 2]
 **/
size_t TTriangle::getEdgeIndex(size_t i, size_t j) {
    if ((i == 0) && (j == 1)) return 0;
    if ((i == 0) && (j == 2)) return 2;
    return 1;
}

/**
 * This method return the contribution of the convection boundary condition
 * for the K matrix element. The shape of the matrix is related with the nodes
 * involved in the boundary condition.
 * So for each nodes convination we have:
 * edge == 1   edge == 2   edge == 3
 * | 2 1 0 |   | 0 0 0 |   | 2 0 1 |
 * | 1 2 0 |   | 0 2 1 |   | 0 0 0 |
 * | 0 0 0 |   | 0 1 2 |   | 1 0 2 |
 * The contribution is determined scaling the matrix km by the convectivity factor:
 * (convectivity * L) / 6.0
 * where L is the edge length
 **/
gsl_matrix * TTriangle::getKm(long double convectivity) {
    gsl_matrix *km = gsl_matrix_alloc(3, 3); //k element convection
    gsl_matrix_set_all(km, 0);
    size_t i = -1, j = -1; size_t c = 0;
    map<size_t, SCondition>::iterator it;
    for (it = conditions.begin(); it != conditions.end(); it++) {
        if (it->second.type == "Convection") {
            if (i == -1) i = c; else j = c;
        }
        c++;
    }
    if (i != -1 && j != -1) {
        size_t k = getEdgeIndex(i, j);
        
        if (k == 0) {
            gsl_matrix_set(km, 0, 0, 2); gsl_matrix_set(km, 0, 1, 1); gsl_matrix_set(km, 0, 2, 0);
            gsl_matrix_set(km, 1, 0, 1); gsl_matrix_set(km, 1, 1, 2); gsl_matrix_set(km, 1, 2, 0);
            gsl_matrix_set(km, 2, 0, 0); gsl_matrix_set(km, 2, 1, 0); gsl_matrix_set(km, 2, 2, 0);
        }

        if (k == 1) {
            gsl_matrix_set(km, 0, 0, 0); gsl_matrix_set(km, 0, 1, 0); gsl_matrix_set(km, 0, 2, 0);
            gsl_matrix_set(km, 1, 0, 0); gsl_matrix_set(km, 1, 1, 2); gsl_matrix_set(km, 1, 2, 1);
            gsl_matrix_set(km, 2, 0, 0); gsl_matrix_set(km, 2, 1, 1); gsl_matrix_set(km, 2, 2, 2);
        }
        
        if (k == 2) {
            gsl_matrix_set(km, 0, 0, 2); gsl_matrix_set(km, 0, 1, 0); gsl_matrix_set(km, 0, 2, 1);
            gsl_matrix_set(km, 1, 0, 0); gsl_matrix_set(km, 1, 1, 0); gsl_matrix_set(km, 1, 2, 0);
            gsl_matrix_set(km, 2, 0, 1); gsl_matrix_set(km, 2, 1, 0); gsl_matrix_set(km, 2, 2, 2);
        }
        
        gsl_matrix_scale(km, (convectivity * getEdgeLength(k)) / 6.0);
    }
    
    return km;
}

/**
 * Get the f element contribution for the fixed temperature
 * boundary condition
 **/
gsl_vector * TTriangle::getF() {
    size_t c = 0;
    f = gsl_vector_alloc(3); gsl_vector_set_all(f, 0);
    map<size_t, SCondition>::iterator it;
    for (it = conditions.begin(); it != conditions.end(); it++) {
        if (it->second.type == "Temperature") {
            gsl_vector_set(f, c, it->second.temperature);
        }
        c++;
    }
    return f;
}

/**
 * Calculate the f element convectivity boundary condition contribution
 * Same as the km matrix it is determined by a pair of node so:
 * edge == 1   edge == 2   edge == 3
 * | 1 |       | 0 |       | 1 |
 * | 1 |       | 1 |       | 0 |
 * | 0 |       | 1 |       | 1 |
 * The contribution is calculated scaling the vector f by:
 * (convectivity * Ta * L) / 2
 * where Ta is the ambient temperature
 * and L is the edge length
 **/
gsl_vector * TTriangle::getFConvection(long double convectivity) {
    fc = gsl_vector_alloc(3); gsl_vector_set_all(fc, 0);
    size_t i = -1, j = -1; size_t c = 0;
    map<size_t, SCondition>::iterator it;
    for (it = conditions.begin(); it != conditions.end(); it++) {
        if (it->second.type == "Convection") {
            if (i == -1) i = c; else j = c;
            gsl_vector_set(fc, c, convectivity * it->second.ambient);
        }
        c++;
    }
    if (i != -1 && j != -1) {
        gsl_vector_scale(fc, getEdgeLength(getEdgeIndex(i, j)) / 2);
    } else {
        gsl_vector_set_all(fc, 0);
    }
    return fc;
}

/**
 * Calculate the f element flux boundary condition contribution
 * It is also determined by a pair of node so:
 * edge == 1   edge == 2   edge == 3
 * | 1 |       | 0 |       | 1 |
 * | 1 |       | 1 |       | 0 |
 * | 0 |       | 1 |       | 1 |
 * The contribution is calculated scaling the vector f by:
 * (flux * L) / 2
 * where L is the edge length
 **/
gsl_vector * TTriangle::getFFlux(long double convectivity) {
    ff = gsl_vector_alloc(3); gsl_vector_set_all(ff, 0);
    size_t i = -1, j = -1; size_t c = 0;
    map<size_t, SCondition>::iterator it;
    for (it = conditions.begin(); it != conditions.end(); it++) {
        if (it->second.type == "Flux") {
            if (i == -1) i = c; else j = c;
            gsl_vector_set(ff, c, it->second.flux);
        }
        c++;
    }
    if (i != -1 && j != -1) {
        gsl_vector_scale(ff, getEdgeLength(getEdgeIndex(i, j)) / 2);
    } else {
        gsl_vector_set_all(ff, 0);
    }
    return ff;
}
