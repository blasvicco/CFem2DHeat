//
//  TInputParser.cpp
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

#include "TInputParser.hpp"

using namespace std;

ifstream TInputParser::inFile;
size_t TInputParser::status;
size_t TInputParser::currentFileSection;
size_t TInputParser::amountOfMaterials;
size_t TInputParser::amountOfConditions;
size_t TInputParser::amountOfNodes;
size_t TInputParser::amountOfElements;
size_t TInputParser::factor;
string TInputParser::unit;
map<size_t, SMaterial> TInputParser::Materials;
map<size_t, SCondition> TInputParser::Conditions;
map<size_t, SNode> TInputParser::Coordinates;
map<size_t, TElement*> TInputParser::Connectivities;
map<string, unsigned int> TInputParser::fileSections;

TInputParser::TInputParser() { }

TInputParser::~TInputParser() { }

void TInputParser::readFile(string fileName) {
    string line;
    inFile.open(fileName.c_str());
    currentFileSection = TInputParser::FILE_BEGINNING;
    status = TInputParser::FAIL;
    if (inFile.is_open()) {
        fileSections["Geometry Unity:"]             = TInputParser::FILE_UNIT;
        fileSections["Number of Elements & Nodes:"] = TInputParser::FILE_AMOUNTS;
        fileSections["Point conditions"]            = TInputParser::FILE_CONDITIONS;
        fileSections["Line conditions"]             = TInputParser::FILE_CONDITIONS;
        fileSections["Coordinates:"]                = TInputParser::FILE_COORDINATES;
        fileSections["Connectivities:"]             = TInputParser::FILE_CONNECTIVITIES;
        fileSections["Begin Materials"]             = TInputParser::FILE_MATERIALS;
        while (!inFile.eof()) {
            getline(inFile, line);
            updateCurrentFileSection(line);
            switch (currentFileSection) {
                case TInputParser::FILE_UNIT:            parseUnit();      break;
                case TInputParser::FILE_AMOUNTS:         parseAmounts();   break;
                case TInputParser::FILE_MATERIALS:       parseMaterial();  break;
                case TInputParser::FILE_CONDITIONS:      parseCondition(); break;
                case TInputParser::FILE_COORDINATES:     parseNode();      break;
                case TInputParser::FILE_CONNECTIVITIES:  parseElement();   break;
                default: break;
            }
        }
        inFile.close();
        amountOfConditions = Conditions.size();
        status = TInputParser::SUCCESS;
    }
    currentFileSection = TInputParser::FILE_END;
}

void TInputParser::updateCurrentFileSection(string line) {
    currentFileSection = (fileSections.find(line) != fileSections.end())
        ? fileSections.at(line)
        : currentFileSection;
}

void TInputParser::parseUnit() {
    getline(inFile, unit);
    factor = 1;
    if (unit == "DM") factor = 10;
    if (unit == "CM") factor = 100;
    if (unit == "MM") factor = 1000;
    currentFileSection = TInputParser::FILE_NO_RELEVANT;
}

void TInputParser::parseAmounts() {
    string line;
    getline(inFile, line);
    vector<string> tmp  = TSString::split(line, " ");
    amountOfElements    = atoi(tmp[0].c_str());
    amountOfNodes       = atoi(tmp[1].c_str());
    currentFileSection  = TInputParser::FILE_NO_RELEVANT;
}

void TInputParser::parseMaterial() {
    string line;
    getline(inFile, line);
    vector<string> tmp = TSString::split(line, "=");
    amountOfMaterials = atoi(tmp[1].c_str());
    getline(inFile, line);
    for (size_t i = 0; i < amountOfMaterials; i++) {
        getline(inFile, line);
        vector<string> tmp = TSString::split(line, " ");
        SMaterial OMaterial;
        OMaterial.conductivity = atof(tmp[1].c_str()) / factor;
        OMaterial.convectivity = atof(tmp[2].c_str()) / (factor * factor);
        Materials[atoi(tmp[0].c_str())] = OMaterial;
    }
    currentFileSection = TInputParser::FILE_NO_RELEVANT;
}

