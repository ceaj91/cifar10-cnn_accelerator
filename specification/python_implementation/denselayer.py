import numpy as np

class DenseLayer:
  def __init__(self,input_size,output_size,softmax):
    self.input_size = input_size
    self.output_size = output_size
    self.softmax = softmax # 1 ako je poslednji layer, inace 0

  def load_dense_layer(self,weights_file_name,bias_file_name):
    self.dense_weights = np.zeros((self.input_size,self.output_size))
    self.dense_bias = np.zeros(self.output_size)
    self.dense_weights = self.dense_weights.astype('float32')
    self.dense_bias = self.dense_bias.astype('float32')

    with open(str(weights_file_name),'r') as f:
      lines = f.readlines()
      for i in range(self.input_size):
        float_values = np.array(lines[i].split(),dtype=np.float32)
        for j in range(self.output_size):
          self.dense_weights[i,j] = float_values[j] 
    f.close()

    with open(str(bias_file_name), 'r') as f:
      lines = f.readlines()
      for i in range(self.output_size):
        float_number=np.float32(lines[i])
        self.dense_bias[i] =float_number
    f.close()

  def forward_prop(self,input):
    temp = np.dot(input[0:],self.dense_weights) + self.dense_bias
    if(self.softmax==0):
      output = np.maximum(0,temp)
    else:
      output = temp - np.max(temp,axis=1,keepdims=True)
      exp_out = np.exp(output)
      sum_exp_out = np.sum(exp_out,axis=1,keepdims=True)
      output = exp_out / sum_exp_out

    self.output = output
    return output
