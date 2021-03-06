//
// Created by Alcan on 2018/5/12.
//

/***************************************************************************
 *
 * Author: "Sjors H.W. Scheres"
 * MRC Laboratory of Molecular Biology
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * This complete copyright notice must be included in any revised version of the
 * source code. Additional authorship citations may be added, but existing
 * author citations must be preserved.
 ***************************************************************************/
/***************************************************************************
 *
 * Authors:     J.R. Bilbao-Castro (jrbcast@ace.ual.es)
 *
 * Unidad de  Bioinformatica of Centro Nacional de Biotecnologia , CSIC
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307  USA
 *
 *  All comments concerning this program package may be sent to the
 *  e-mail address 'xmipp@cnb.csic.es'
 ***************************************************************************/

#include <cmath>

#include "strings.h"
#include "error.h"
#include "macros.h"
#include "gcc_version.h"

std::string removeChar(const std::string &str, char character) {
    std::string temp;

    for (unsigned int i = 0; i < str.length(); i++) {
        if (str[i] != character)
            temp += str[i];
    }

    return temp;
}

std::string unescape(const std::string &str) {
    std::string temp;

    for (unsigned int i = 0; i < str.length(); i++) {
        char current_char = str[i];

        if (current_char != '\n' && current_char != '\t' &&
            current_char != '\v' && current_char != '\b' &&
            current_char != '\r' && current_char != '\f' &&
            current_char != '\a') {
            temp += str[i];
        } else if (current_char == '\t') {
            temp += ' ';
        }
    }

    return temp;
}

std::string simplify(const std::string &str) {
    std::string temp;

    // First, unescape string
    std::string straux = unescape(str);

    // Remove spaces from the beginning
    int pos = straux.find_first_not_of(' ');
    straux.erase(0, pos);

    // Trim the rest of spaces
    for (unsigned int i = 0; i < straux.length();) {
        temp += straux[i];

        if (straux[i] == ' ') {
            while (straux[i] == ' ') {
                i++;
            }
        } else {
            i++;
        }
    }

    // Remove space left at the end of the string
    // if needed
    if (temp[temp.size() - 1] == ' ') {
        temp.resize(temp.size() - 1);
    }

    return temp;
}

/** Trim all spaces from the begining and the end */
void trim(std::string &str) {
    std::string::size_type pos = str.find_last_not_of(' ');

    if (pos != std::string::npos) {
        str.erase(pos + 1);
        pos = str.find_first_not_of(' ');
        if (pos != std::string::npos)
            str.erase(0, pos);
    } else
        str.clear();
}

/* NOTE: not a very safe implemenation but standard c functions do not retrieve
 * more than 6 significative digits */
double textToDouble(const char *str, int _errno, std::string errmsg) {
    double ret_val;
    int ok;

    if (str == NULL)
        REPORT_ERROR(errmsg);

#ifdef RELION_SINGLE_PRECISION
    ok = sscanf(str, "%f", &retval);
#else
    ok = sscanf(str, "%lf", &ret_val);
#endif

    if (ok)
        return ret_val;

    REPORT_ERROR(errmsg);

    return 0;
}

float textToFloat(const char *str, int _errno, std::string errmsg) {
    float ret_val;
    int ok;

    if (str == NULL)
        REPORT_ERROR(errmsg);

    ok = sscanf(str, "%f", &ret_val);

    if (ok)
        return ret_val;

    REPORT_ERROR(errmsg);

    return 0;
}

int textToInteger(const char *str, int _errno, std::string errmsg) {
    int ret_val;
    int ok;

    if (str == NULL)
        REPORT_ERROR(errmsg);

    ok = sscanf(str, "%d", &ret_val);

    if (ok)
        return ret_val;

    REPORT_ERROR(errmsg);

    return 0;
}

long long textToLongLong(const char *str, int _errno, std::string errmsg) {
    long long int ret_val;
    int ok;

    if (str == NULL)
        REPORT_ERROR(errmsg);

    ok = sscanf(str, "%lld", &ret_val);

    if (ok)
        return ret_val;

    REPORT_ERROR(errmsg);

    return 0;
}

int bestPrecision(float F, int _width) {
    // If it is 0
    if (F == 0)
        return 1;

    // Otherwise
    int exp = FLOOR(log10(ABS(F)));
    int advised_prec;

    if (exp >= 0)
        if (exp > _width - 3)
            advised_prec = -1;
        else
            advised_prec = _width - 2;
    else {
        advised_prec = _width + (exp - 1) - 3;
        if (advised_prec <= 0)
            advised_prec = -1;
    }

    if (advised_prec < 0)
        advised_prec = -1; // Choose exponential format

    return advised_prec;
}

std::string floatToString(float F, int _width, int _prec) {
#if GCC_VERSION < 30300
    char aux[15];
    std::ostrstream outs(aux, sizeof(aux));
#else
    std::ostringstream outs;
#endif

    outs.fill(' ');

    if (_width != 0)
        outs.width(_width);

    if (_prec == 0)
        _prec = bestPrecision(F, _width);

    if (_prec == -1 && _width > 7) {
        outs.precision(_width - 7);
        outs.setf(std::ios::scientific);
    } else
        outs.precision(_prec);

#if GCC_VERSION < 30301
    outs << F << std::ends;
#else
    outs << F;
#endif

#if GCC_VERSION < 30300
    return std::string(aux);
#else
    std::string ret_val = outs.str();
    int i = ret_val.find('\0');

    if (i != -1)
        ret_val = ret_val.substr(0, i);

    return ret_val;
#endif
}