void TInputParser::parseCondition() {
    string line;
    string type = "unset";
    getline(inFile, line);
    size_t amount = atoi(line.c_str());
    getline(inFile, type);
    getline(inFile, line);
    for (size_t i = 0; i < amount; i++) {
        getline(inFile, line);
        vector<string> tmp = TSString::split(line, " ");
        SCondition OCondition;
        OCondition.type         = type;
        OCondition.temperature  = 0;
        OCondition.flux         = 0;
        OCondition.ambient      = 0;
        if (type == "Temperature") {
            OCondition.temperature  = atof(tmp[1].c_str());
        } else if (type == "Flux") {
            OCondition.flux         = atof(tmp[1].c_str()) * factor;
        } else if (type == "Convection") {
            OCondition.ambient      = atof(tmp[1].c_str());
        }

        size_t nodeId = atoi(tmp[0].c_str());
        if (Conditions.find(nodeId) != Conditions.end() && Conditions[nodeId].type == OCondition.type) {
            OCondition.temperature  = (Conditions[nodeId].temperature + OCondition.temperature) / 2;
            OCondition.flux         = (Conditions[nodeId].flux + OCondition.flux) / 2;
            OCondition.ambient      = (Conditions[nodeId].ambient + OCondition.ambient) / 2;
        }
        Conditions[nodeId] = OCondition;
    }
    currentFileSection = TInputParser::FILE_NO_RELEVANT;
}

void TInputParser::parseNode() {
    string line;
    getline(inFile, line);
    for (size_t i = 0; i < amountOfNodes; i++) {
        getline(inFile, line);
        vector<string> tmp = TSString::split(line, " ");
        SNode ONode;
        ONode.x = atof(tmp[1].c_str());
        ONode.y = atof(tmp[2].c_str());
        Coordinates[atoi(tmp[0].c_str())] = ONode;
    }
    currentFileSection = TInputParser::FILE_NO_RELEVANT;
}

void TInputParser::parseElement() {
    string line;
    getline(inFile, line);
    SCondition OCondition; OCondition.type = "unset";
    for (size_t i = 0; i < amountOfElements; i++) {
        getline(inFile, line);
        vector<string> tmp = TSString::split(line, " ");
        if (tmp.size() > 5) throw runtime_error("Error: Sorry, It supports triangle elements only for now. Sorry :)");
        TTriangle *OElement = new TTriangle();
        map<size_t, SNode> elmNodes;
        map<size_t, SCondition> elmConditions;
        for (unsigned int j = 1; j < 4; j++) {
            size_t nodeId           = atoi(tmp[j].c_str());
            elmNodes[nodeId]        = Coordinates[nodeId];
            elmConditions[nodeId]   = (Conditions.find(nodeId) != Conditions.end()) ? Conditions[nodeId] : OCondition;
        }
        OElement->ini(elmNodes, elmConditions, atoi(tmp[4].c_str()));
        Connectivities[atoi(tmp[0].c_str())] = OElement;
    }
    currentFileSection = TInputParser::FILE_NO_RELEVANT;
}

size_t TInputParser::getStatus() {
    return status;
}

size_t TInputParser::getAmountOfMaterials() {
    return amountOfMaterials;
}

size_t TInputParser::getAmountOfConditions() {
    return amountOfConditions;
}

size_t TInputParser::getAmountOfNodes() {
    return amountOfNodes;
}

size_t TInputParser::getAmountOfElements() {
    return amountOfElements;
}

map<size_t, SMaterial> TInputParser::getMaterials() {
    return Materials;
}

map<size_t, SCondition> TInputParser::getConditions() {
    return Conditions;
}

map<size_t, SNode> TInputParser::getCoordinates() {
    return Coordinates;
}

map<size_t, TElement*> TInputParser::getConnectivities() {
    return Connectivities;
}

size_t TInputParser::getFactor() {
    return factor;
}

void TInputParser::printMaterials() {
    map<size_t, SMaterial>::iterator it;
    for (it = Materials.begin(); it != Materials.end(); it++) {
        cout << "Material: Id " << it->first;
        cout << " Conductivity " << it->second.conductivity;
        cout << " Convectivity " << it->second.convectivity << endl;
    }
}

void TInputParser::printConditions() {
    map<size_t, SCondition>::iterator it;
    for (it = Conditions.begin(); it != Conditions.end(); it++) {
        cout << "Condition: Node " << it->first;
        if (it->second.type == "Temperature") cout << " Temperature " << it->second.temperature << endl;
        if (it->second.type == "Flux") cout << " Flux " << it->second.flux << endl;
        if (it->second.type == "Convection") cout << " Convection Ambient Temperature " << it->second.ambient << endl;
    }
}

void TInputParser::printCoordinates() {
    map<size_t, SNode>::iterator it;
    for (it = Coordinates.begin(); it != Coordinates.end(); it++) {
        cout << "Node: Id " << it->first << " Coords(" << it->second.x << ", " << it->second.y << ")" << endl;
    }
}

void TInputParser::printConnectivities() {
    map<size_t, TElement*>::iterator it;
    map<size_t, SNode>::iterator itn;
    for (it = Connectivities.begin(); it != Connectivities.end(); it++) {
        cout << "Element: Id " << it->first << " Nodes [ ";
        for (itn = it->second->nodes.begin(); itn != it->second->nodes.end(); itn++) cout << itn->first << " ";
        cout << "] Material " << it->second->getMaterialId() << endl;
    }
}

