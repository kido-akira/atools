//----------------------------------------------------------------------
//  A Misc ver.0.1.0            Time-stamp: <2016-11-24 05:06:56 kido>
//
//      Copyright (c) 2016 Akira KIDO
//      https://github.com/kido-akira/atools
//      This software is released under the MIT License.
//      http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------
#include <cstdio>
#include <cstdarg>
#include <iostream>
using namespace std;

#include "amisc.h"

namespace atools {

//----------------------------------------------------------------------
//  AMisc : Miscellaneous
//----------------------------------------------------------------------
void errlog(const char* file, int line,
            const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    char pos[BUFSIZ];
     snprintf(pos, sizeof(pos), "%s:%d ", file, line);
    char tmp[BUFSIZ];
    vsnprintf(tmp, sizeof(tmp), fmt, args);
    cerr << pos << tmp << endl;
    va_end(args);
}

void errfmt(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    char tmp[BUFSIZ];
    vsnprintf(tmp, sizeof(tmp), fmt, args);
    cerr << tmp << endl;
    va_end(args);
}

void outfmt(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    char tmp[BUFSIZ];
    vsnprintf(tmp, sizeof(tmp), fmt, args);
    cout << tmp << endl;
    cout.flush();
    va_end(args);
}

void ofsfmt(ofstream& ofs,
            const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    char tmp[BUFSIZ];
    vsnprintf(tmp, sizeof(tmp), fmt, args);
    ofs << tmp << endl;
    ofs.flush();
    va_end(args);
}

string strfmt(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    char tmp[BUFSIZ];
    vsnprintf(tmp, sizeof(tmp), fmt, args);
    va_end(args);
    return tmp;
}

int tokenizer(const std::string& text,
              std::vector<std::string>& token,
              const char delimiter) {
   const int imax = (int)text.size();
   int ipos  = 0;
   int istep = 1;
   for(int i = 0; i < imax; i += istep) {
       unsigned char a = text[i];
       if        (a < 0x80) {
           istep = 1;
           if(a == (unsigned char)delimiter) {
               token.push_back(text.substr(ipos, i - ipos));
               ipos = i + 1;
           }
       } else if (a < 0xE0) {
           istep = 2;
       } else if (a < 0xF0) {
           istep = 3;
       } else {
           istep = 4;
       }
   }
   if(ipos < imax) {
       token.push_back(text.substr(ipos, imax - ipos));
   }
   return (int)token.size();
}

void getlinetrim(ifstream& ifs, string& buf) {
    std::getline(ifs, buf);
    if(buf[buf.length() - 1] == '\n') buf.erase(buf.end() - 1);
    if(buf[buf.length() - 1] == '\r') buf.erase(buf.end() - 1);
}

} //namespace atools

//----------------------------------------------------------------------
//  End Of File
//----------------------------------------------------------------------
