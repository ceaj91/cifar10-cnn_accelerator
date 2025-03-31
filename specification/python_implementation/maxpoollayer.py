import numpy as np

class MaxPoolLayer:
  def __init__(self,pool_size):
    self.pool_size=(pool_size,pool_size)
  def forward_prop(self,input_tensor,stride=None):
    input_shape = input_tensor.shape
    if stride is None:
        stride = self.pool_size
    
    output_height = int((input_shape[1] - self.pool_size[0]) / stride[0]) + 1
    output_width = int((input_shape[2] - self.pool_size[1]) / stride[1]) + 1
    output_channels = input_shape[3]
    output_shape = (input_shape[0], output_height, output_width, output_channels)

    output_tensor = np.zeros(output_shape)
    for b in range(output_shape[0]):
        for h in range(output_shape[1]):
            for w in range(output_shape[2]):
                for c in range(output_shape[3]):
                    h_start = h * stride[0]
                    h_end = h_start + self.pool_size[0]
                    w_start = w * stride[1]
                    w_end = w_start + self.pool_size[1]
                    pool_region = input_tensor[b, h_start:h_end, w_start:w_end, c]
                    output_tensor[b, h, w, c] = np.max(pool_region)

    

    self.output = output_tensor
    return output_tensor

