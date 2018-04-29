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

short X[BUFFERSIZE];
COMPLEX w[BUFFERSIZE]; // twiddle factors
COMPLEX B[BUFFERSIZE]; // spectrum i/o
float denominator = 0;
float numerator = 0;
float magnitude = 0;
float avg = 0;
int coeff=0;
double spectrum[BUFFERSIZE]; // magnitude of spectrum
double mfcc_result[13]={0};
GMM gmm[1]; // create GMM model

double means[39] = {-21.886301,6.976259,6.042062,-4.477110,4.430626,-9.190420,-11.391828,-14.599642,-27.150184,-21.557519,-30.328668,-9.382853,2.176836,56.965736,-8.535606,-3.669411,-2.013095,-2.403152,-6.841192,-1.904727,-7.778224,-5.127691,-1.682388,-5.240222,0.280528,-2.112341,-26.870300,-32.062663,3.481534,-9.211527,3.318361,-5.534016,-16.145194,4.625832,15.333227,1.840135,-26.633860,29.791624,-9.028150};
double covars[39] = {22.405835,22.124207,30.521040,25.387684,24.134605,45.671848,60.058246,83.720737,188.664844,241.919513,125.386420,153.791075,149.670614,3180.827262,23.987366,41.932890,85.273218,44.293291,55.793819,115.615535,96.138436,98.889871,63.149674,113.537507,90.038883,57.874027,365.988394,105.474120,19.101089,25.312028,49.286471,69.028913,94.601796,278.989319,143.788978,178.670241,260.106954,252.097583,208.018876};
double weights[3] = {0.236509,0.482258,0.281233};

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

//	Init_UART2(115200); // set baudrate
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
