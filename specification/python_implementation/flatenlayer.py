import numpy as np

class FlatenLayer:
  def forward_prop(self,input):
    input_size = input.shape
    flattened_size = np.prod(input_size[1:])
    X_flat = np.reshape(input,(input_size[0],flattened_size))
    self.output = X_flat
    return X_flat
