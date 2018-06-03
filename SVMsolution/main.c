#include "DSP_Config.h"
#include <stdio.h>
#include "fft.h"
#include "libmfcc.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

//#define PI 3.141592
#define BUFFERSIZE 256
int M=BUFFERSIZE;

int kk=0;
int startflag = 0;
int begining = 1;

volatile union {
	Uint16 sh;
	Uint8 i8[2];
} UARTout;


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
double mfcc_result[13]={0};
float llh;


volatile union {
	double sh;
	Uint8 i8[8];
} UARTdouble;

double wt[3][13]={{0.0123,    0.3516,   -0.0964,   -0.2464,    0.0458,   -0.4829,   -0.0599,    0.1076,    0.0225,   -0.0436,    0.2005, -0.0148,    0.1955},
		          {-0.2204,    0.3488,    0.1646,   -0.5489,   -0.4747,   -0.6662,   -0.2254,   -0.5297,   -0.4109,   -0.5412,    0.2984, 0.0130,    0.4316},
		          {-0.1865,  -0.3488,   -0.1913,    0.0667,    0.1214,    0.0627,    0.1729,    0.0763,   -0.1632,    0.0974,    0.0260, 0.1302,   -0.1830}};


double beta[3]={-1.9239, 2.0448, 6.5058};
double tempgx[3] = {0,0,0};
double tempmax = 0;
int dd;
int label = -1;

int main()
{
  int N = 1;
  int D = 13;

  DSP_Init();

  Init_UART2(115200);

  //ReadParameters(params, 468); // receive SVM parameters from Matlab through UART

  int ii, mm, bb, ll;
  int cc = 0;
  int label = 0;

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
          if(magnitude>30000){
        	  // N-blocks
        	  for(bb=0;bb<N;bb++){
        		  for(ll=0; ll<M; ll++){
        			  B[ll].real = X[10*bb+ll];
        			  B[ll].imag = 0;
        		  }
				  fft(B, M, w);

				  // Features for SVM : MFCC
				  for(mm=0; mm<M; mm++)
					   spectrum[mm] = sqrt(B[mm].real*B[mm].real + B[mm].imag*B[mm].imag);
				  for(coeff = 0; coeff < D; coeff++){
				  	  mfcc_result[coeff] = GetCoefficient(spectrum, 12000, 48, M, coeff);
				  }
        	  }
        	  	label = -1;
        	  	tempmax = 0;
				for(cc=0;cc<3;cc++){
					tempgx[cc]= 0;
					for(dd=0;dd<D;dd++){
						tempgx[cc] = tempgx[cc] + mfcc_result[dd]*wt[cc][dd];
					}
					tempgx[cc] = tempgx[cc] + beta[cc];
					if(tempmax<tempgx[cc]){
						label = cc;
						tempmax = tempgx[cc];
					}
				}
				printf("label: %d, gx0: %lf, gx1: %lf, gx2: %lf \n",label, tempgx[0], tempgx[1], tempgx[2]);
          }
        startflag = 0;
      }
  }
}
