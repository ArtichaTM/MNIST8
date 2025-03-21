#include "matmul.hpp"


/* Layer 1 matrix multiplication */
void hwmm_layer1(float input[n_inputs], const float weights[n_inputs][n_layer1], const float bias[n_layer1], float output[1][n_layer1]) {
#pragma HLS ARRAY_PARTITION variable=input complete dim=1
#pragma HLS ARRAY_PARTITION variable=weights complete dim=1
#pragma HLS ARRAY_PARTITION variable=output complete dim=2
    col: for (int j = 0; j < n_layer1; ++j) {
#pragma HLS UNROLL
      float sum = 0;
      prod: for (int k = 0; k < n_inputs; ++k) {
#pragma HLS UNROLL
        sum += input[k] * weights[k][j];
      }
      output[0][j] = sum + bias[j];
    }
  return;
}


/* Layer 2 matrix multiplication */
void hwmm_layer2(float input[1][n_layer1], const float weights[n_layer1][n_layer2], const float bias[n_layer2], float output[1][n_layer2]) {
#pragma HLS ARRAY_PARTITION variable=input complete dim=2
#pragma HLS ARRAY_PARTITION variable=weights complete dim=1
#pragma HLS ARRAY_PARTITION variable=output complete dim=2

    col: for (int j = 0; j < n_layer2; ++j) {
#pragma HLS UNROLL
      float sum = 0;

      prod: for (int k = 0; k < n_layer1; ++k) {
#pragma HLS UNROLL
        sum += input[0][k] * weights[k][j];
      }
      output[0][j] = sum + bias[j];
    }
  return;
}


/* ReLU layer 1 activation function */
void hw_act_layer1(float input[1][n_layer1], float output[1][n_layer1]) {
#pragma HLS ARRAY_PARTITION variable=input complete dim=2
#pragma HLS ARRAY_PARTITION variable=output complete dim=2
	loop1: for (int i = 0; i < n_layer1; i++){
#pragma HLS UNROLL
		if (input[0][i] < 0.0)
			output[0][i] = 0.0;
		else
			output[0][i] = input[0][i];
	}
	return;
}


/* Softmax layer 2 activation function */
void hw_act_layer2(float input[1][n_layer2], int &pred) {
#pragma HLS ARRAY_PARTITION variable=input complete dim=2
	int max_idx = -1;
	float max_val = -999.9;
	loop1: for (int i = 0; i < n_layer2; i++){
#pragma HLS UNROLL
		if (input[0][i] > max_val){
			max_idx = i;
			max_val = input[0][i];
		}
	}
	pred = max_idx;
	return;
}


/* Connect NN Layers */
int nn_inference(float input_img[n_inputs]){
#pragma HLS ARRAY_PARTITION variable=input_img complete dim=1

	float temp_output[1][n_layer1] = {0};
	float temp_output2[1][n_layer2] = {0};
	int prediction = -1;

	hwmm_layer1(input_img, weights::layer1_weights, weights::layer1_bias, temp_output);
	hw_act_layer1(temp_output, temp_output);
	hwmm_layer2(temp_output, weights::layer2_weights, weights::layer2_bias, temp_output2);
	hw_act_layer2(temp_output2, prediction);

	return prediction;

}
