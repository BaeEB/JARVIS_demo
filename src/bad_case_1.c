#include "bad_case.h"

short test0902(int x, int y, short e){
   short buf[ 3 ][ 2 ] = { { 1, 2 }, { 0, 0 }, { 5, 6 } }; // Now compliant with MISRA C:2012 Rule 9.2
   buf[x][y] = e;
   return buf[x][y];
}
