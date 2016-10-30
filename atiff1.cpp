//----------------------------------------------------------------------
//  A TIFF for 1-bit ver.1.0.0  Time-stamp: <2016-10-31 00:11:31 kido>
//----------------------------------------------------------------------
#include <cstdio>
#include <cstdarg>
#include <iostream>
using namespace std;

#include <tiff.h>
#include <tiffio.h>

#include "atiff1.h"

inline void OUTMSG(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    char tmp[BUFSIZ];
    vsnprintf(tmp, sizeof(tmp), fmt, args);
    cout << tmp << endl;
    va_end(args);
}

inline void ERRMSG(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    char tmp[BUFSIZ];
    vsnprintf(tmp, sizeof(tmp), fmt, args);
    cerr << tmp << endl;
    va_end(args);
}

//----------------------------------------------------------------------
//  ATIFF : TIFF manipulate class for 1-bit
//----------------------------------------------------------------------
atiff1::atiff1() {
    this->initialize();
}

atiff1::atiff1(int width, int height) {
    this->build(width, height);
}

// atiff1::~atiff1() {
// }
// コピー演算・ムーブ演算の自動生成のために宣言しない

void atiff1::initialize() {
    parent::initialize();
    depth_        = ATIFF_2LEVEL;           // 2階調(白黒2値)
    software_     = "atiff1_powerdby_libtiff";
}

bool atiff1::build(int width, int height) {
    if(width % 8 != 0) {
        ERRMSG("%d: width must be multiples of 8", width);
        return false;
    }
    this->initialize();

    width_   = (uint32)width;
    height_  = (uint32)height;
    ncolors_ = ATIFF_MONO_COLOR;
    depth_   = ATIFF_2LEVEL;
    nx_      =         width;
    ny_      =         height;
    nc_      = ATIFF_MONO_COLOR;

    photometric_  = PHOTOMETRIC_MINISWHITE;
    planarmode_   = PLANARCONFIG_CONTIG;
    ordering_     =  0; //single plane
    if((int)buf_.size() != nx_*ny_*nc_/8) buf_.resize(nx_*ny_*nc_/8, 0);
}

//----------------------------------------------------------------------
//  End Of File
//----------------------------------------------------------------------
