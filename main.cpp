#include <iostream>
using namespace std;

#include "atools.h"

int main(int argc, char* argv[]) {
    if(argc < 3) {
        ERRMSG("usage: %s input.tif output.tif", argv[0]);
        return 1;
    }
    string  infile = argv[1];
    string outfile = argv[2];

    atiff obj1;
    if(!obj1.load(infile)) return 2;
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
    if(!obj2.save(outfile)) return 3;

    return 0;
}
