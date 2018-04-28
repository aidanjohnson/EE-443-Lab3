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

double params[357] = {3.000000,11.000000,8.000000,6.000000,-75.555445,-2.181172,1.628606,-4.841924,3.023439,-4.607048,7.382408,-11.429259,8.504989,-8.236018,-4.133523,-4.623875,0.025013,-25.661493,-11.683908,-1.587598,-0.316452,9.375475,0.117418,7.826971,15.573061,-12.770978,-1.040792,-21.907760,-0.769306,0.479163,-77.985543,-28.035388,-6.914472,-15.658620,-2.644190,-7.060340,-16.605613,-26.510957,13.621668,-2.849405,-15.092910,-4.434003,6.558828,-46.715958,-12.409958,-8.039585,-11.194694,13.253397,0.003223,-2.484685,17.955966,-9.337057,-0.452845,-16.041633,8.034262,3.989896,-69.923614,-10.073994,-0.416790,13.889497,14.824390,-3.977279,24.310114,-9.157143,-15.684653,-30.366240,-39.625657,-25.942037,27.760116,-49.897507,-7.775416,-1.292283,-4.146798,4.709361,-4.905027,-4.865891,7.556050,-0.160443,7.281813,-3.199132,-2.643903,1.545945,-37.331806,-6.148679,1.785612,0.121780,12.918308,8.563888,13.740396,-14.064474,-26.453881,-6.947640,-35.422086,-14.428128,-24.904423,-38.356421,-13.331293,-2.289261,-11.064631,-0.470405,-1.993688,-3.683895,-2.634874,-0.575676,-14.632719,-17.743339,-0.359106,-11.016234,-38.645874,-10.055175,-5.349084,-10.231935,14.960246,-9.409652,-0.259824,-1.979786,-7.343352,16.896393,-2.784403,20.890552,-14.396325,-37.114133,-10.853176,-3.949784,-7.723958,6.518584,6.213844,4.909360,4.671948,-3.471275,3.045031,-3.127006,-8.411133,-11.501253,-39.349953,-6.262179,-5.192193,-12.713848,2.649591,2.096855,4.358943,10.402583,4.378986,-11.611325,-20.612683,3.742193,-1.594344,85.012537,-13.426771,-3.126439,-5.904325,-8.945612,-8.388052,-1.417031,-0.276477,-13.367714,-8.739102,-14.210151,7.163084,11.981542,85.120276,-9.618585,2.987706,-0.303204,1.464268,-9.398314,-1.213639,3.579422,4.266279,-13.904469,-12.320983,-9.207343,-5.635213,86.142302,-10.852593,-6.328036,-1.111560,2.934221,-8.905255,2.931233,11.414417,-11.203805,-3.754832,6.554278,14.746229,5.735846,85.519201,-9.528077,3.880815,-4.094933,3.012688,1.322623,7.892388,2.928708,5.163047,9.115726,-10.850113,-2.291614,-8.407840,84.241816,-10.993371,0.637769,-0.876217,1.618549,-4.889566,-3.208839,-8.198171,4.960387,6.987934,-6.167035,-1.693467,-7.842942,86.482916,-8.087772,0.800694,-9.424843,-5.504970,-19.365090,-11.705911,-13.502257,-9.211643,-4.245828,-9.294663,22.980212,-6.618235,-11.002854,-32.957033,-6.522711,-0.832290,13.690733,-10.071546,-5.355900,16.888686,-3.771105,-1.477973,-29.228501,21.197191,-0.516548,-7.436570,-34.045516,7.085633,-6.443996,5.823674,-8.131208,-22.389974,22.524190,4.667028,-0.263483,-23.157057,35.442278,-20.241257,84.191051,-8.717844,4.867093,7.288897,-0.646484,-7.364539,-19.015409,-9.852919,-7.976606,-17.967354,2.155226,3.854940,1.674492,84.996364,-8.395446,1.156202,2.598261,1.512968,-5.851464,-16.125752,-13.054825,-12.540838,-6.172530,-18.779412,-6.825858,-8.350469,84.006217,-10.445136,-1.630010,-8.196672,-4.161905,-5.258070,-6.586248,-5.110268,-22.587033,-14.401134,-0.138755,1.438843,-0.929434,86.384115,-6.687362,-4.233856,-13.236070,-8.122984,-19.542411,-16.033617,-8.041540,-2.834525,-8.321972,-7.376899,-10.767850,10.161533,-6.874019,0.175693,-1.146536,-9.804842,-1.650892,-32.681232,-49.419870,-22.551330,-26.429233,26.302688,22.987411,0.460369,-12.357255,-5.773478,-7.670489,-2.096233,-5.300070,12.857262,-4.895423,-21.828407,-12.446111,-30.124183,-51.267431,-32.171257,-7.265751,22.386135,0.107031,0.154206,0.022030,0.036929,0.055410,0.101932,0.004404,0.005612,0.047175,0.021463,0.084200,0.000084,0.000008,0.000028,0.000047,0.000002,0.000013,0.000160,0.000022,0.000079,0.000050,0.000071,0.000016,0.000093,0.000122,-12.147289,0.409536,0.517498};

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
                printf("Index: %d, Received double: %lf \n", ii, UARTdouble.sh);
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
    svm_read_model(params, 357, param);

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

