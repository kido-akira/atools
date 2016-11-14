//----------------------------------------------------------------------
//  A TIFF ver.1.0.0            Time-stamp: <2016-11-15 05:50:06 kido>
//
//      Copyright (c) 2016 Akira KIDO
//      https://github.com/kido-akira/atools
//      This software is released under the MIT License.
//      http://opensource.org/licenses/mit-license.php
//----------------------------------------------------------------------
#ifndef ATIFF_INCLUDED
#define ATIFF_INCLUDED

#include <string>
#include <vector>

#define ATIFF_1BIT_DEPTH        1  // 2階調(白黒2値)
#define ATIFF_8BIT_DEPTH        8  // 256階調
#define ATIFF_ORDER_NONE         0 // 1プレーン
#define ATIFF_PLANE_MAJOR       +1 // ピクセル毎に格納(plane-major)
#define ATIFF_PIXEL_MAJOR       -1 // プレーン毎に格納(pixel-major)

// see tiff.h
#define ABYTE                      unsigned char
#define AINT16                     unsigned short
#define AINT32                     unsigned int
#define AFLOAT                     float
#define APHOTOMETRIC_MINISWHITE    0 // 0が白、最大が黒
#define APHOTOMETRIC_MINISBLACK    1 // 0が黒、最大が白
#define APHOTOMETRIC_RGB           2 // RGB or RGBA
#define APHOTOMETRIC_SEPARATED     5 // CMYK
#define ACOMPRESSION_NONE          1 // 無圧縮(ダンプモード)
#define ACOMPRESSION_LZW           5 // Lempel-Ziv & Welch圧縮

//----------------------------------------------------------------------
//  ATIFF : TIFF manipulate class
//----------------------------------------------------------------------
class atiff {
protected:
    AINT16 depth_;        // 階調の深さ(ビット数)
    AINT32 width_;        // 画像の横幅(ピクセル数)
    AINT32 height_;       // 画像の高さ(ピクセル数)
    AINT16 ncolors_;      // 色数(サンプル数)
    AINT16 photometric_;  // カラーモード(色表現)
    AINT16 compression_;  // 圧縮形式
    AINT16 orientation_;  // 画像の原点
    AINT16 fillorder_;    // ビットオーダ
    AINT16 planarmode_;   // 優先モード(データの並び順)
    AFLOAT xreso_;        // x方向解像度[DPI]
    AFLOAT yreso_;        // y方向解像度[DPI]
    AINT16 resounit_;     // 解像度の単位
    AINT32 rowsperstrip_; // 1ストリップ当たりの行数
    std::string software_;// 出力ソフトウェア名

    int ordering_; // 0:single plane, +:plane-major, -:pixel-major
    int nx_;
    int ny_;
    int nc_;

    std::vector<ABYTE> buf_; //描画データの格納バッファ

    void initialize();
    int index(int i, int j, int c) const {
        int ijc = i + j*nx_;
        if     (ordering_ == 0) { return ijc; }
        else if(ordering_  > 0) { ijc  = ijc*nc_ + c; }
        else if(ordering_  < 0) { ijc += c*nx_*ny_; }
        return ijc;
    }

public:
    atiff();
    atiff(int width,
          int height,
          int ncolors  = 1,
          int ordering = ATIFF_ORDER_NONE);
 // ~atiff();
    // コピー演算・ムーブ演算の自動生成のために宣言しない

          ABYTE& operator[](int i)       { return buf_[i]; }
    const ABYTE& operator[](int i) const { return buf_[i]; }
          ABYTE& operator()(int i, int j, int c = 0)       {
        return buf_[index(i, j, c)];
    }
    const ABYTE& operator()(int i, int j, int c = 0) const {
        return buf_[index(i, j, c)];
    }

    bool load(std::string filename);
    bool save(std::string filename, int depth = 0) const;
    bool build(int width,
               int height,
               int ncolors  = 1,
               int ordering = ATIFF_ORDER_NONE);

    int            nx() const { return nx_; }
    int            ny() const { return ny_; }
    int            nc() const { return nc_; }
    int      ordering() const { return ordering_; }

    int   depth      () const { return (int)  depth_;       }
    int   photometric() const { return (int)  photometric_; }
    int   compression() const { return (int)  compression_; }
    float xreso      () const { return (float)xreso_;       }
    float yreso      () const { return (float)yreso_;       }
    int   resounit   () const { return (int)  resounit_;    }

    void depth      (int   i)  { depth_       = (AINT16)i; }
    void photometric(int   i)  { photometric_ = (AINT16)i; }
    void compression(int   i)  { compression_ = (AINT16)i; }
    void xreso      (float f)  { xreso_       = (AFLOAT)f; }
    void yreso      (float f)  { yreso_       = (AFLOAT)f; }
    void resounit   (int   i)  { resounit_    = (AINT16)i; }
};

#undef ABYTE
#undef AINT16
#undef AINT32
#undef AFLOAT

#endif //ATIFF_INCLUDED

//----------------------------------------------------------------------
//  End Of File
//----------------------------------------------------------------------
