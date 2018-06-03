#include "DSP_Config.h"
#include <stdio.h>
#include "fft.h"
#include "gmm.h"
#include "libmfcc.h"
#include <math.h>

//#define PI 3.141592
#define BUFFERSIZE 256
int M=BUFFERSIZE;
int kk=0;
int startflag = 0;
int training = 1;

short X[BUFFERSIZE];
COMPLEX w[BUFFERSIZE];
COMPLEX B[BUFFERSIZE];
double spectralData[BUFFERSIZE];
float feature;
float denominator = 0;
float numerator = 0;
float magnitude = 0;
float avg = 0;

int coeff=0;
double spectrum[BUFFERSIZE];
double mfcc_result[13]={0};
float llh;

double weights[3] = {0.3527,0.3679,0.2794};

double means[39] = {23.9046,-3.8032,-2.9450,-0.0319,2.0933,-1.1712,1.3214,1.4598,-1.5932,0.1862,0.5991,1.5942,-1.5857,29.0979,0.7390,-4.2447,-4.5347,4.3583,-0.4218,2.3313,-1.0113,-0.3600,0.6380,-0.0538,-0.4462,0.1246,28.5412,7.2086,3.0393,-1.9655,0.4203,-2.6370,-0.1741,-1.9910,-0.4227,-1.3651,0.4517,-0.4794,0.7598};
double covars[39] = {3.2719,1.7614,0.5599,0.2874,0.3905,0.4638,0.3624,0.1976,0.1423,0.2358,0.1900,0.2342,0.1399,45.9504,2.6670,1.3884,0.7824,1.0076,0.7047,0.5232,0.2775,0.2915,0.2510,0.1590,0.1608,0.1831,26.2940,2.6459,0.4981,0.3032,0.8626,0.3012,0.2847,0.0818,0.0939,0.0912,0.1585,0.0828,0.1371};
GMM gmm[1]; // create GMM model

double dove[13] = {32.9163,6.4401,2.8580,-1.8951,-0.1993,-2.1098,-0.0232,-1.7740,-0.5665,-1.7042,0.0851,-0.3963,0.9659};
double blue[13] = {23.1255,-3.6244,-3.2140,-0.5849,1.8573,-1.2209,1.3021,1.1458,-1.4594,-0.1276,-0.2934,1.9484,-0.9905};
double duck[13] = {20.4756,1.8432,-3.8635,-4.3971,5.1733,0.2892,3.0182,-0.4227,0.4812,0.9375,0.1434,-0.0751,-0.0890};

int main()
{
	int K = 3; // Number of Classes
	int N = 1; // Number of Blocks
	int D = 13; // Number of Features

	DSP_Init();

	int ii, mm, bb, ll;

	// Initialize GMM model
	gmm_new(gmm, K, D, "diagonal");
	gmm_set_convergence_tol(gmm, 1e-6);
	gmm_set_regularization_value(gmm, 1e-6);
	gmm_set_initialization_method(gmm, "random");

	for (ii=0; ii<3; ii++) {
		gmm->weights[ii] = weights[ii];
	}
	for (ii=0; ii<39; ii++) {
		gmm->means[ii] = means[ii];
	}
	for (ii=0; ii<39; ii++) {
		gmm->covars[ii] = covars[ii];
	}
	printf("model saved \n");

	// Twiddle factor
	for(ii=0; ii<M; ii++) {
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
			if(magnitude>30000){
				// N-blocks
				for(bb=0;bb<N;bb++) {
					for(ll=0; ll<M; ll++){
						B[ll].real = X[ll]-avg;
						B[ll].imag = 0;
						//printf("%lf\n",B[ll].real);
					}
					// (P3). FFT: B is input and output, w is twiddle factors
					fft(B, M, w);

					for (ii = 0; ii < BUFFERSIZE; ii++) {
						spectralData[ii] = sqrt(B[ii].real*B[ii].real+B[ii].imag*B[ii].imag);
					}

					// (P3). Find 13 MFCC coefficients
					for (ii = 0; ii < 13; ii++) {
						mfcc_result[ii] = GetCoefficient(spectralData, 12000, 48, BUFFERSIZE, ii);
					}
				}
				llh = gmm_score(gmm, mfcc_result, 1);
			}
			startflag = 0;
		}
	}
}
