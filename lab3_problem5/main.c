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
//void storeSVM(int index1, int index2, int param);
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

// 1: Bluejay, 2: Dove, 3: Duck
double params[315] = {3.000000,11.000000,5.000000,6.000000,-65.829106,-9.665810,6.153694,-0.218610,-5.776602,2.106955,4.566502,-8.281994,9.398335,-4.282913,3.665591,-0.266174,3.194216,-41.375249,-6.639517,5.601669,4.028826,4.833218,5.051772,14.341838,3.829979,17.051776,2.893184,4.116108,-11.977712,-7.134808,-16.706092,-11.181535,-2.845474,2.894310,8.196255,7.698533,13.775479,1.204401,6.472611,-4.800524,11.140062,-4.283053,-13.042894,-41.158139,-8.361417,-11.339919,-6.045949,-0.251613,7.542158,7.099592,18.809824,-12.379735,-8.531726,21.952025,-14.022475,-6.719045,-0.510650,-27.172310,-23.743737,14.102000,-4.520843,2.868487,3.441899,1.081719,-35.339765,3.343020,8.355735,9.285872,-21.266151,-81.196629,-33.073509,-16.911083,-6.895406,2.413460,-9.872505,4.341071,-13.204426,-12.807570,-6.700658,-3.305710,-0.013614,2.892064,-107.867077,-29.682570,-6.878822,-8.179896,-4.256162,-5.694923,-3.483033,-3.083377,-1.618417,1.700249,4.350387,7.553151,5.097610,-50.641826,-24.092910,-25.446264,9.741331,0.265530,-10.102542,5.119797,-7.459389,-7.552222,10.798159,-22.757993,19.058396,23.181435,-60.617031,-14.021675,-2.722119,-7.052471,9.004108,3.587177,-3.884257,-27.991754,-10.232066,-16.414887,-19.725168,-23.260170,-21.116592,-40.396753,-11.527437,2.161142,-3.108671,5.407871,1.218136,8.554495,0.807229,1.647230,1.683031,10.137166,3.696399,-4.820458,-30.967781,-12.747554,-8.177038,-11.776758,5.729036,-2.657039,20.547255,15.155654,0.653602,-0.349027,11.182327,5.648796,-1.527572,92.116780,-20.678337,-1.420805,-6.911345,-2.543843,-5.427101,-7.351667,-5.430650,1.976882,4.014519,1.959226,11.387634,-2.078462,93.640936,-16.070683,-2.136787,-8.174457,-5.898239,-5.637287,-5.978742,-0.814093,-4.826625,2.189691,8.902860,3.810477,2.985506,94.745478,-16.202428,0.293536,-1.623834,-0.665966,-6.136404,-9.952132,-11.305806,-9.186637,-7.807056,3.219606,6.482687,-0.422554,-1.227499,-27.225071,-22.765168,15.062441,-1.048463,-0.045454,2.525673,0.005546,-15.816602,-2.206216,5.384973,23.367397,-19.914791,2.658329,-28.176922,-24.658675,15.210075,0.228484,3.649591,-9.727719,11.137588,-19.696520,-2.775009,15.838715,11.393136,-16.570833,91.683058,-17.705801,-4.341763,-5.506691,-2.372362,-9.163298,-3.800414,-7.025073,-0.445648,0.187621,-2.496996,-2.192022,4.208384,93.640936,-16.070683,-2.136787,-8.174457,-5.898239,-5.637287,-5.978742,-0.814093,-4.826625,2.189691,8.902860,3.810477,2.985506,-11.102008,3.677210,-14.611706,-8.675806,-12.603719,-30.091680,-7.861101,-5.356623,-13.694594,6.981622,33.143338,21.384173,12.247091,-12.447303,4.252150,-19.306297,-16.950667,-7.810214,-31.418994,-5.311872,-6.890987,-28.671661,-13.142975,1.006019,-5.320192,4.716557,-12.084543,2.082824,-21.373028,-10.760226,-1.827935,-30.998044,-2.130153,-9.770323,-31.684985,-15.932125,1.844835,-5.202901,9.554037,-28.102755,-5.663546,-4.944116,-15.441889,1.107408,-7.952443,10.346169,17.744801,0.506653,2.746986,8.209552,-11.536291,-7.271000,0.005092,0.000326,0.012137,0.000449,0.000733,0.001451,0.002752,0.000125,0.003122,0.018243,0.001700,0.000188,0.000001,0.000004,0.000047,0.000147,0.000093,0.000064,0.000071,0.000031,0.000048,0.000007,-1.712286,0.625583,0.515424};

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

//void storeSVM(int index1, int index2, int param)
//{
//	// UART input
//	int iter = 0;
//	while (iter < 8) {
//		if(IsDataReady_UART2()){
//			UARTdouble.i8[iter++] = Read_UART2();
//			if(iter==8){
//				if (param == 1) {
//					model->nr_class = UARTdouble.sh;
//				} else if (param == 2) {
//					model->nSV[index1] = UARTdouble.sh;
//				} else if (param == 3) {
//					struct svm_node svm;
//					svm.index = index1;
//                    svm.value = UARTdouble.sh;
//                    model->SV[index2] = &svm;
//				} else if (param == 4) {
//					model->sv_coef[index1][index2] = UARTdouble.sh;
//				} else if (param == 5) {
//					model->rho[index1] = UARTdouble.sh;
//				}
//                printf("Index: %d, Received double: %lf \n", ii, UARTdouble.sh);
//            }
//			while(IsTxReady_UART2()==0) ;
//			Write_UART2(1);
//			wait(10000);
//		}
//	}
//}

void modelSVM(int K, int D) 
{
	// Get SVM model parameters
    svm_read_model(params, 315, &param);

    // uncomment for UART transfer
//	// Get SVM model parameters
//    storeSVM(0, 0, 1); // number of classes
//	int i;
//	for (i = 0; i < model->nr_class; i++) {
//		storeSVM(i, 0, 2); // number of support vectors
//	}
//    int class;
//	for(class = 0; class < model->nr_class; class++) {
//        int sv;
//        for(sv = 0; sv < model->nSV[class]; sv++) {
//            int j;
//            for(j = 0; j < D; j++) {
//                storeSVM(class, sv*D+j, 3); // support vectors
//            }
//        }
//	}
//	for(class = 0; class < model->nr_class; class++) {
//        int sv;
//        for(sv = 0; sv < model->nSV[class]; sv++) {
//            storeSVM(class, sv, 4); // alpha sv coefficients
//        }
//	}
//    for(class = 0; class < model->nr_class; class++) {
//        storeSVM(class, 0, 5); // rho bias
//    }
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

