//----------------------------------------------------------------------
//  A TIFF ver.1.0.1            Time-stamp: <2016-11-18 03:25:20 kido>
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

#include <tiff.h>
#include <tiffio.h>

#include "atiff.h"

inline void ERRMSG(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    char tmp[BUFSIZ];
    vsnprintf(tmp, sizeof(tmp), fmt, args);
    cerr << tmp << endl;
    va_end(args);
}

#define ATIFF_STR(label)        #label
#define ATIFF_TAG_CONCAT(label) TIFFTAG_ ## label
#define ATIFF_GET_SAFE(label, var)\
    if(TIFFGetField(tif, ATIFF_TAG_CONCAT(label), &var) != 1) {\
        ERRMSG("%s: cannot get %s\n",\
               filename.c_str(), ATIFF_STR(label));\
        return false;\
    }
#define ATIFF_SET_SAFE(label, var)\
    if(TIFFSetField(tif, ATIFF_TAG_CONCAT(label),  var) != 1) {\
        ERRMSG("%s: cannot set %s\n",\
               filename.c_str(), ATIFF_STR(label));    \
        return false;\
    }

//----------------------------------------------------------------------
//  ATIFF : TIFF manipulate class
//----------------------------------------------------------------------
atiff::atiff() {
    this->initialize();
}

atiff::atiff(int width, int height, int ncolors, int ordering) {
    this->build(width, height, ncolors, ordering);
}

atiff::atiff(string filename) {
    this->initialize();
    this->load(filename);
}

// atiff::~atiff() {
// }
// コピー演算・ムーブ演算の自動生成のために宣言しない

void atiff::initialize() {
    depth_        = ATIFF_8BIT_DEPTH;       // 256階調
 // depth_        = ATIFF_1BIT_DEPTH;       // 2階調(白黒2値)
    width_        = 0;                      // 横ピクセル数
    height_       = 0;                      // 縦ピクセル数
    ncolors_      = 1;                      // グレイスケール or 白黒2値
 // ncolors_      = 3;                      // RGBカラー
 // ncolors_      = 4;                      // RGBAカラー or CMYKカラー
 // photometric_  = PHOTOMETRIC_MINISWHITE; // 0が白、最大が黒
    photometric_  = PHOTOMETRIC_MINISBLACK; // 0が黒、最大が白
 // photometric_  = PHOTOMETRIC_RGB;        // RGB or RGBA
 // photometric_  = PHOTOMETRIC_SEPARATED;  // CMYK
 // compression_  = COMPRESSION_NONE;       // 無圧縮(ダンプモード)
    compression_  = COMPRESSION_LZW;        // Lempel-Ziv & Welch圧縮
    orientation_  = ORIENTATION_TOPLEFT;    // 左上から右下へ
    fillorder_    = FILLORDER_MSB2LSB;      // 上位ビット→下位ビット
 // fillorder_    = FILLORDER_LSB2MSB;      // 下位ビット→上位ビット
    planarmode_   = PLANARCONFIG_CONTIG;    // ピクセル毎に格納(plane-major)
 // planarmode_   = PLANARCONFIG_SEPARATE;  // プレーン毎に格納(pixel-major)
 // resounit_     = RESUNIT_NONE;           // 単位なし
    xreso_        = 96.0;                   // [DPI] 1280x1024@17inch
    yreso_        = 96.0;                   // [DPI] 1280x1024@17inch
 // resounit_     = RESUNIT_NONE;           // 単位なし
    resounit_     = RESUNIT_INCH;           // インチ
 // resounit_     = RESUNIT_CENTIMETER;     // センチメートル
    software_     = "atiff_powerdby_libtiff";
    rowsperstrip_ = 0x0fffffff;             // 1ストリップ当たりの行数、正しくは2^32-1

    nx_       = 0;
    ny_       = 0;
    nc_       = 1;
    ordering_ = ATIFF_ORDER_NONE;
}

bool atiff::build(int width, int height, int ncolors, int ordering) {
    this->initialize();
    width_   = (uint32)width;
    height_  = (uint32)height;
    ncolors_ = (uint16)ncolors;
    nx_      =         width;
    ny_      =         height;
    nc_      =         ncolors;
    if(nc_ == 1) {
        photometric_  = PHOTOMETRIC_MINISBLACK;
        planarmode_   = PLANARCONFIG_CONTIG;
        ordering_     =  ATIFF_ORDER_NONE;
    } else if(nc_ == 3||nc_ == 4) {
        photometric_  = PHOTOMETRIC_RGB;
        ordering_     = ordering;
        if       (ordering == ATIFF_PLANE_MAJOR) {
            planarmode_   = PLANARCONFIG_CONTIG;
        } else if(ordering == ATIFF_PIXEL_MAJOR) {
            planarmode_   = PLANARCONFIG_SEPARATE;
        } else {
            ERRMSG("%d: unsupported ordering", ordering);
            return false;
        }
    } else {
        ERRMSG("%d: unsupported ncolors", ncolors);
        return false;
    }
    if((int)buf_.size() != nx_*ny_*nc_) buf_.resize(nx_*ny_*nc_);
    return true;
}

