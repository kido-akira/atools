//----------------------------------------------------------------------
//  A TIFF ver.1.0.0            Time-stamp: <2016-10-31 00:10:59 kido>
//----------------------------------------------------------------------
#include <cstdio>
#include <cstdarg>
#include <iostream>
using namespace std;

#include <tiff.h>
#include <tiffio.h>

#include "atiff.h"

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

#define ATIFF_STR(label)        #label
#define ATIFF_TAG_CONCAT(label) TIFFTAG_ ## label
#define ATIFF_GET_SAFE(label, var)\
    if(TIFFGetField(tif, ATIFF_TAG_CONCAT(label), &var) != 1) {\
    fprintf(stderr, "%s: cannot get %s\n",\
            filename.c_str(), ATIFF_STR(label));        \
        return false;\
    }
#define ATIFF_SET_SAFE(label, var)\
    if(TIFFSetField(tif, ATIFF_TAG_CONCAT(label), var) != 1) {\
        fprintf(stderr, "%s: cannot set %s\n",\
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

// atiff::~atiff() {
// }
// コピー演算・ムーブ演算の自動生成のために宣言しない

void atiff::initialize() {
    depth_        = ATIFF_256LEVEL;         // 256階調
 // depth_        = ATIFF_2LEVEL;           // 2階調(白黒2値)
    width_        = 0;                      // 横ピクセル数
    height_       = 0;                      // 縦ピクセル数
    ncolors_      = ATIFF_MONO_COLOR;       // グレイスケール or 白黒2値
 // ncolors_      = ATIFF_RGB_COLOR;        // RGBカラー
 // photometric_  = PHOTOMETRIC_RGB;        // RGB
    photometric_  = PHOTOMETRIC_MINISWHITE; // 0が白、最大が黒
 // photometric_  = PHOTOMETRIC_MINISBLACK; // 0が黒、最大が白
 // compression_  = COMPRESSION_NONE;       // 無圧縮
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
    rowsperstrip_ = 1;                      // 1ストリップ当たりの行数

    nx_       = 0;
    ny_       = 0;
    nc_       = ATIFF_MONO_COLOR;           // グレイスケール or 白黒2値
    ordering_ = 0;                          // single plane
}

bool atiff::build(int width, int height, int ncolors, int ordering) {
    this->initialize();
    width_   = (uint32)width;
    height_  = (uint32)height;
    ncolors_ = (uint16)ncolors;
    nx_      =         width;
    ny_      =         height;
    nc_      =         ncolors;
    if(nc_ == ATIFF_MONO_COLOR) {
        photometric_  = PHOTOMETRIC_MINISWHITE;
        planarmode_   = PLANARCONFIG_CONTIG;
        ordering_     =  0; //single plane
    } else if(nc_ == ATIFF_RGB_COLOR) {
        photometric_ = PHOTOMETRIC_RGB;
        if(ordering == ATIFF_PLANE_MAJOR) {
            planarmode_   = PLANARCONFIG_CONTIG;
            ordering_     = ATIFF_PLANE_MAJOR;
        } else {
            planarmode_   = PLANARCONFIG_SEPARATE;
            ordering_     = ATIFF_PIXEL_MAJOR;
        }
    } else {
        ERRMSG("%d: ncolors is invalid", ncolors);
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
       (photometric_ != PHOTOMETRIC_RGB)) {
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
    ATIFF_GET_SAFE(ORIENTATION,     orientation_);
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
        ordering_ =  0; //single plane
    } else {
        if(planarmode_ == PLANARCONFIG_CONTIG) {
            ordering_ = +1; //plane-major
        } else {
            ordering_ = -1; //pixel-major
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
        rowsperstrip_ = 1;
    }
    int jstep = (int)rowsperstrip_;

    // 描画データの読み込み
    if(depth_ == ATIFF_2LEVEL) {
        if(nx_ % 8 != 0) {
            ERRMSG("%d: width must be multiples of 8", nx_);
            return false;
        }
        buf_.resize(nx_*ny_*nc_/8);
    } else {
        buf_.resize(nx_*ny_*nc_);
    }
    if(ordering_ < 0) { //pixel-major
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
        {
            for(int j = 0; j < ny_; j += jstep) {
                int nrow = (j + jstep > ny_) ? ny_ - j : jstep;
                tstrip_t   pos = TIFFComputeStrip(tif, j, 0);
                tdata_t    ptr = (tdata_t)(&buf_[index(0, j, 0)]);
                tsize_t length = (tsize_t)index(0, nrow, 0);
                if(depth_ == ATIFF_2LEVEL) length /= 8;
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

bool atiff::save(string filename) const {
    // 書き込みモードでTIFFファイルを開く
    TIFF* tif = TIFFOpen(filename.c_str(), "w");
    if(tif == NULL) {
        ERRMSG("%s: cannot open", filename.c_str());
        return false;
    }

    // 階調の深さ(ビット数)
    ATIFF_SET_SAFE(BITSPERSAMPLE,   depth_      );
    // カラーモード(色表現)
    ATIFF_SET_SAFE(PHOTOMETRIC,     photometric_);
    // 画像の横幅(ピクセル数)
    ATIFF_SET_SAFE(IMAGEWIDTH,      width_      );
    // 画像の高さ(ピクセル数)
    ATIFF_SET_SAFE(IMAGELENGTH,     height_     );
    // 色数(サンプル数)
    ATIFF_SET_SAFE(SAMPLESPERPIXEL, ncolors_    );
    // 圧縮形式
    ATIFF_SET_SAFE(COMPRESSION,     compression_);
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
        for(int c = 0; c < nc_; ++c) {
            tdata_t    ptr = (tdata_t)(&buf_[index(0, 0, c)]);
            tsize_t length = (tsize_t)(nx_ * ny_);
            if(TIFFWriteEncodedStrip(tif, c, ptr, length) == -1) {
                ERRMSG("%s: data write error", filename.c_str());
                return false;
            }
        }
    } else { //plane-major
        {
            tdata_t    ptr = (tdata_t)(&buf_[0]);
            tsize_t length = (tsize_t)(nx_ * ny_ * nc_);
            if(depth_ == ATIFF_2LEVEL) length /= 8;
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
