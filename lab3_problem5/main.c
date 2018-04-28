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

double params[441] = {3.000000,11.000000,9.000000,11.000000,42.689744,-4.925129,2.683695,-0.732025,0.594174,7.722543,9.893335,-1.545915,10.555778,2.878253,5.016590,-4.776792,-5.592062,55.447001,-7.430793,-4.973519,1.647004,5.149983,3.651909,7.079610,3.474823,2.477834,-2.620167,7.481508,-4.834300,-7.600662,22.534819,-17.631078,-14.651854,-0.966481,-4.552142,-9.052660,7.960354,-4.435809,-2.441721,0.427103,-7.649863,11.043073,8.242145,-0.917451,-15.639568,2.369268,-7.100641,3.310396,-0.924353,-5.070913,-9.975924,-3.004441,5.826416,5.827535,0.008943,0.798836,43.115670,-5.995259,-10.827143,-3.538015,1.573406,3.404820,1.629863,14.892055,-5.743216,-0.976269,9.979384,-7.697255,-4.107828,13.560622,-20.131427,-8.348898,-5.559880,-3.472879,-5.318787,-0.562494,-1.586370,3.316030,2.789695,3.480190,4.203899,8.166274,67.488669,-18.366028,-17.189057,8.325668,3.512145,-0.407155,-1.246810,9.920994,-14.468904,-3.356789,11.525077,9.413535,-6.153624,25.503586,-15.153546,-12.199172,0.506071,-9.559015,-9.936611,8.794907,-0.446624,0.481280,7.452519,-4.015473,6.848727,6.881228,22.993844,-14.768799,-11.615593,-2.001894,-8.185478,-10.848632,-0.133473,-5.544713,-2.861027,-2.382973,-9.957265,0.457960,4.931105,45.618615,-7.550606,0.419255,-2.886301,2.816955,-1.096374,4.335590,1.612336,0.311183,-0.476936,6.632837,2.257757,0.242514,48.975166,-7.818267,-4.518216,-8.544079,0.055544,-7.147759,6.829052,3.302326,-2.448039,-6.476252,0.450639,-5.228710,-3.150699,66.751789,-10.343100,-0.256802,-1.359878,-0.951402,-1.939464,-2.662418,-3.636753,-1.744705,0.529180,3.022652,-0.193400,-2.063297,67.335363,-11.570350,-3.207685,-7.237195,-3.066935,1.592535,-1.339740,-4.018548,-2.989025,1.553327,3.516225,-1.392387,-4.729765,67.902509,-9.448048,0.385804,-0.673577,1.145646,1.792051,0.692716,5.061337,2.902559,-1.894828,-2.159867,-1.648699,-2.398363,67.919741,-9.132549,1.668651,-2.751158,-0.078643,0.638813,4.743757,3.172687,-2.387512,0.472109,5.968069,5.083468,1.795646,42.689744,-4.925129,2.683695,-0.732025,0.594174,7.722543,9.893335,-1.545915,10.555778,2.878253,5.016590,-4.776792,-5.592062,55.447001,-7.430793,-4.973519,1.647004,5.149983,3.651909,7.079610,3.474823,2.477834,-2.620167,7.481508,-4.834300,-7.600662,66.073466,-19.880868,-14.280042,10.040868,-1.352957,2.237388,-10.376656,7.737990,-8.841165,-3.823595,10.264804,0.153443,-7.736230,62.054692,-20.614401,-22.851187,6.701931,-1.294597,-3.778756,3.603985,3.080378,-10.297696,-4.786425,-1.910272,10.928279,-2.490434,60.819901,-20.895820,-22.137303,7.082443,-4.261263,-5.067659,7.558490,4.596809,1.927718,-2.690210,-5.329047,12.208032,-5.947871,64.803191,-10.646012,-2.532170,-2.982933,-1.226292,-5.125975,-2.400175,-4.360283,-0.542071,0.097982,-1.713324,-1.246574,0.941051,66.096028,-9.758873,-0.847206,-4.837430,-3.174488,-3.547148,-3.468569,-0.494857,-3.121877,0.799978,3.832955,1.488897,1.211653,67.420820,-9.452408,-1.396706,-4.579541,-3.281516,-6.354301,-1.759008,-1.389491,0.410646,-1.989903,5.656356,5.050135,6.821326,67.747402,-9.359236,-0.245644,-5.258031,-2.151010,-5.437494,-1.243887,-3.210579,-6.129668,-6.200579,-4.417367,-0.293875,-0.179084,63.728699,7.539286,-6.039934,-13.848672,-10.029237,-18.930043,-9.978688,-12.798241,-9.613418,1.666786,16.614721,16.305436,10.547952,65.050082,4.017156,-9.981614,-6.882490,-8.939317,-24.984235,-7.958783,-10.095117,-14.674347,1.478802,19.640817,13.097217,12.513211,63.531186,3.228720,-13.973654,-9.717973,-5.196521,-19.564740,-6.694001,-12.013478,-21.338057,-11.229564,-6.858648,-6.395657,-1.717159,49.518004,-8.033527,-4.823153,-7.416937,2.418529,-3.250918,11.454176,8.288409,-1.221057,0.896879,8.449102,2.529640,-0.401885,50.032915,-6.855014,-3.004253,-7.069158,2.570885,-3.332695,7.219042,4.077256,-3.895713,-1.672933,2.191718,-2.272454,3.344783,50.233804,-4.831600,0.656911,-3.905536,4.975146,-4.746961,-0.990363,-5.102998,-6.176290,-7.486385,-0.813013,-2.919845,2.537899,49.568191,-4.413873,0.334336,-6.924561,2.239654,-6.513355,1.136508,5.099513,1.775807,2.172273,1.497122,-1.032719,3.224931,0.002282,0.012513,0.001473,0.001148,0.000529,0.001065,0.000984,0.000204,0.001716,0.017216,0.004699,0.000632,0.001567,0.003893,0.000611,0.000459,0.003996,0.001557,0.000079,0.000614,0.003028,0.002178,0.001370,0.000712,0.000738,0.000290,0.000888,0.000395,0.000256,0.001669,0.003054,1.224714,3.386436,5.306891};

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
    svm_read_model(params, 441, param);

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

