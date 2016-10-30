//----------------------------------------------------------------------
//  A TIFF for 1-bit ver.1.0.0  Time-stamp: <2016-10-31 00:11:50 kido>
//----------------------------------------------------------------------
#ifndef ATIFF1_INCLUDED
#define ATIFF1_INCLUDED

#include "atiff.h"

#define ATIFF_BYTE              unsigned char

//----------------------------------------------------------------------
//  ATIFF1 : TIFF manipulate class for 1-bit
//----------------------------------------------------------------------
class atiff1 : public atiff {
    typedef atiff parent;
protected:
    void initialize();

public:
    atiff1();
    atiff1(int width,
           int height);
 // ~atiff1();
    // コピー演算・ムーブ演算の自動生成のために宣言しない

    void setBit(int i, int j, ATIFF_BYTE value) {
        const int ibit  = i % 8;
        const int ibyte = i / 8;
        const ATIFF_BYTE unity = 128 >> ibit; //MSB2LSB
        if(value) { //on
            buf_[ibyte + j*nx_/8] |=  unity;
        } else { //off
            buf_[ibyte + j*nx_/8] &= ~unity;
        }
    }
    ATIFF_BYTE getBit(int i, int j) const {
        const int ibit  = i % 8;
        const int ibyte = i / 8;
        const ATIFF_BYTE unity = 128 >> ibit; //MSB2LSB
        return (buf_[ibyte + j*nx_/8] & unity) ? 255 : 0;
    }

    bool build(int width,
               int height);
};

#undef ATIFF_BYTE

#endif //ATIFF1_INCLUDED

//----------------------------------------------------------------------
//  End Of File
//----------------------------------------------------------------------
