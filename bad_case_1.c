#include "bad_case.h"

short test0902(int x, int y, short e){
   short buf[ 3 ][ 2 ] = { { 1, 2 }, { 0, 0 }, { 5, 6 } }; // Corrected to use braces around initializers
   buf[x][y] = e;
   return buf[x][y];
}
