#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <iomanip>
#include <random>

#include "common.hpp"
#include "MaxPoolLayer.hpp"

MaxPoolLayer::MaxPoolLayer(int param_pool_size) {
    pool_size = {param_pool_size, param_pool_size};
}

vector4D MaxPoolLayer::forward_prop(vector4D input, std::vector<int> stride = {}) {

    input_shape = {(int)input.size(), (int)input[0].size(), (int)input[0][0].size(), (int)input[0][0][0].size()};
    int h_start;
    int h_end;
    int w_start;
    int w_end;


    if (stride.empty()) {
        stride = pool_size;
    }

    int output_height = std::floor((input_shape[1] - pool_size[0]) / stride[0]) + 1;
    int output_width = std::floor((input_shape[2] - pool_size[1]) / stride[1]) + 1;
    int output_channels = input_shape[3];
    output_shape = {input_shape[0], output_height, output_width, output_channels};


    output.resize(output_shape[0], vector3D(output_shape[1], vector2D(output_shape[2], vector1D(output_shape[3], 0))));

    vector2D pool_region(pool_size[0], vector1D(pool_size[1], 0));

    for (int b = 0; b < output_shape[0]; b++) {
        for (int h = 0; h < output_shape[1]; h++) {
            for (int w = 0; w < output_shape[2]; w++) {
                for (int c = 0; c < output_shape[3]; c++) {
                    h_start = h * stride[0];
                    h_end = h_start + pool_size[0];
                    w_start = w * stride[1];
                    w_end = w_start + pool_size[1];

                    for (int i = h_start; i < h_end; i++) {
                        for (int j = w_start; j < w_end; j++) {
                            pool_region[i-h_start][j-w_start] = input[b][i][j][c];
                        }
                    }

                    float max_elem = pool_region[0][0];
                    for (int i = 0; i < pool_size[0]; i++) {
                        for (int j = 0; j < pool_size[1]; j++) {
                            if(pool_region[i][j] > max_elem) max_elem = pool_region[i][j];
                        }
                    }
                    output[b][h][w][c] = max_elem;
                }
            }
        }
    }

    return output;

}


void MaxPoolLayer::GetInfo() {
    std::cout << "----------------" << std::endl;
    std::cout << "Output tensor:" << std::endl;

    for(int i = 0; i < output_shape[0]; i++) {
        for(int j = 0; j < output_shape[3]; j++) {
            for(int k = 0; k < output_shape[1]; k++) {
                for(int m = 0; m < output_shape[2]; m++) {
                    std::cout << output[i][m][k][j] << " ";
                }
                std::cout << std::endl;
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
}