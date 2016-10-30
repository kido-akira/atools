//----------------------------------------------------------------------
//  A TIFF ver.1.0.0            Time-stamp: <2016-10-31 00:11:07 kido>
//----------------------------------------------------------------------
#ifndef ATIFF_INCLUDED
#define ATIFF_INCLUDED

#include <string>
#include <vector>

#define ATIFF_BYTE              unsigned char
#define ATIFF_INT16             unsigned short
#define ATIFF_INT32             unsigned int
#define ATIFF_FLOAT             float
#define ATIFF_MONO_COLOR        1  // グレイスケール or 白黒2値
#define ATIFF_RGB_COLOR         3  // RGBカラー
#define ATIFF_2LEVEL            1  // 2階調(白黒2値)
#define ATIFF_256LEVEL          8  // 256階調
#define ATIFF_PLANE_MAJOR       +1 // ピクセル毎に格納(plane-major)
#define ATIFF_PIXEL_MAJOR       -1 // プレーン毎に格納(pixel-major)

//----------------------------------------------------------------------
//  ATIFF : TIFF manipulate class
//----------------------------------------------------------------------
class atiff {
protected:
    ATIFF_INT16 depth_;        // 階調の深さ(ビット数)
    ATIFF_INT32 width_;        // 画像の横幅(ピクセル数)
    ATIFF_INT32 height_;       // 画像の高さ(ピクセル数)
    ATIFF_INT16 ncolors_;      // 色数(サンプル数)
    ATIFF_INT16 photometric_;  // カラーモード(色表現)
    ATIFF_INT16 compression_;  // 圧縮形式
    ATIFF_INT16 orientation_;  // 画像の原点
    ATIFF_INT16 fillorder_;    // ビットオーダ
    ATIFF_INT16 planarmode_;   // 優先モード(データの並び順)
    ATIFF_FLOAT xreso_;        // x方向解像度[DPI]
    ATIFF_FLOAT yreso_;        // y方向解像度[DPI]
    ATIFF_INT16 resounit_;     // 解像度の単位
    std::string software_;     // 出力ソフトウェア名
    ATIFF_INT32 rowsperstrip_; // 1ストリップ当たりの行数

    int ordering_; // 0:single plane, +:plane-major, -:pixel-major
    int nx_;
    int ny_;
    int nc_;

    std::vector<ATIFF_BYTE> buf_; //描画データの格納バッファ

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
          int ncolors = ATIFF_MONO_COLOR,
          int major   = ATIFF_PLANE_MAJOR);
 // ~atiff();
    // コピー演算・ムーブ演算の自動生成のために宣言しない

    int       nx() const { return nx_; }
    int       ny() const { return ny_; }
    int       nc() const { return nc_; }
    int ordering() const { return ordering_; }

          ATIFF_BYTE& operator[](int i)       { return buf_[i]; }
    const ATIFF_BYTE& operator[](int i) const { return buf_[i]; }
          ATIFF_BYTE& operator()(int i, int j, int c = 0)       {
        return buf_[index(i, j, c)];
    }
    const ATIFF_BYTE& operator()(int i, int j, int c = 0) const {
        return buf_[index(i, j, c)];
    }

    bool load(std::string filename);
    bool save(std::string filename) const;
    bool build(int width,
               int height,
               int ncolors = ATIFF_MONO_COLOR,
               int major   = ATIFF_PLANE_MAJOR);
};

#undef ATIFF_BYTE
#undef ATIFF_INT16
#undef ATIFF_INT32

#endif //ATIFF_INCLUDED

//----------------------------------------------------------------------
//  End Of File
//----------------------------------------------------------------------
