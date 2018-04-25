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

double means[39] = {55.41294813,2.252788404,-0.556662682,-7.423346771,-2.474767998,-15.48520814,-4.899061448,-9.071620764,-15.17051189,-10.11818708,-1.499874373,-6.634220428,-1.78827644,48.2102645,-17.14338595,-18.48036885,5.599185845,-3.281692406,-6.523866071,1.797533142,-1.050195537,-3.753813644,1.879293328,-1.880303776,11.06334315,0.189924814,68.62885399,-12.09173378,-6.452327198,-2.438582243,-0.085607682,-0.278211813,-3.512968313,-1.022882746,-2.60503226,0.40747978,2.554614121,1.654221305,-1.86659562};
double covars[39] = {14.55488607,19.5989198,24.1538145,8.509932438,12.25040987,23.48955831,29.49280116,35.29366191,37.01939496,21.94673472,26.12472438,33.67127647,27.89471426,195.9791879,73.66429411,40.00177001,54.63798984,15.42211561,47.52862031,25.80038408,41.92995756,29.59812576,16.81655519,76.11054092,30.02538184,44.98519358,7.530712698,22.10078845,69.46993089,47.1234126,17.09509342,21.61947041,11.85253675,37.75127115,45.9313067,10.573994,44.63550722,24.98639858,43.87903994};
double weights[3] = {0.345198288134232,0.189372178821439,0.465429533044329};

// stores 8-bit segments of double (64-bit) data
volatile union {
	double sh;
	Uint8 i8[8];
} UARTdouble;

void storeGMM(int index, int param)
{
	if (param == 1) {
		gmm[0].means[index] = means[index];
	} else if (param == 2) {
		gmm[0].weights[index] = weights[index];
	} else if (param == 3) {
		gmm[0].covars[index] = covars[index];
	}

	// UART input
//	int iter = 0;
//	while (iter < 8) {
//		if(IsDataReady_UART2()){
//			UARTdouble.i8[iter++] = Read_UART2();
//			if(iter==8){
//				if (param == 1) {
//					gmm[0].means[index] = UARTdouble.sh;
//				} else if (param == 2) {
//					gmm[0].weights[index] = UARTdouble.sh;
//				} else if (param == 3) {
//					gmm[0].covars[index] = UARTdouble.sh;
//				}
//			}
//			while(IsTxReady_UART2()==0) ;
//			Write_UART2(1);
//			wait(10000);
//		}
//	}
}

void modelGM(int K, int D)
{
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

				// (P3). Print GMM score using gmm model and mfcc_result
				double llh = gmm_score(gmm, mfcc_result, N);
			}
			startflag = 0;
		}
	}
}
