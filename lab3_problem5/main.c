#include "DSP_Config.h"
#include <stdio.h>
#include "fft.h"
#include "svm.h"
#include "libmfcc.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#define Malloc(type,n) (type *)malloc((n)*sizeof(type))

//#define PI 3.141592
#define BUFFERSIZE 256
int M=BUFFERSIZE;
#define numFilters 48

int kk=0;
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
double mfcc_result[13]={0};

// SVM parameters
struct svm_parameter param;		// set by parse_command_line
struct svm_problem prob;		// set by read_problem
struct svm_model *model;        // SVM model for training and classifications
struct svm_node *x_space;
int label[240];
int cross_validation;
int nr_fold;

void initialization();
void modelSVM(int K, int D);
void storeSVM(int index1, int index2, int param);
void read_problem(double *featureVector, int *label,int N, int D);
void do_cross_validation();

// stores 8-bit segments of double (64-bit) data
volatile union {
	double sh;
	Uint8 i8[8];
} UARTdouble;

// Predict
int max_nr_attr = 64;
int predict_probability=0;
struct svm_node *x;
static int (*info)(const char *fmt,...) = &printf;
void predict(double *featureVector, int N, int D);

int main()
{
  int K = 3;  // Number of Classes
  int N = 1;  // Number of Blocks
  int D = 13;  // Number of Features

  DSP_Init();

  int ii, mm, bb, ll;

  // Initialize SVM model
  initialization();
  // (P5). SVM training through UART communication with Matlab
  modelSVM(K, D);

  // Check SVM parameters
  svm_check_parameter(&prob,&param);

  if(cross_validation){
	   do_cross_validation();
  }
  else{
	   model = svm_train(&prob,&param);
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
          if(magnitude>30000){
        	  // N-blocks
        	  for(bb=0;bb<N;bb++){
        		  for(ll=0; ll<M; ll++){
        			  B[ll].real = X[ll];
        			  B[ll].imag = 0;
        		  }
        		  // (P5). FFT: B is input and output, w is twiddle factors
                  fft(B, M, w);
				  // (P5). Features for SVM: MFCC
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
			  svm_check_probability_model(model);
			  // (P5). Print SVM classification results
              predict(mfcc_result, N, D);
          }
        startflag = 0;
      }
  }
}

void do_cross_validation()
{
	int i;
	int total_correct = 0;
	double total_error = 0;
	double sumv = 0, sumy = 0, sumvv = 0, sumyy = 0, sumvy = 0;
	double *target = Malloc(double,prob.l);

	svm_cross_validation(&prob,&param,nr_fold,target);
	if(param.svm_type == EPSILON_SVR ||
	   param.svm_type == NU_SVR)
	{
		for(i=0;i<prob.l;i++)
		{
			double y = prob.y[i];
			double v = target[i];
			total_error += (v-y)*(v-y);
			sumv += v;
			sumy += y;
			sumvv += v*v;
			sumyy += y*y;
			sumvy += v*y;
		}
		printf("Cross Validation Mean squared error = %g\n",total_error/prob.l);
		printf("Cross Validation Squared correlation coefficient = %g\n",
			((prob.l*sumvy-sumv*sumy)*(prob.l*sumvy-sumv*sumy))/
			((prob.l*sumvv-sumv*sumv)*(prob.l*sumyy-sumy*sumy))
			);
	}
	else
	{
		for(i=0;i<prob.l;i++)
			if(target[i] == prob.y[i])
				++total_correct;
		printf("Cross Validation Accuracy = %g%%\n",100.0*total_correct/prob.l);
	}
	free(target);
}

void initialization()
{
	// default values
	param.svm_type = C_SVC;
	param.kernel_type = RBF;
	param.degree = 3;
	param.gamma = 0;	// 1/num_features
	param.coef0 = 0;
	param.nu = 0.5;
	param.cache_size = 100;
	param.C = 1;
	param.eps = 1e-3;
	param.p = 0.1;
	param.shrinking = 1;
	param.probability = 0;
	param.nr_weight = 0;
	param.weight_label = NULL;
	param.weight = NULL;
	cross_validation = 0;
}

