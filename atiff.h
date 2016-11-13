//----------------------------------------------------------------------
//  A TIFF ver.1.0.0            Time-stamp: <2016-11-13 09:37:53 kido>
//----------------------------------------------------------------------
#ifndef ATIFF_INCLUDED
#define ATIFF_INCLUDED

#include <string>
#include <vector>

#define ATIFF_1BIT_DEPTH        1  // 2�K��(����2�l)
#define ATIFF_8BIT_DEPTH        8  // 256�K��
#define ATIFF_ORDER_NONE         0 // 1�v���[��
#define ATIFF_PLANE_MAJOR       +1 // �s�N�Z�����Ɋi�[(plane-major)
#define ATIFF_PIXEL_MAJOR       -1 // �v���[�����Ɋi�[(pixel-major)

// see tiff.h
#define ABYTE                      unsigned char
#define AINT16                     unsigned short
#define AINT32                     unsigned int
#define AFLOAT                     float
#define APHOTOMETRIC_MINISWHITE    0 // 0�����A�ő傪��
#define APHOTOMETRIC_MINISBLACK    1 // 0�����A�ő傪��
#define APHOTOMETRIC_RGB           2 // RGB or RGBA
#define APHOTOMETRIC_SEPARATED     5 // CMYK
#define ACOMPRESSION_NONE          1 // �����k(�_���v���[�h)
#define ACOMPRESSION_LZW           5 // Lempel-Ziv & Welch���k

//----------------------------------------------------------------------
//  ATIFF : TIFF manipulate class
//----------------------------------------------------------------------
class atiff {
protected:
    AINT16 depth_;        // �K���̐[��(�r�b�g��)
    AINT32 width_;        // �摜�̉���(�s�N�Z����)
    AINT32 height_;       // �摜�̍���(�s�N�Z����)
    AINT16 ncolors_;      // �F��(�T���v����)
    AINT16 photometric_;  // �J���[���[�h(�F�\��)
    AINT16 compression_;  // ���k�`��
    AINT16 orientation_;  // �摜�̌��_
    AINT16 fillorder_;    // �r�b�g�I�[�_
    AINT16 planarmode_;   // �D�惂�[�h(�f�[�^�̕��я�)
    AFLOAT xreso_;        // x�����𑜓x[DPI]
    AFLOAT yreso_;        // y�����𑜓x[DPI]
    AINT16 resounit_;     // �𑜓x�̒P��
    AINT32 rowsperstrip_; // 1�X�g���b�v������̍s��
    std::string software_;// �o�̓\�t�g�E�F�A��

    int ordering_; // 0:single plane, +:plane-major, -:pixel-major
    int nx_;
    int ny_;
    int nc_;

    std::vector<ABYTE> buf_; //�`��f�[�^�̊i�[�o�b�t�@

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
    // �R�s�[���Z�E���[�u���Z�̎��������̂��߂ɐ錾���Ȃ�

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

    int   depth      () const { return (int)depth_;       }
    int   photometric() const { return (int)photometric_; }
    int   compression() const { return (int)compression_; }
    float xreso      () const { return      xreso_;       }
    float yreso      () const { return      yreso_;       }
    int   resounit   () const { return (int)resounit_;    }

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
