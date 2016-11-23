//----------------------------------------------------------------------
//  A Misc ver.0.1.0            Time-stamp: <2016-11-24 05:09:04 kido>
//
//      Copyright (c) 2016 Akira KIDO
//      https://github.com/kido-akira/atools
//      This software is released under the MIT License.
//      http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------
#ifndef AMISC_INCLUDED
#define AMISC_INCLUDED

#include <cstdarg>
#include <fstream>
#include <string>
#include <vector>

namespace atools {

//----------------------------------------------------------------------
//  AMisc : Miscellaneous
//----------------------------------------------------------------------
#define ERRLOG(fmt, ...) errlog(__FILE__, __LINE__, fmt, __VA_ARGS__)
#define ERRMSG(fmt, ...) errfmt(                    fmt, __VA_ARGS__)

void        errlog(const char* file, int line,
                   const char* fmt, ...);
void        errfmt(const char* fmt, ...);
void        outfmt(const char* fmt, ...);
void        ofsfmt(std::ofstream& ofs,
                   const char* fmt, ...);
std::string strfmt(const char* fmt, ...);

int tokenizer(const std::string& text,
              std::vector<std::string>& token,
              const char delimiter = '\t');

void getlinetrim(std::ifstream& ifs, std::string& buf);

} //namespace atools

#endif //AMISC_INCLUDED

//----------------------------------------------------------------------
//  End Of File
//----------------------------------------------------------------------
	