void storeSVM(int index1, int index2, int param)
{
	// UART input
	int iter = 0;
	while (iter < 8) {
		if(IsDataReady_UART2()){
			UARTdouble.i8[iter++] = Read_UART2();
			if(iter==8){
				if (param == 1) {
					model->nr_class = UARTdouble.sh;
				} else if (param == 2) {
					model->nSV[index1] = UARTdouble.sh;
				} else if (param == 3) {
					struct svm_node svm;
					svm.index = index1;
                    svm.value = UARTdouble.sh;
                    model->SV[index2] = &svm;
				} else if (param == 4) {
					model->sv_coef[index1][index2] = UARTdouble.sh;
				} else if (param == 5) {
					model->rho[index1] = UARTdouble.sh;
				}
            }
			while(IsTxReady_UART2()==0) ;
			Write_UART2(1);
			wait(10000);
		}
	}
}

void modelSVM(int K, int D) 
{
	// Get SVM model parameters
    storeSVM(0, 0, 1); // number of classes
	int i;
	for (i = 0; i < model->nr_class; i++) {
		storeSVM(i, 0, 2); // number of support vectors
	}
    int class;
	for(class = 0; class < model->nr_class; class++) {
        int sv;
        for(sv = 0; sv < model->nSV[class]; sv++) {
            int j;
            for(j = 0; j < D; j++) {
                storeSVM(class, sv*D+j, 3); // support vectors
            }
        }
	}
	for(class = 0; class < model->nr_class; class++) {
        int sv;
        for(sv = 0; sv < model->nSV[class]; sv++) {
            storeSVM(class, sv, 4); // alpha sv coefficients
        }
	}
    for(class = 0; class < model->nr_class; class++) {
        storeSVM(class, 0, 5); // rho bias
    }
}

// read in a problem (in svmlight format)
void read_problem(double *featureVector, int *label, int N, int D)
{
	int i,j;

	prob.l = N;
	prob.y = Malloc(double,prob.l);
	prob.x = Malloc(struct svm_node *,prob.l);
	x_space = Malloc(struct svm_node,N);

	for(i=0;i<prob.l;i++)
	{
		prob.x[i] = &x_space[j];
		prob.y[i] = label[i];

		for(j=0;j<D;j++)
		{
			x_space[j].index = j;
			x_space[j].value = featureVector[i*D+j];
		}

		x_space[j++].index = -1;
	}

}


void predict(double *featureVector, int N, int D)
{
	int correct = 0;
	int total = 0;
	double error = 0;
	double sump = 0, sumt = 0, sumpp = 0, sumtt = 0, sumpt = 0;

	int svm_type=svm_get_svm_type(model);
	int nr_class=svm_get_nr_class(model);
	double *prob_estimates=NULL;
	int i,j;

	if(predict_probability)
	{
		if (svm_type==NU_SVR || svm_type==EPSILON_SVR)
			info("Prob. model for test data: target value = predicted value + z,\nz: Laplace distribution e^(-|z|/sigma)/(2sigma),sigma=%g\n",svm_get_svr_probability(model));
		else
		{
			int *labels=(int *) malloc(nr_class*sizeof(int));
			svm_get_labels(model,labels);
			prob_estimates = (double *) malloc(nr_class*sizeof(double));
			free(labels);
		}
	}

	int max_line_len = 1024;
	for(i=0;i<N;i++)
	{
		double target_label, predict_label;

		for(j=0;j<D;j++)
		{
			if(i>=max_nr_attr-1)	// need one more for index = -1
			{
				max_nr_attr *= 2;
				x = (struct svm_node *) realloc(x,max_nr_attr*sizeof(struct svm_node));
			}

			x[i].index = i;
			x[i].value = featureVector[i*D+j];

			++i;
		}
		x[i].index = -1;

		if (predict_probability && (svm_type==C_SVC || svm_type==NU_SVC))
		{
			predict_label = svm_predict_probability(model,x,prob_estimates);
		}
		else
		{
			predict_label = svm_predict(model,x);
		}

		if(predict_label == target_label)
			++correct;
		error += (predict_label-target_label)*(predict_label-target_label);
		sump += predict_label;
		sumt += target_label;
		sumpp += predict_label*predict_label;
		sumtt += target_label*target_label;
		sumpt += predict_label*target_label;
		++total;
	}
	if (svm_type==NU_SVR || svm_type==EPSILON_SVR)
	{
		info("Mean squared error = %g (regression)\n",error/total);
		info("Squared correlation coefficient = %g (regression)\n",
			((total*sumpt-sump*sumt)*(total*sumpt-sump*sumt))/
			((total*sumpp-sump*sump)*(total*sumtt-sumt*sumt))
			);
	}
	else
		info("Accuracy = %g%% (%d/%d) (classification)\n",
			(double)correct/total*100,correct,total);
	if(predict_probability)
		free(prob_estimates);
}

