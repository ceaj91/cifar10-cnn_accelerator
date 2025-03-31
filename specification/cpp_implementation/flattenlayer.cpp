#include <iostream>
#include <vector>
#include <numeric>

#include "common.hpp"
#include "flattenlayer.hpp"

vector2D FlattenLayer::forward_prop(vector4D input) {
    std::vector<int> input_size = {(int)input.size(), (int)input[0].size(), (int)input[0][0].size(), (int)input[0][0][0].size()};
    int flattened_size = std::accumulate(input_size.begin() + 1, input_size.end(), 1, std::multiplies<int>());

    vector2D X_flat(input_size[0], vector1D(flattened_size));

    int flat_idx = 0;
    for (int i = 0; i < input_size[0]; i++) {
      for (int j = 0; j < input_size[1]; j++) {
        for (int k = 0; k < input_size[2]; k++) {
          for (int l = 0; l < input_size[3]; l++) {
            X_flat[i][flat_idx] = input[i][j][k][l];
            flat_idx++;
          }
        }
      }
      flat_idx = 0;
    }

    output = X_flat;
    return X_flat;
  };
