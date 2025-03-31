#ifndef TYPES_H
#define TYPES_H
#define SC_INCLUDE_FX
#include <systemc>
#include <vector>
#define W   16
#define Q   SC_RND
#define O   SC_SAT_SYM
using namespace sc_dt;
using namespace std;


typedef sc_fixed_fast<W,4,Q,O> hwdata_t;
typedef vector<vector<vector<vector<float>>>> vector4D;
typedef vector<vector<vector<float>>> vector3D;
typedef vector<vector<float>> vector2D;
typedef vector<float> vector1D;
#endif