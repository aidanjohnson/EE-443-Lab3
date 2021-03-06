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

double means[39] = {98.544516,-15.821519,-4.013999,-9.174635,-1.090104,-0.867722,-6.021943,-5.387161,2.367495,3.687933,0.570331,-0.370824,3.208517,-19.733692,-30.282670,-28.114051,13.968482,-4.224699,-3.094450,2.701978,7.954160,-12.777455,4.207125,3.581064,15.874056,-9.545329,-22.546771,0.997186,-3.219962,-14.764423,-5.620561,-25.440829,-8.680617,-10.950428,-20.574510,-9.647099,8.750492,2.213107,5.819828};
double covars[39] = {16.303888,3.357701,52.822133,18.383158,49.500857,62.802100,27.843582,51.307410,29.292119,35.129271,42.861519,42.321541,25.379295,490.515334,20.772607,42.920840,44.148868,22.836740,59.983323,53.363405,87.568618,68.475421,50.039098,179.698237,79.170835,161.996366,32.775088,34.659736,49.236219,16.175279,36.813475,75.615089,92.097544,86.208240,80.024457,59.751583,87.747608,95.569511,53.873768};
double weights[3] = {0.358608,0.276539,0.364853};

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