std::string integerToString(int I, int _width, char fill_with) {
    char aux[15];

    // Check width
    int width = _width;
    int Iaux = ABS(I);

    if (SGN(I) < 0)
        width--;

    if (width == 0)
        do {
            Iaux /= 10;
            width++;
        } while (Iaux != 0);

    // Fill the number with the fill character
    for (int i = 0; i < width; i++)
        aux[i] = fill_with;

    // Start filling the array
    aux[width--] = '\0';
    Iaux = ABS(I);
    do {
        int digit = Iaux % 10;
        Iaux /= 10;
        aux[width--] = '0' + digit;
    } while (Iaux != 0);

    if (SGN(I) < 0)
        return static_cast< std::string >("-") + aux;
    else
        return static_cast< std::string >(aux);
}

int textToInt(const char *str, int _errno, std::string errmsg) {
    char read_val;
    int ok;

    if (str == NULL)
        REPORT_ERROR(errmsg);

    ok = sscanf(str, "%c", &read_val);

    if (ok)
        return read_val - 48;

    REPORT_ERROR(errmsg);

    return 0;
}

std::string stringToString(const std::string &str, int _width) {
    if (_width == 0)
        return str;

    if (_width < str.length())
        return str.substr(0, _width);

    std::string aux = str;
    return aux.append(_width - str.length(), ' ');
}

std::string removeSpaces(const std::string &_str) {
    std::string ret_val;
    int first = _str.find_first_not_of("\n \t");
    int last = _str.find_last_not_of("\n \t");
    bool after_blank = false;

    for (int i = first; i <= last; i++) {
        if (_str[i] == ' ' || _str[i] == '\n' || _str[i] == '\t') {
            if (!after_blank)
                ret_val += _str[i];

            after_blank = true;
        } else {
            ret_val += _str[i];
            after_blank = false;
        }
    }

    return ret_val;
}

// Remove quotes ===========================================================
void removeQuotes(char **_str) {
    std::string ret_val = *_str;
    if (ret_val.length() == 0)
        return;
    char c = ret_val[0];
    if (c == '\"' || c == '\'')
        ret_val = ret_val.substr(1, ret_val.length() - 1);
    c = ret_val[ret_val.length() - 1];
    if (c == '\"' || c == '\'')
        ret_val = ret_val.substr(0, ret_val.length() - 1);
    free(*_str);
    *_str = strdup(ret_val.c_str());
}

// Split a string ==========================================================
int splitString(const std::string &input,
                const std::string &delimiter,
                std::vector<std::string> &results,
                bool include_empties) {
    results.clear();
    int i_pos = 0;
    int new_pos = -1;
    int size_s2 = static_cast< int >(delimiter.size());
    int i_size = static_cast< int >(input.size());

    if (i_size == 0 || size_s2 == 0)
        return 0;

    std::vector<int> positions;
    new_pos = input.find(delimiter, 0);

    if (new_pos < 0)
        return 0;

    int num_found = 0;
    while (new_pos >= i_pos) {
        num_found++;
        positions.push_back(new_pos);
        i_pos = new_pos;
        new_pos = input.find(delimiter, i_pos + size_s2);
    }

    if (num_found == 0)
        return 0;

    for (int i = 0; i <= static_cast< int >(positions.size()); i++) {
        std::string s("");
        if (i == 0)
            s = input.substr(i, positions[i]);
        int offset = positions[i - 1] + size_s2;
        if (offset < i_size) {
            if (i == positions.size())
                s = input.substr(offset);
            else if (i > 0)
                s = input.substr(positions[i - 1] + size_s2,
                                 positions[i] - positions[i - 1] - size_s2);
        }
        if (include_empties || s.size() > 0)
            results.push_back(s);
    }
    return num_found;
}

// To lower ================================================================
void toLower(char *_str) {
    int i = 0;
    while (_str[i] != '\0') {
        if (_str[i] >= 'A' && _str[i] <= 'Z')
            _str[i] += 'a' - 'A';
        i++;
    }
}

void toLower(std::string &_str) {
    int i = 0;
    while (_str[i] != '\0') {
        if (_str[i] >= 'A' && _str[i] <= 'Z')
            _str[i] += 'a' - 'A';
        i++;
    }
}

// Next token ==============================================================
std::string nextToken(const std::string &str, int &i) {
    std::string ret_val;
    if (i >= str.length())
        return ret_val;
    int j = str.find_first_not_of(" \t\n", i);
    if (j == -1)
        return ret_val;
    int k = str.find_first_of(" \t\n", j + 1);
    if (k == -1)
        k = str.length();
    ret_val = str.substr(j, k - j + 1);
    i = k + 1;
    return ret_val;
}

// Get word ================================================================
char *firstWord(char *str, int _errno, const std::string &errmsg) {
    char *token;

    // Get token
    if (str != NULL)
        token = firstToken(str);
    else
        token = nextToken();

    // Check that there is something
    if (token == NULL)
        REPORT_ERROR(errmsg);

    return token;
}

// Tokenize a C++ string ===================================================
void tokenize(const std::string &str, std::vector<std::string> &tokens,
              const std::string &delimiters) {
    tokens.clear();
    // Skip delimiters at beginning.
    std::string::size_type last_pos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    std::string::size_type pos = str.find_first_of(delimiters, last_pos);

    while (std::string::npos != pos || std::string::npos != last_pos) {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(last_pos, pos - last_pos));
        // Skip delimiters.  Note the "not_of"
        last_pos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, last_pos);
    }
}
