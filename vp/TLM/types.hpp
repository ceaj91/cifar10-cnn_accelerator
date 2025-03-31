#ifndef TYPES_H
#define TYPES_H
#define SC_INCLUDE_FX
#include <systemc>
#include <vector>
#include <tlm>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>
#define QUANTUM
#define W   16
#define Q   SC_RND
#define O   SC_SAT_SYM


#define CLK_PERIOD 10

// ------------
// Framerate calculation:
#define CONV0_PARALELISM 1
#define CONV1_PARALELISM 1
#define CONV2_PARALELISM 1

// Comment 2 lines below if it is not needed to simulate certain delays in convolution
//#define IMG_SLICE_TO_BUFFER_DELAY
//#define COMPARE_AND_BIAS_ADDITION_DELAY

//-------------
// Simulation context:

// Comment/uncomment following line to disallow/allow printing context:
#define PRINTS

//-------------
using namespace sc_dt;
using namespace std;
using namespace tlm;

typedef sc_fixed_fast<W,4,Q,O> hwdata_t;
typedef vector<vector<vector<vector<float>>>> vector4D;
typedef vector<vector<vector<float>>> vector3D;
typedef vector<vector<float>> vector2D;
typedef vector<float> vector1D;
typedef tlm_base_protocol_types::tlm_payload_type pl_t;

#endif