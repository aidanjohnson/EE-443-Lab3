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
#define numFilters 20
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

double means[39] = {69.293762,-9.629693,-2.633801,-5.892930,-0.472678,-0.610761,-4.035035,-3.688805,0.632121,1.063087,-0.473543,-0.399980,1.352881,-8.152296,2.455577,-0.907181,-7.632943,-2.843421,-15.911326,-5.186789,-9.176411,-15.138992,-9.663626,-0.503171,-5.530658,-1.096480,-5.861911,-20.030855,-19.597398,8.175964,-1.116135,-2.480942,1.278765,3.125821,-6.738081,0.713563,2.417605,9.894469,-5.551529};
double covars[39] = {7.639745,1.424659,25.120411,8.519288,20.791051,26.585748,8.761488,14.667818,10.251274,10.034052,12.645719,9.140463,5.963226,17.158942,19.313845,25.137146,9.181311,14.191854,25.629091,29.643240,33.839229,35.485160,24.812275,42.908232,53.533194,35.123562,211.519281,9.008078,18.915956,15.707187,11.769270,17.908905,18.659993,39.368493,43.727406,16.270682,96.588529,27.062061,60.504957};
double weights[3] = {0.358608,0.364853,0.276539};

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
