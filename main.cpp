#include <iostream>
using namespace std;

#include "atiff.h"

int main(int argc, char* argv[]) {
    if(argc < 3) {
        cerr << "usage: " << argv[0] << " input.tif output.tif" << endl;
        return 1;
    }
    atiff obj1(argv[1]);
    int nx = obj1.nx();
    int ny = obj1.ny();
    int nc = obj1.nc();
    atiff obj2(nx, ny); //gray-scale
    for(int iy = 0; iy < ny; ++iy) {
    for(int ix = 0; ix < nx; ++ix) {
        int sum = 0;
        for(int ic = 0; ic < nc; ++ic) { //maybe multi-color
            sum += obj1(ix, iy, ic);
        }
        obj2(ix, iy) = sum / nc;
    }
    }
    obj2.save(argv[2], 1, ACOMPRESSION_NONE);
    return 0;
}
