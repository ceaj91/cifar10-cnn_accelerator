import numpy as np

class ConvLayer:
  def __init__(self,filter_size,number_of_channels,num_of_filters,padding='same'):
    self.filter_size=filter_size
    self.number_of_channels = number_of_channels
    self.num_of_filters = num_of_filters
    self.padding=padding
    self.conv_filters = np.zeros((self.filter_size, self.filter_size, self.number_of_channels, self.num_of_filters))
    self.conv_bias = np.zeros(self.num_of_filters)
    self.conv_filters = self.conv_filters.astype('float32')
    self.conv_bias = self.conv_bias.astype('float32')
    
  
  def load_conv_layer(self,filter_file_name,bias_file_name):
    row=0
    channel=0
    filter=0
    with open(str(filter_file_name), 'r') as f:
      lines = f.readlines()
      for i in range(len(lines)):
        float_values = np.array(lines[i].split(),dtype=np.float32)
        self.conv_filters[row,0,channel,filter] = float_values[0]
        self.conv_filters[row,1,channel,filter] = float_values[1]
        self.conv_filters[row,2,channel,filter] = float_values[2]
        row = row+1
        if(row == self.filter_size): 
          row=0
          channel = channel+1
          if(channel == self.number_of_channels):
            channel = 0
            filter = filter+1
            if(filter == self.num_of_filters):
              break;
    f.close()

    with open(str(bias_file_name), 'r') as f:
      lines = f.readlines()
      for i in range(len(lines)):
        float_number=np.float32(lines[i])
        self.conv_bias[i] = float_number
      #print(conv1_bias[i])  
    f.close()

  def forward_prop(self,input_tensor, stride=1):
    """
    Perform 2D convolution on input tensor with given kernel weights and bias weights.
    
    Arguments:
    input_tensor -- Input tensor of shape (batch_size, input_height, input_width, input_channels)
    kernel_weights -- Kernel weights of shape (kernel_height, kernel_width, input_channels, output_channels)
    bias_weights -- Bias weights of shape (output_channels,)
    padding -- Padding method. 'same' or 'valid'. Default is 'same'.
    stride -- Stride length. Default is 1.
    
    Returns:
    convolved_tensor -- Convolved tensor of shape (batch_size, output_height, output_width, output_channels)
    """
    
    # Get input tensor shape
    batch_size, input_height, input_width, input_channels = input_tensor.shape
    
    # Get kernel weights shape
    kernel_height, kernel_width, _, output_channels = self.conv_filters.shape
    
    # Calculate output tensor shape
    if self.padding == 'same':
          output_height = int(np.ceil(float(input_height) / float(stride)))
          output_width = int(np.ceil(float(input_width) / float(stride)))
          padding_height = ((output_height - 1) * stride + kernel_height - input_height) // 2
          padding_width = ((output_width - 1) * stride + kernel_width - input_width) // 2
          input_tensor = np.pad(input_tensor, ((0, 0), (padding_height, padding_height), 
                                             (padding_width, padding_width), (0, 0)), mode='constant')
    elif self.padding == 'valid':
          output_height = int(np.ceil(float(input_height - kernel_height + 1) / float(stride)))
          output_width = int(np.ceil(float(input_width - kernel_width + 1) / float(stride)))
    
    # Initialize output tensor
    convolved_tensor = np.zeros((batch_size, output_height, output_width, output_channels))

    # Perform convolution
    for i in range(output_height):
          for j in range(output_width):
              input_slice = input_tensor[:, i*stride:i*stride+kernel_height, 
                                        j*stride:j*stride+kernel_width, :]
              for k in range(output_channels):
                  convolved_tensor[:, i, j, k] = np.sum(input_slice * self.conv_filters[:, :, :, k], axis=(1, 2, 3)) + self.conv_bias[k]
    
    convolved_tensor = np.maximum(convolved_tensor, 0)
    self.output = convolved_tensor
    return convolved_tensor
