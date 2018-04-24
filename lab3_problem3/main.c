// Welch, Wright, & Morrow, 
// Real-time Digital Signal Processing, 2011
 
///////////////////////////////////////////////////////////////////////
// Filename: main.c
//
// Synopsis: Main program file for demonstration code
//
///////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include "DSP_Config.h"
#include "fft.h"
#include "gmm.h"
#include "libmfcc.h"
#include <math.h>

#define BUFFERSIZE 256
#define numFilters 48
#define M BUFFERSIZE
int kk = 0;
int startflag = 0;

int training = 1;
short X[BUFFERSIZE];
COMPLEX w[BUFFERSIZE];
COMPLEX B[BUFFERSIZE];
float feature;
float denominator = 0;
float numerator = 0;
float magnitude = 0;
float avg = 0;
int coeff=0;
double spectrum[BUFFERSIZE];
double *spectral_mag[BUFFERSIZE];
double mfcc_result[13]={0};
float llh;
GMM gmm[1]; // create GMM model

int main() {
	int K = 3; // Number of Classes
	int N = 1; // Number of Blocks
	int D = 13; // Number of Features

	DSP_Init();

	int ii, mm, bb, ll;

	Init_UART2(115200); // set baudrate

  	// Initialize GMM model
	gmm_new(gmm, K, D, "diagonal");
	gmm_set_convergence_tol(gmm, 1e-6);
	gmm_set_regularization_value(gmm, 1e-6);
	gmm_set_initialization_method(gmm, "random");
	// (P3). Update GMM parameters (gmm[0]) through UART communication with Matlab
	if(IsDataReady_UART2()){
		int i;
		for (i = 0; i < D; i++) {
			gmm[0].means[i] = Read_UART2();
		}
		int j;
		for (j = 0; j < K; j++) {
			gmm[0].weights[j] = Read_UART2();
		}
		int k;
		for (k = 0; k < K; k++) {
			gmm[0].covars[k] = Read_UART2(); // variances
		}
	}

  	// Twiddle factor
	for(ii=0; ii<M; ii++){
		w[ii].real = cos((float)ii/(float)M*PI);
		w[ii].imag = sin((float)ii/(float)M*PI);
	}
	
	// main stalls here, interrupts drive operation
	while(1) {
		if(startflag){
			 // Remove bias (DC offset)
			avg = 0;
			for(mm=0; mm<M; mm++){
				avg = avg + X[mm];
			}

			// Measure the Magnitude of the input to find starting point
			avg = avg/M;
			magnitude = 0;
			for(mm=0; mm<M; mm++){
				magnitude = magnitude + abs(X[mm]-avg);
			}

			if(magnitude>30000){ // N-blocks
				for(bb=0;bb<N;bb++){
					for(ll=0; ll<M; ll++){
						B[ll].real = X[ll];
						B[ll].imag = 0;
					}
					// (P3). FFT: B is input and output, w is twiddle factors
					fft(B, M, w);
					// (P3). Find 13 MFCC coefficients
					for(mm=0; mm<M; mm++){
						*spectral_mag[mm] = (double) pow((pow(B[mm].real, 2) + pow(B[mm].imag, 2)), 0.5);
					}
					int fs = GetSampleFreq();
					int coeff;
					for (coeff = 0; coeff < D; coeff++) {
						mfcc_result[coeff] = GetCoefficient(*spectral_mag, fs, numFilters, M, coeff);
					}
				}

				// (P3). Print GMM score using gmm model and mfcc_result
				double llh = gmm_score(gmm, mfcc_result, sizeof(mfcc_result));

			}
			startflag = 0;
		}
	}
}
