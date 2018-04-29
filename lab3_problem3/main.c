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
int startflag = 1;

int training = 1;
short X[BUFFERSIZE];
COMPLEX w[BUFFERSIZE]; // twiddle factors
COMPLEX B[BUFFERSIZE]; // spectrum i/o
float feature;
float denominator = 0;
float numerator = 0;
float magnitude = 0;
float avg = 0;
int coeff=0;
double spectrum[BUFFERSIZE]; // magnitude of spectrum
double mfcc_result[13]={0};
float llh;
GMM gmm[1]; // create GMM model

double means[39] = {-0.257190,6.494495,5.953608,-4.465433,4.957656,-8.330755,-11.073247,-14.433859,-25.975059,-21.044766,-30.539448,-10.112836,1.522666,18.546886,-14.665365,-14.439087,14.024178,-5.309502,-9.861339,8.944821,-5.630116,-0.381186,2.032356,-4.438779,7.487769,-0.430139,-1.334688,-17.958052,0.709882,-7.298532,0.091223,-6.130308,-9.430713,-2.657167,3.064354,-0.409081,-13.842112,12.443726,-5.167535};
double covars[39] = {29.513145,24.722302,29.308259,24.366678,25.742153,47.537002,55.102085,77.347074,184.606274,233.903036,99.936321,144.799016,153.042719,4.870084,8.067373,21.809915,22.504691,40.797584,57.040262,39.501916,26.435657,35.067687,41.325525,44.589819,50.518709,34.330855,277.051673,203.469152,20.551286,28.254793,49.420135,60.907186,134.477981,225.993917,243.484489,114.475515,298.548987,377.517667,134.409795};
double weights[3] = {0.252383,0.092072,0.655546};

// stores 8-bit segments of double (64-bit) data
volatile union {
	double sh;
	Uint8 i8[8];
} UARTdouble;

// comment out second header for UART, first for hard coded means, covars, weights
//void storeGMM(double *parameters, int paramsize)
void storeGMM(int index, int param)
{
//	int ii = 0;
//	int iter = 0;
//    Uint8 temp = 0;
//	while(ii<paramsize){
//		if(IsDataReady_UART2()){
//            temp = Read_UART2();
//            UARTdouble.i8[iter++] = temp;
//            while(IsTxReady_UART2()==0) ;
//            Write_UART2(1);
//
//            if(iter>7){
//                iter = 0;
//                parameters[ii++] = UARTdouble.sh;
//                printf("Index: %d, Received double: %lf \n", ii, UARTdouble.sh);
//            }
//		}
//	}

	// uncomment for hard coded means, covars, weights; comment for UART
	if (param == 1) {
		gmm[0].means[index] = means[index];
	} else if (param == 2) {
		gmm[0].weights[index] = weights[index];
	} else if (param == 3) {
		gmm[0].covars[index] = covars[index];
	}

}

void modelGM(int K, int D)
{
	// comment out for hard coded means, covars, weights; uncomment for UART
//	storeGMM(gmm[0].means, D*K);
//	storeGMM(gmm[0].weights, K);
//	storeGMM(gmm[0].covars, D*K);

	// uncomment for hard coded means, covars, weights; comment for UART
	// Get GM model mean, weights, variances
	int i;
	for (i = 0; i < D*K; i++) {
		storeGMM(i, 1);
	}
	int j;
	for (j = 0; j < K; j++) {
		storeGMM(j, 2);
	}
	int k;
	for (k = 0; k < D*K; k++) {
		storeGMM(k, 3);
	}
}

void initializeGMM(int K, int D)
{
  	// Initialize GM model
	gmm_new(gmm, K, D, "diagonal");
	gmm_set_convergence_tol(gmm, 1e-6);
	gmm_set_regularization_value(gmm, 1e-6);
	gmm_set_initialization_method(gmm, "random");

	modelGM(K, D);
}

void twiddleFactors()
{
	int ii;
  	// Gets twiddle factor
	for(ii=0; ii<M; ii++){
		w[ii].real = cos((float)ii/(float)M*PI);
		w[ii].imag = sin((float)ii/(float)M*PI);
	}
}

int main() {
	int K = 3; // Number of Classes
	int N = 1; // Number of Blocks
	int D = 13; // Number of Features
	int mm, bb, ll;

	DSP_Init();

	Init_UART2(115200); // set baudrate
	initializeGMM(K, D); // get GM model
	twiddleFactors();

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
						double re = (double) B[mm].real;
						double im = (double) B[mm].imag;
						spectrum[mm] = sqrt(re*re + im*im);
					}
					int fs = GetSampleFreq();
					int coeff;
					for (coeff = 0; coeff < D; coeff++) {
						mfcc_result[coeff] = GetCoefficient(spectrum, fs, numFilters, M, coeff);
					}
				}

				// (P3). Print GMM score using gmm model and mfcc_result for 1 21 ms frame
				double llh = gmm_score(gmm, mfcc_result, N);
			}
			startflag = 0;
		}
	}
}
