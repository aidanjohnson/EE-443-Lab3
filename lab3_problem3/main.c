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

double means[39] = {89.481242,-8.853878,-4.180293,-0.292837,-4.835100,-7.981023,-0.894761,-6.408656,-2.859300,0.607927,-2.315493,1.285301,-0.348409,66.581675,-34.903001,3.789813,-9.022074,4.174007,-3.913041,-14.496893,7.486661,17.130828,-1.255630,-30.675816,33.106668,-11.295202,70.527346,1.943785,3.343556,-5.969985,4.236736,-8.207524,-11.131441,-14.588358,-21.234881,-14.296727,-22.959673,-7.058202,-0.097888};
double covars[39] = {26.100715,21.583987,46.028943,85.574910,26.186812,62.323569,135.065426,50.330170,46.023445,135.515257,70.258029,63.832389,64.943166,303.930624,11.896259,18.360150,25.850217,43.070656,31.375431,48.066910,218.201893,96.006825,52.516978,98.748635,160.398238,156.074272,245.892628,81.315742,40.680347,32.885851,29.206289,65.297095,107.155445,127.465223,267.673339,295.861858,267.927997,157.067352,138.642227};
double weights[3] = {0.374631,0.254626,0.370744};

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
				double llh = gmm_score(gmm, mfcc_result, 1);
			}
			startflag = 0;
		}
	}
}
