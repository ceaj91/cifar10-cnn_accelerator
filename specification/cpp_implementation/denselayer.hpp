

#include "common.hpp"

class DenseLayer {
private:
    int input_size;
    int output_size;
    int softmax;
    vector2D dense_weights;
    vector1D dense_bias;
    vector2D output;

public:
    DenseLayer(int input_size, int output_size, int softmax);
    void load_dense_layer(const char* weights_file_name, const char* bias_file_name);
    vector2D forward_prop(vector2D input);
    void GetInfo();
    void GetOutput();
};
