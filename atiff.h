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
#define ATIFF_MONO_COLOR        1  // �O���C�X�P�[�� or ����2�l
#define ATIFF_RGB_COLOR         3  // RGB�J���[
#define ATIFF_2LEVEL            1  // 2�K��(����2�l)
#define ATIFF_256LEVEL          8  // 256�K��
#define ATIFF_PLANE_MAJOR       +1 // �s�N�Z�����Ɋi�[(plane-major)
#define ATIFF_PIXEL_MAJOR       -1 // �v���[�����Ɋi�[(pixel-major)

//----------------------------------------------------------------------
//  ATIFF : TIFF manipulate class
//----------------------------------------------------------------------
class atiff {
protected:
    ATIFF_INT16 depth_;        // �K���̐[��(�r�b�g��)
    ATIFF_INT32 width_;        // �摜�̉���(�s�N�Z����)
    ATIFF_INT32 height_;       // �摜�̍���(�s�N�Z����)
    ATIFF_INT16 ncolors_;      // �F��(�T���v����)
    ATIFF_INT16 photometric_;  // �J���[���[�h(�F�\��)
    ATIFF_INT16 compression_;  // ���k�`��
    ATIFF_INT16 orientation_;  // �摜�̌��_
    ATIFF_INT16 fillorder_;    // �r�b�g�I�[�_
    ATIFF_INT16 planarmode_;   // �D�惂�[�h(�f�[�^�̕��я�)
    ATIFF_FLOAT xreso_;        // x�����𑜓x[DPI]
    ATIFF_FLOAT yreso_;        // y�����𑜓x[DPI]
    ATIFF_INT16 resounit_;     // �𑜓x�̒P��
    std::string software_;     // �o�̓\�t�g�E�F�A��
    ATIFF_INT32 rowsperstrip_; // 1�X�g���b�v������̍s��

    int ordering_; // 0:single plane, +:plane-major, -:pixel-major
    int nx_;
    int ny_;
    int nc_;

    std::vector<ATIFF_BYTE> buf_; //�`��f�[�^�̊i�[�o�b�t�@

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
    // �R�s�[���Z�E���[�u���Z�̎��������̂��߂ɐ錾���Ȃ�

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
