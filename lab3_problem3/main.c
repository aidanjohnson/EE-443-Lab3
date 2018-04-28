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

double means[39] = {-29.147305,2.333083,4.253114,-5.730461,3.536479,-9.752722,-13.476314,-13.115708,-14.058496,-3.277280,-13.741680,-8.661314,-4.201054,-18.928495,4.440490,2.399173,-6.951114,4.250677,-8.653768,-11.344707,-18.477967,-35.938350,-29.125249,-36.306207,-4.384843,9.702717,38.477449,-20.081126,-1.102902,-3.707494,-0.930238,-5.654143,-5.462575,-0.515065,5.629462,-1.183046,-14.461311,14.216568,-5.295018};
double covars[39] = {72.534609,71.120907,52.218530,42.486786,40.054461,105.881755,188.264920,158.162992,159.453189,286.205628,293.478764,160.137148,123.213193,14.264268,15.301704,12.269610,17.519772,25.138029,38.668954,51.361851,49.853568,58.517800,252.955730,63.974677,137.484032,100.245113,3694.054802,168.098306,49.984659,78.656424,51.911313,40.066056,113.681187,158.086526,161.533424,51.320804,251.009289,345.363670,112.882242};
double weights[3] = {0.238312,0.132783,0.628904};

// stores 8-bit segments of double (64-bit) data
volatile union {
	double sh;
	Uint8 i8[8];
} UARTdouble;

// comment out second header for UART, first for hard coded means, covars, weights
void storeGMM(double *parameters, int paramsize)
//void storeGMM(int index, int param)
{
	int ii = 0;
	int iter = 0;
    Uint8 temp = 0;
	while(ii<paramsize){
		if(IsDataReady_UART2()){
            temp = Read_UART2();
            UARTdouble.i8[iter++] = temp;
            while(IsTxReady_UART2()==0) ;
            Write_UART2(1);

            if(iter>7){
                iter = 0;
                parameters[ii++] = UARTdouble.sh;
                printf("Index: %d, Received double: %lf \n", ii, UARTdouble.sh);
            }
		}
	}
	// uncomment for hard coded means, covars, weights; comment for UART
//	if (param == 1) {
//		gmm[0].means[index] = means[index];
//	} else if (param == 2) {
//		gmm[0].weights[index] = weights[index];
//	} else if (param == 3) {
//		gmm[0].covars[index] = covars[index];
//	}

}

void modelGM(int K, int D)
{
	// comment out for hard coded means, covars, weights; uncomment for UART
	storeGMM(gmm[0].means, D*K);
	storeGMM(gmm[0].weights, K);
	storeGMM(gmm[0].covars, D*K);

	// uncomment for hard coded means, covars, weights; comment for UART
	// Get GM model mean, weights, variances
//	int i;
//	for (i = 0; i < D*K; i++) {
//		storeGMM(i, 1);
//	}
//	int j;
//	for (j = 0; j < K; j++) {
//		storeGMM(j, 2);
//	}
//	int k;
//	for (k = 0; k < D*K; k++) {
//		storeGMM(k, 3);
//	}
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
