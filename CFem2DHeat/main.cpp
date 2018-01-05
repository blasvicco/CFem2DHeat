//
//  main.cpp
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

#include <iostream>

#include "TInputParser.hpp"
#include "TSGsl.hpp"

using namespace std;

int main(int argc, const char * argv[]) {
    const map<string, unsigned int> verbosity = {{"-v", 1}, {"-vv", 2}, {"-vvv", 3}};
    string fileName = argv[1] ? (string)argv[1] + ".dat" : "";
    string verbose = argv[2] ? (string)argv[2] : "";
    
    unsigned int verbosityLevel = verbosity.find(verbose) != verbosity.end()
        ? verbosity.at(verbose)
        : 0;

    if ( verbosityLevel >= 1) {
        cout << "Loading problem solver..." << endl;
        cout << "Reading input file: " << fileName << endl << endl;
    }
    
    /**
     * Reading GID pre processing file
     * Loading everything in memory so take care when the input file is too large
     * TODO: Improve reading input file method.
     **/
    TInputParser::readFile(fileName);
    if (TInputParser::getStatus() == TInputParser::FAIL) {
        throw runtime_error("ERROR: Cannot open the input file.");
    }
    
    /**
     * Printing problem dimension
     * Number of Nodes
     * Number of elements
     **/
    size_t amountOfNodes    = TInputParser::getAmountOfNodes();
    size_t amountOfElements = TInputParser::getAmountOfElements();
    if ( verbosityLevel >= 1) {
        cout << "Problem dimension:" << endl;
        cout << "Number of nodes (" << amountOfNodes << ")" << endl;
        cout << "Number of elements (" << amountOfElements << ")" << endl << endl;
    }
    
    /**
     * Printing problem details
     * Only if verbosity >= 3
     **/
    if ( verbosityLevel >= 3) {
        TInputParser::printConditions();
        TInputParser::printCoordinates();
        TInputParser::printConnectivities();
        TInputParser::printMaterials();
        cout << endl;
    }
    
    /**
     * Begining of the assembling matrix
     **/
    if ( verbosityLevel >= 1) cout << "Assembling matrixs..." << endl;
    
    /**
     * Getting information of the Input static object
     **/
    map<size_t, TElement*> connectivities   = TInputParser::getConnectivities();
    map<size_t, SCondition> conditions      = TInputParser::getConditions();
    map<size_t, SMaterial> materials        = TInputParser::getMaterials();
    
    /**
     * Memory alloc and initialization of the needed matrix and vectors
     * K/F = A
     **/
    gsl_matrix *K = gsl_matrix_alloc(amountOfNodes, amountOfNodes); gsl_matrix_set_all(K, 0);
    gsl_vector *F = gsl_vector_alloc(amountOfNodes); gsl_vector_set_all(F, 0);
    gsl_vector *A = gsl_vector_alloc(amountOfNodes); gsl_vector_set_all(A, 0);
    
    /**
     * For each element in the problem we calculate k and f
     **/
    for (size_t i = 1; i <= amountOfElements; i++) {
        if ( verbosityLevel >= 2) cout << "Processing element " << i << endl;
        
        // Getting the element from the conectivities array.
        TElement *OElement  = connectivities[i];
        size_t amountOfNPE  = OElement->nodes.size(); // Nodes per element
        
        // Getting some element properties
        long double conductivity  = materials[OElement->getMaterialId()].conductivity;
        long double convectivity  = materials[OElement->getMaterialId()].convectivity;
        
        // Getting k element conductivity contribution
        gsl_matrix *ke  = OElement->getKd(conductivity);
        
        // Getting k element convection contribution if any
        gsl_matrix *km  = OElement->getKm(convectivity);
        
        // Getting element boundary condition contributions
        gsl_vector *fe  = OElement->getF(); // f element contribution (Fix temperature if any)
        gsl_vector *fec = OElement->getFConvection(convectivity); // f element convection contribution
        gsl_vector *fef = OElement->getFFlux(convectivity); // f element flux contribution
        
        // Printing values Ks and Fs for the element if verbosity >= 3
        if (verbosityLevel >= 3) {
            cout << "Element K: " << endl; TSGsl::gsl_show_matrix(*ke); cout << endl;
            cout << "Element Km: " << endl; TSGsl::gsl_show_matrix(*km); cout << endl;
            
            cout << "Element F: " << endl; TSGsl::gsl_show_vector(*fe); cout << endl;
            cout << "Element Fc: " << endl; TSGsl::gsl_show_vector(*fec); cout << endl;
            cout << "Element Ff: " << endl; TSGsl::gsl_show_vector(*fef); cout << endl;
        }
        
        /**
         * For each node in the element we get contribution values for the global K/F assembling
         **/
        gsl_matrix_add(ke, km);
        vector<size_t> nodeIds = OElement->getNodeIds();
        for (size_t j = 0; j < amountOfNPE; j++) {
            size_t nodeJ = nodeIds[j] - 1;
            // Element ke into global K
            for (size_t k = 0; k < amountOfNPE; k++) {
                size_t nodeK = nodeIds[k] - 1;
                long double value = gsl_matrix_get(K, nodeJ, nodeK);
                value += gsl_matrix_get(ke, j, k);
                gsl_matrix_set(K, nodeJ, nodeK, value);
            }
            
            gsl_vector_set(F, nodeJ, gsl_vector_get(F, nodeJ) + gsl_vector_get(fec, j)); // element convection into global F
            gsl_vector_set(F, nodeJ, gsl_vector_get(F, nodeJ) + gsl_vector_get(fef, j)); // element flux into global F
            
            // If fixed temperature we set F as fixed T in the node position
            // and K row as 1 for the current fixed temperature node position
            // and ceros for any other node position
            if (conditions.find(nodeIds[j]) != conditions.end() && (conditions[nodeIds[j]].type == "Temperature")) {
                gsl_vector_set(F, nodeJ, gsl_vector_get(fe, j));
                gsl_vector *vecTmp;
                vecTmp = gsl_vector_alloc(amountOfNodes);
                gsl_vector_set_all(vecTmp, 0);
                gsl_vector_set(vecTmp, nodeJ, 1);
                gsl_matrix_set_row(K, nodeJ, vecTmp);
            }
        }
    }

    // Just printing the assembled global K/F if verbosity >= 2
    if (verbosityLevel >= 2) {
        cout << endl << "Equation system matrix assembled" << endl;
        cout << "K: " << endl; TSGsl::gsl_show_matrix(*K); cout << endl;
        cout << "F: " << endl; TSGsl::gsl_show_vector(*F); cout << endl;
    }
    
    if (verbosityLevel >= 1)
        cout << "Solving using linal Householder solver (gsl_linalg_HH_solve)..." << endl;
    
    /**
     * Solving linear K/F equation using House Holder solver
     **/
    gsl_linalg_HH_solve(K, F, A);

    // Printing the Temperature distribution obtained from K/F resolution
    if (verbosityLevel >= 2) {
        cout << "Temperature disribution: " << endl; TSGsl::gsl_show_vector(*A); cout << endl;
    }
    
    if (verbosityLevel >= 1) cout << "Estimating Flux Heat vectors..." << endl;
    
    /**
     * Really poor gradient estimation from temperature distribution
     * TODO: Research and implement a better approach of flux estimation
     * For example "Super convergent points for the flux"
     **/
    
    // Flux contribution by 2D axes
    gsl_vector *xFlux  = gsl_vector_alloc(amountOfNodes); gsl_vector_set_all(xFlux, 0);
    gsl_vector *yFlux  = gsl_vector_alloc(amountOfNodes); gsl_vector_set_all(yFlux, 0);
    
    // Counters of elements contribution per node
    gsl_vector *xFluxC  = gsl_vector_alloc(amountOfNodes);  gsl_vector_set_all(xFluxC, 0);
    gsl_vector *yFluxC  = gsl_vector_alloc(amountOfNodes);  gsl_vector_set_all(yFluxC, 0);
    
    /**
     * For each element in the problem...
     **/
    for (size_t i = 1; i <= amountOfElements; i++) {
        TElement *OElement = connectivities[i];
        size_t amountOfNPE = OElement->nodes.size();
        long double conductivity = materials[OElement->getMaterialId()].conductivity;
        long double temp = 0;
        
        vector<size_t> nodeIds = OElement->getNodeIds();
        // Getting the avg of nodal temperatures
        for (size_t j = 0; j < amountOfNPE; j++) {
            temp += gsl_vector_get(A, nodeIds[j] - 1);
        }
        temp /= amountOfNPE;
        
        // For each node in element
        for (size_t j = 0; j < amountOfNPE; j++) {
            size_t nodeJ = nodeIds[j] - 1;
            SDiff nDiff = OElement->getCenterDiff(j); // Distance to the centroid
            long double kT = conductivity * (gsl_vector_get(A, nodeJ) - temp); // conductivity * delta temperature
            
            if ( abs(nDiff.x) > DBL_EPSILON ) { // if distance is to small we avoid dividing by 0
                gsl_vector_set(xFluxC, nodeJ, gsl_vector_get(xFluxC, nodeJ) + 1); // counting node contributions
                gsl_vector_set(xFlux, nodeJ, gsl_vector_get(xFlux, nodeJ) + (kT / nDiff.x)); // Adding element x flux contribution to the node
            }
            
            if ( abs(nDiff.y) > DBL_EPSILON ) { // if distance is to small we avoid dividing by 0
                gsl_vector_set(yFluxC, nodeJ, gsl_vector_get(yFluxC, nodeJ) + 1); // counting node contributions
                gsl_vector_set(yFlux, nodeJ, gsl_vector_get(yFlux, nodeJ) + (kT / nDiff.y)); // Adding element y flux contribution to the node
            }
        }
    }
    
    gsl_vector_div(xFlux, xFluxC); // getting nodal x flux avg. value
    gsl_vector_div(yFlux, yFluxC); // getting nodal y flux avg. value

    // Printing flux if verbosity >= 3
    if (verbosityLevel >= 3) {
        cout << "Flux X: " << endl; TSGsl::gsl_show_vector(*xFlux); cout << endl;
        cout << "Flux Y: " << endl; TSGsl::gsl_show_vector(*yFlux); cout << endl;
    }
    
    
    /**
     * Generating GID post processing file
     **/
    fileName = (string)argv[1] + ".post.res";
    if (verbosityLevel >= 1) cout << "Saving result: " << fileName << endl;
    ofstream outFile;
    outFile.open(fileName.c_str());
    outFile << "GID Post Results File 1.0" << endl;
    outFile << endl;
//    outFile << "GaussPoints \"Elements\" ElemType Triangle" << endl;
//    outFile << "Number Of Gauss Points: 1" << endl;
//    outFile << "Natural Coordinates: internal" << endl;
//    outFile << "end gausspoints" << endl;
    outFile << endl;
    outFile << "Result \"Temperature\" \"LOAD ANALISYS\" 1 Scalar OnNodes" << endl;
    outFile << "Values" << endl;
    for (size_t i = 0; i < amountOfNodes; i++)
        outFile << i+1 << " " << gsl_vector_get(A, i) << endl;
    
    outFile << "End values" << endl;
    outFile << endl;
    
//    outFile << "Result \"Flux\" \"LOAD ANALISYS\" 1 Vector OnGaussPoints \"Elements\"" << endl;
    outFile << "Result \"Flux\" \"LOAD ANALISYS\" 1 Vector OnNodes" << endl;
    outFile << "Values" << endl;
    for (size_t i = 0; i < amountOfNodes; i++)
        outFile << i+1 << " " << gsl_vector_get(xFlux, i) << " " << gsl_vector_get(yFlux, i) << " 0" << endl;
    
    outFile << "End values" << endl;
    
    outFile.close();
    
    return 0;
}
