#define SC_INCLUDE_FX
#include <vector>
#include <systemc>
#define INPUT_PICTURE_SIZE 32
#define INPUT_CHANNEL_SIZE 3

typedef std::vector<std::vector<std::vector<std::vector<float>>>> vector4D;
typedef std::vector<std::vector<std::vector<float>>> vector3D;
typedef std::vector<std::vector<float>> vector2D;
typedef std::vector<float> vector1D;

typedef sc_dt::sc_fix_fast impl_t;

typedef std::vector<std::vector<std::vector<std::vector<impl_t>>>> vector4D_impl;
typedef std::vector<std::vector<std::vector<impl_t>>> vector3D_impl;
typedef std::vector<std::vector<impl_t>> vector2D_impl;
typedef std::vector<impl_t> vector1D_impl;
