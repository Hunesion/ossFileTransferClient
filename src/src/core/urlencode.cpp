/*
* Copyright (c) 2020 Hunesion Inc.
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; 
* either version 2.1 of the License, or (at your option) any later version.
* See the file COPYING included with this distribution for more information.
* https://github.com/HuneOpenUp/ossFileTransferClient/blob/master/COPYING
*/

#include "urlencode.h"
#include <cctype>
#include <iomanip>
#include <sstream>
#include <string>

using namespace std;


std::string url_encode(const std::string &value) {
    ostringstream escaped;
    escaped.fill('0');
    escaped << hex;

    for (std::string::const_iterator i = value.begin(), n = value.end(); i != n; ++i) {
        std::string::value_type c = (*i);

        // Keep alphanumeric and other accepted characters intact
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            escaped << c;
            continue;
        }

        // Any other characters are percent-encoded
        escaped << uppercase;
        escaped << '%' << setw(2) << int((unsigned char) c);
        escaped << nouppercase;
    }

    return escaped.str();
}

std::string url_decode(std::string &src) {
    std::string ret;
    char ch;
    int i, ii;
    for (i=0; i<src.length(); i++) {
        if (src[i] == '%') {
            sscanf(src.substr(i+1,2).c_str(), "%x", &ii);
            ch = static_cast<char>(ii);
            ret += ch;
            i += 2;
        } else {
            if(src[i] == '+') {
                ret += ' ';
            }
            else {
                ret += src[i];
            }
        }
    }
    return ret;
}