bool atiff::load(string filename) {
    // 読み込みモードでTIFFファイルを開く
    TIFF* tif = TIFFOpen(filename.c_str(), "r");
    if(tif == NULL) {
        ERRMSG("%s: cannot open", filename.c_str());
        return false;
    }

    // 階調の深さ(ビット数)
    ATIFF_GET_SAFE(BITSPERSAMPLE,   depth_      );
    // カラーモード(色表現)
    ATIFF_GET_SAFE(PHOTOMETRIC,     photometric_);
    if((photometric_ != PHOTOMETRIC_MINISWHITE)&&
       (photometric_ != PHOTOMETRIC_MINISBLACK)&&
       (photometric_ != PHOTOMETRIC_RGB)&&
       (photometric_ != PHOTOMETRIC_SEPARATED)) {
        ERRMSG("%s: unsupported PHOTOMETRIC type '%u'",
               filename.c_str(), photometric_);
        return false;
    }
    // 画像の横幅(ピクセル数)
    ATIFF_GET_SAFE(IMAGEWIDTH,      width_      );
    nx_ = (int)width_;
    // 画像の高さ(ピクセル数)
    ATIFF_GET_SAFE(IMAGELENGTH,     height_     );
    ny_ = (int)height_;
    // 色数(サンプル数)
    ATIFF_GET_SAFE(SAMPLESPERPIXEL, ncolors_    );
    nc_ = (int)ncolors_;
    // 圧縮形式
    ATIFF_GET_SAFE(COMPRESSION,     compression_);
    if((compression_ != COMPRESSION_NONE)&&
       (compression_ != COMPRESSION_LZW)) {
        ERRMSG("%s: unsupported COMPRESSION type '%u'",
               filename.c_str(), compression_);
        return false;
    }
    // 画像原点の取得
 // ATIFF_GET_SAFE(ORIENTATION,     orientation_);
    if(orientation_ != ORIENTATION_TOPLEFT) {
        ERRMSG("%s: unsupported ORIENTATION type '%u'",
               filename.c_str(), orientation_);
        return false;
    }
    // ビットオーダ
 // ATIFF_GET_SAFE(FILLORDER, fillorder_);
    // 優先モード(データの並び順)
    ATIFF_GET_SAFE(PLANARCONFIG,    planarmode_ );
    if(nc_ == 1) {
        ordering_ =  ATIFF_ORDER_NONE;
    } else {
        if(planarmode_ == PLANARCONFIG_CONTIG) {
            ordering_ = ATIFF_PLANE_MAJOR;
        } else {
            ordering_ = ATIFF_PIXEL_MAJOR;
        }
    }
    // x方向解像度[DPI]
 // ATIFF_GET_SAFE(XRESOLUTION,     xreso_      );
 // y方向解像度[DPI]
 // ATIFF_GET_SAFE(YRESOLUTION,     yreso_      );
    // 解像度の単位
 // ATIFF_GET_SAFE(RESOLUTIONUNIT,  resounit_   );
    // 出力ソフトウェア名
 // char softname[BUFSIZ];
 // ATIFF_GET_SAFE(SOFTWARE,        softname    );
 // software_ = softname;

    // 1ストリップ当たりの行数
    if(TIFFGetField(tif, TIFFTAG_ROWSPERSTRIP, &rowsperstrip_) != 1) {
        rowsperstrip_ = 0x0fffffff; //正しくは2^32-1、圧縮書き込み時に省略される
    }
    int jstep = (int)rowsperstrip_;

    // 描画データの読み込み
    if((int)buf_.size() != nx_*ny_*nc_) buf_.resize(nx_*ny_*nc_);
    if(ordering_ < 0) { //pixel-major
        if(depth_ == ATIFF_1BIT_DEPTH) {
            ERRMSG("%s: 1-bit depth is invalid in pixel-major mode",
                   filename.c_str());
            return false;
        }
        for(int c = 0; c < nc_; ++c) {
            for(int j = 0; j < ny_; j += jstep) {
                int nrow = (j + jstep > ny_) ? ny_ - j : jstep;
                tstrip_t   pos = TIFFComputeStrip(tif, j, c);
                tdata_t    ptr = (tdata_t)(&buf_[index(0, j, c)]);
                tsize_t length = (tsize_t)index(0, nrow, 0);
                if(TIFFReadEncodedStrip(tif, pos, ptr, length) == -1) {
                    ERRMSG("%s: data read error", filename.c_str());
                    return false;
                }
            }
        }
    } else { //plane-major
        if(depth_ == ATIFF_1BIT_DEPTH) {
            const int nx8 = (nx_ + 7) / 8;
            vector<uint8> tmp(nx8*ny_*nc_);
            for(int j = 0; j < ny_; j += jstep) {
                int nrow = (j + jstep > ny_) ? ny_ - j : jstep;
                tstrip_t   pos = TIFFComputeStrip(tif, j, 0);
                tdata_t    ptr = (tdata_t)(&tmp[nx8*j]);
                tsize_t length = (tsize_t)(nx8*nrow);
                if(TIFFReadEncodedStrip(tif, pos, ptr, length) == -1) {
                    ERRMSG("%s: data read error", filename.c_str());
                    return false;
                }
            }
            for(int j = 0; j < ny_; ++j) {
            for(int i = 0; i < nx_; ++i) {
                const uint8 unity = 0x80 >> (i % 8); //MSB2LSB
                buf_[i + j*nx_] = (tmp[i/8 + j*nx8] & unity) ? 0xFF : 0;
            }
            }
        } else {
            for(int j = 0; j < ny_; j += jstep) {
                int nrow = (j + jstep > ny_) ? ny_ - j : jstep;
                tstrip_t   pos = TIFFComputeStrip(tif, j, 0);
                tdata_t    ptr = (tdata_t)(&buf_[index(0, j, 0)]);
                tsize_t length = (tsize_t)index(0, nrow, 0);
                if(TIFFReadEncodedStrip(tif, pos, ptr, length) == -1) {
                    ERRMSG("%s: data read error", filename.c_str());
                    return false;
                }
            }
        }
    }

    TIFFClose(tif);
    return true;
}

