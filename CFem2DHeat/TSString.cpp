//
//  TSString.cpp
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

#include "TSString.hpp"

using namespace std;

string TSString::ftos(long double value) {
    string s = to_string(floor(value * 100) / 100);
    vector<string> vs = split(s, ".");
    vs[1].erase(vs[1].find_last_not_of('0') + 1, std::string::npos);
    return (value > 0 ? "+" : (value == 0 ? " " : "")) + vs[0] + "." + (vs[1].empty() ? "00" : vs[1]);
}

vector<string> TSString::split(string exp, string token) {
    vector<string> result;
    string::size_type lastPos = exp.find_first_not_of(token, 0); // Skip delimiters at beginning.
    string::size_type pos     = exp.find_first_of(token, lastPos); // Find first "non-delimiter".
    
    while (string::npos != pos || string::npos != lastPos) {
        result.push_back(exp.substr(lastPos, pos - lastPos)); // Found a token, add it to the vector.
        lastPos = exp.find_first_not_of(token, pos); // Skip delimiters.  Note the "not_of"
        pos     = exp.find_first_of(token, lastPos); // Find next "non-delimiter"
    }
    return result;
}
