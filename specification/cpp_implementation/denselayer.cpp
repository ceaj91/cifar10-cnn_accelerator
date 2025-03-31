#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <functional>
#include <iomanip>

#include "denselayer.hpp"

DenseLayer::DenseLayer(int input_size, int output_size, int softmax) {
        this->input_size = input_size;
        this->output_size = output_size;
        this->softmax = softmax;

        // Postavljamo velicinu vectora weightsa da bude ista kao i input_size
        // Tako sto ga prosirujemo kontenerima koji sadrze 0
        dense_weights.resize(input_size, vector1D(output_size, 0));

        // Postavljamo velicinu vectora biasa da bude ista kao i output_size
        dense_bias.resize(output_size, 0);
    }


void DenseLayer::load_dense_layer(const char* weights_file_name,const char* bias_file_name) {
    std::ifstream weights_file(weights_file_name, std::ifstream::in);
    std::ifstream bias_file(bias_file_name, std::ifstream::in);

    for (int i = 0; i < input_size; ++i) {
        std::string line;
        std::getline(weights_file, line);
        std::istringstream iss(line);
        vector1D float_values;
        for (int j = 0; j < output_size; ++j) {
              float value;
              iss >> value;
              float_values.push_back(value);
        }
        dense_weights[i] = float_values;
    }

    for (int i = 0; i < output_size; ++i) {
        std::string line;
        std::getline(bias_file, line);
        float value = std::stof(line); // string to float
        dense_bias[i] = value;
    }

    weights_file.close();
    bias_file.close();
}



vector2D DenseLayer::forward_prop(vector2D input) {

    int batch_size = input.size();

    // Napravi vector temp koji je size = output_size
    // Inicijalizuj 0 u svakom kontejneru
    vector2D temp(batch_size, vector1D(output_size, 0));


    // Za svaku OUTPUT NODU mnozi svaku INPUT NODU sa odgovarajucim WEIGHTOM
    // I na kraju dodaje bias za svaku OUTPUT NODU
    // Ekvivalent: temp = np.dot(input[0:],self.dense_weights) + self.dense_bias
    for(int batch = 0; batch < batch_size; batch++){
        for (int i = 0; i < output_size; i++) {
                for (int k = 0; k < input_size; k++) {
                    temp[batch][i] += input[batch][k] * dense_weights[k][i];
                }
            temp[batch][i] += dense_bias[i];
        }
    }

    if (!softmax) {
            for(int batch = 0; batch < batch_size; batch++){
                for (int i = 0; i < output_size; i++) {
                            // Sve negativne koef normalizuje na 0
                            // Ekvivalent: output = np.maximum(0,temp)
                        if(temp[batch][i] < 0) temp[batch][i] = 0;
                }
            }
    } else {
            // Pronalazi najveci element u temp vectoru
            float max_val;
            float sum_exp = 0.0;
            for(int batch = 0; batch < batch_size; batch++){
                max_val = *max_element(temp[batch].begin(), temp[batch].end());
                sum_exp = 0.0;
                for (int j = 0; j < output_size; j++) {
                        // Uredjuje temp vector tako sto od starih vrednosti njegovih kontejnera
                        // Oduzima vrednosti max elementa, i eksponira na e (skalira)
                        // Ova for petlja je ekvivalent:
                        // output = temp - np.max(temp,axis=1,keepdims=True)
                        // exp_out = np.exp(output)
                        // sum_exp_out = np.sum(exp_out,axis=1,keepdims=True)
                    temp[batch][j] = exp(temp[batch][j] - max_val);
                    sum_exp += temp[batch][j];
                }
                for (int j = 0; j < output_size; j++) {
                        // Ekvivalent: output = exp_out / sum_exp_out
                    temp[batch][j] /= sum_exp;
                }
            }
    }
    output = temp;
    return output;
}


void DenseLayer::GetInfo() {
	std::cout << "Input size is " << input_size << std::endl;
	std::cout << "Output size is " << output_size << std::endl;
	std::cout << "Softmax ? " << softmax << std::endl;
	std::cout << std::setprecision(20);
	std::cout << "First weight is: " << dense_weights[0][0] << std::endl;
	std::cout << "First bias is: " << dense_bias[0] << std::endl;
}


void DenseLayer::GetOutput() {
	std::cout << "------------------" << std::endl;
	std::cout << "Output is " << std::endl;
	for(int i = 0; i < output.size(); ++i){
        for(int j = 0; j < output[0].size(); j++){
            std::cout << output[i][j] << std::endl;
        }
	}

	std::cout << "------------------" << std::endl;
}