bool atiff::save(string filename, int depth, int compression) const {
    if(depth       == 0) depth       = depth_;
    if(compression == 0) compression = compression_;
    // 書き込みモードでTIFFファイルを開く
    TIFF* tif = TIFFOpen(filename.c_str(), "w");
    if(tif == NULL) {
        ERRMSG("%s: cannot open", filename.c_str());
        return false;
    }

    // 階調の深さ(ビット数)
    ATIFF_SET_SAFE(BITSPERSAMPLE,   depth       );
    // カラーモード(色表現)
    ATIFF_SET_SAFE(PHOTOMETRIC,     photometric_);
    // 画像の横幅(ピクセル数)
    ATIFF_SET_SAFE(IMAGEWIDTH,      width_      );
    // 画像の高さ(ピクセル数)
    ATIFF_SET_SAFE(IMAGELENGTH,     height_     );
    // 色数(サンプル数)
    ATIFF_SET_SAFE(SAMPLESPERPIXEL, ncolors_    );
    // 圧縮形式
    ATIFF_SET_SAFE(COMPRESSION,     compression );
    // 画像の原点
    ATIFF_SET_SAFE(ORIENTATION,     orientation_);
    // ビットオーダ
 // ATIFF_SET_SAFE(FILLORDER,       fillorder_  );
    // 優先モード(データの並び順)
    ATIFF_SET_SAFE(PLANARCONFIG,    planarmode_ );
    // x方向解像度[DPI]
    ATIFF_SET_SAFE(XRESOLUTION,     xreso_      );
    // y方向解像度[DPI]
    ATIFF_SET_SAFE(YRESOLUTION,     yreso_      );
    // 解像度の単位
    ATIFF_SET_SAFE(RESOLUTIONUNIT,  resounit_   );
    // 出力ソフトウェア名
    ATIFF_SET_SAFE(SOFTWARE,   software_.c_str());

    // 描画データの書き込み
    if(ordering_ < 0) { //pixel-major
        if(depth == ATIFF_1BIT_DEPTH) {
            ERRMSG("%s: 1-bit depth is invalid in pixel-major mode",
                   filename.c_str());
            return false;
        }
        for(int c = 0; c < nc_; ++c) {
            tdata_t    ptr = (tdata_t)(&buf_[index(0, 0, c)]);
            tsize_t length = (tsize_t)index(0, ny_, 0);
            if(TIFFWriteEncodedStrip(tif, c, ptr, length) == -1) {
                ERRMSG("%s: data write error", filename.c_str());
                return false;
            }
        }
    } else { //plane-major
        if(depth == ATIFF_1BIT_DEPTH) {
            const int nx8 = (nx_ + 7) / 8;
            vector<uint8> tmp(nx8*ny_*nc_, 0);
            for(int j = 0; j < ny_; ++j) {
            for(int i = 0; i < nx_; ++i) {
                if(buf_[i + j*nx_] == 0) continue;
                const uint8 unity = 0x80 >> (i % 8); //MSB2LSB
                tmp[i/8 + j*nx8] |= unity;
            }
            }
            tdata_t    ptr = (tdata_t)(&tmp[0]);
            tsize_t length = (tsize_t)tmp.size();
            if(TIFFWriteEncodedStrip(tif, 0, ptr, length) == -1) {
                ERRMSG("%s: data write error", filename.c_str());
                return false;
            }
        } else {
            tdata_t    ptr = (tdata_t)(&buf_[0]);
            tsize_t length = (tsize_t)buf_.size();
            if(TIFFWriteEncodedStrip(tif, 0, ptr, length) == -1) {
                ERRMSG("%s: data write error", filename.c_str());
                return false;
            }
        }
    }

    TIFFClose(tif);
    return true;
}

//----------------------------------------------------------------------
//  End Of File
//----------------------------------------------------------------------
