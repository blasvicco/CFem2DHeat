//
//  TInputParser.hpp
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

#ifndef TInputParser_hpp
#define TInputParser_hpp

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

#include "TSString.hpp"
#include "TTriangle.hpp"

struct SMaterial {
    long double conductivity;
    long double convectivity;
};

class TInputParser {
    private:
        static size_t status;
        static size_t currentFileSection;
        static size_t amountOfConditions;
        static size_t amountOfNodes;
        static size_t amountOfElements;
        static size_t amountOfMaterials;
        static size_t factor;
        static std::ifstream inFile;
        static std::string unit;
        static std::map<size_t, SCondition> Conditions;
        static std::map<size_t, SNode> Coordinates;
        static std::map<size_t, TElement*> Connectivities;
        static std::map<size_t, SMaterial> Materials;
        static std::map<std::string, unsigned int> fileSections;
    
        static void updateCurrentFileSection(std::string line);
        static void parseUnit();
        static void parseAmounts();
        static void parseCondition();
        static void parseNode();
        static void parseElement();
        static void parseMaterial();
    
    public:
        static const unsigned int FAIL = 0;
        static const unsigned int SUCCESS = 1;
    
        static const unsigned int FILE_BEGINNING = 0;
        static const unsigned int FILE_UNIT = 10;
        static const unsigned int FILE_AMOUNTS = 20;
        static const unsigned int FILE_COORDINATES = 30;
        static const unsigned int FILE_CONNECTIVITIES = 40;
        static const unsigned int FILE_MATERIALS = 50;
        static const unsigned int FILE_CONDITIONS = 60;
        static const unsigned int FILE_END = 70;
        static const unsigned int FILE_NO_RELEVANT = 80;
    
        TInputParser();
        virtual ~TInputParser();
    
        static void readFile(std::string fileName);
        static size_t getStatus();
        static size_t getUnit();
        static size_t getAmountOfConditions();
        static size_t getAmountOfNodes();
        static size_t getAmountOfElements();
        static size_t getAmountOfMaterials();
        static size_t getFactor();
        static std::map<size_t, SCondition> getConditions();
        static std::map<size_t, SNode> getCoordinates();
        static std::map<size_t, TElement*> getConnectivities();
        static std::map<size_t, SMaterial> getMaterials();
    
        static void printConditions();
        static void printCoordinates();
        static void printConnectivities();
        static void printMaterials();
};

#endif /* TInputParser_hpp */
