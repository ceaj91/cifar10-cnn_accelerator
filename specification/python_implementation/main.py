import tensorflow as tf
from tensorflow.keras import layers, models
from tensorflow.keras.datasets import cifar10
from tensorflow.keras.utils import to_categorical
import numpy as np
from convlayer import ConvLayer
from maxpoollayer import MaxPoolLayer
from flatenlayer import FlatenLayer
from denselayer import DenseLayer

conv1_filter = '../parametars/conv1/conv1_filters.txt'
conv1_bias = '../parametars/conv1/conv1_bias.txt'
conv2_filter = '../parametars/conv2/conv2_filters.txt'
conv2_bias = '../parametars/conv2/conv2_bias.txt'
conv3_filter = '../parametars/conv3/conv3_filters.txt'
conv3_bias = '../parametars/conv3/conv3_bias.txt'
dense1_weights='../parametars/dense1/dense1_weights.txt'
dense1_bias='../parametars/dense1/dense1_bias.txt'
dense2_weights='../parametars/dense2/dense2_weights.txt'
dense2_bias='../parametars/dense2/dense2_bias.txt'

#Loading data
(_, _), (x_test, y_test) = cifar10.load_data()
x_test = x_test.astype('float32')

# Normalize pixel values between 0 and 1
x_test = x_test / 255.0


# Convert labels to one-hot encoding
y_test = to_categorical(y_test)





#CNN initialization
conv1=ConvLayer(3,3,32)
max_pool1=MaxPoolLayer(2)
conv2=ConvLayer(3,32,32)
max_pool2=MaxPoolLayer(2)
conv3=ConvLayer(3,32,64)
max_pool3=MaxPoolLayer(2)
flatten = FlatenLayer()
dense1 = DenseLayer(1024,512,0)
dense2 = DenseLayer(512,10,1)

#loading filters, bias and weights
conv1.load_conv_layer(conv1_filter,conv1_bias)
conv2.load_conv_layer(conv2_filter,conv2_bias)
conv3.load_conv_layer(conv3_filter,conv3_bias)
dense1.load_dense_layer(dense1_weights,dense1_bias)
dense2.load_dense_layer(dense2_weights,dense2_bias)








#print(output.shape)
#max_index = np.argmax(output[0])

broj_pogodaka=0

print(f"Tacnos mreze: {broj_pogodaka/10000}")
for i in range(10000):
	image=x_test[i,:,:,:]
	image = np.expand_dims(image,axis=0)
	output = conv1.forward_prop(image)
	output = max_pool1.forward_prop(output)
	output = conv2.forward_prop(output)
	output = max_pool2.forward_prop(output)
	output = conv3.forward_prop(output)
	output = max_pool3.forward_prop(output)
	output=flatten.forward_prop(output)
	output=dense1.forward_prop(output)
	output=dense2.forward_prop(output)
	max_index = np.argmax(output[0])
	
	if(max_index == np.argmax(y_test[i])):
		print("POGODAK!")
		broj_pogodaka = broj_pogodaka+1
	else:
		print("PROMASAJ")
print(f"Tacnos mreze: {broj_pogodaka/10000}")



