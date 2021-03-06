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
#define BUFFERSIZE 128
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

// SVM parameters
struct svm_parameter param;		// set by parse_command_line
struct svm_problem prob;		// set by read_problem
struct svm_model *model;
struct svm_node *x_space;
int label[1];
int cross_validation=0;
int nr_fold;

void initialization();
void read_problem(double *featureVector, int *label,int N, int D);
void do_cross_validation();

// Predict
int max_nr_attr = 64;
int predict_probability=0;
struct svm_node x[33];
static int (*info)(const char *fmt,...) = &printf;
void predict(double *featureVector, int N, int D);

volatile union {
	double sh;
	Uint8 i8[8];
} UARTdouble;


Uint8 temp1 = 0;
Uint8 temp2 = 0;

void ReadParameters(double *parameters, int paramsize){
	int ii = 0;
	int iter = 0;
	int count = 0;
	while(ii<paramsize){
		if(IsDataReady_UART2()){
			  temp1 = Read_UART2();
			  if(temp1==temp2){
				  count++;
				  if(count>10){
					  count = 0;
					  while(IsTxReady_UART2()==0) ;
					  Write_UART2(1);
				  }
				  else{
					  while(IsTxReady_UART2()==0) ;
					  Write_UART2(0);
				  }
			  }
			  else{
				  UARTdouble.i8[iter++] = temp1;
				  //printf("Iter: %d, Received uint8: %d \n", iter, UARTdouble.i8[iter-1]);
				  while(IsTxReady_UART2()==0) ;
				  Write_UART2(0);
				  count = 0;
			  }
			  temp2 = temp1;

			  if(iter>7){
				  iter = 0;
				  parameters[ii++] = UARTdouble.sh;
				  printf("Index: %d, Received double: %lf \n", ii, UARTdouble.sh);
			  }
		}
	}
}

void ReadParameters2(double *parameters, int paramsize){
	int ii = 0;
	int iter = 0;
	while(ii<paramsize){
		if(IsDataReady_UART2()){
			  temp1 = Read_UART2();
			  UARTdouble.i8[iter++] = temp1;
			  while(IsTxReady_UART2()==0) ;
			  Write_UART2(1);

			  if(iter>7){
				  iter = 0;
				  parameters[ii++] = UARTdouble.sh;
				  printf("Index: %d, Received double: %lf \n", ii, UARTdouble.sh);
			  }
		}
	}
}

// SVM parameters (rho[3] nSV[3] sv_coef[sum(nSV)] SV[13*sum(nSV)]), please use your own parameters
double params[468]={-2.80152041552964,	15.9759436273019,	4.23889398935636,	11,	13,	9,	0.00241101215109686,	0.00646825049243821,	0.00192506083821236,	0.000115569279657950,	0.000389749320950706,	0.00110759804688888,	4.32092332415133e-05,	-0.00275129633734960,	-0.00684328874288369,	-0.000350471955847967,	-0.00251539232640522,	0.00298174655392685,	0.00232657538472842,	0.00466638308274537,	0.0186708572345135,	0.00397050280625017,	0.00858340152878755,	0.00278782695290103,	0.00129856429555316,	0.0136225276419000,	-0.0313921415589269,	-0.000611270492442601,	-0.00936169879207132,	-0.0175432746378651,	0.000675482583243345,	0.00106363401248709,	0.00381509066749728,	3.73985070976974e-05,	-0.000964197506656628,	-0.000121241394423330,	-0.00113465474568179,	-0.000823473316805908,	-0.00254803880675775,	61.5903653859420,	-13.2224694678373,	-5.06286732949259,	-9.81281851572585,	-1.72201472716901,	-4.82488330750654,	-3.56670100171457,	-6.88812937819684,	-9.48082144184155,	-5.95839542352606,	-10.3862449927213,	-0.982407345287091,	0.488661412679970,	63.4509771993505,	-10.3564043263752,	-1.04420851506771,	-7.18715224066140,	4.04346609822795,	-12.0828716335664,	-10.7624882919518,	-3.90174126274238,	-5.93988780541994,	4.34249813133367,	-4.34848034474101,	5.95962193405356,	-2.98324565303873,	64.7133731049673,	-12.7882978424622,	-3.74430376403859,	-10.6650382534571,	5.02989991993606,	-0.0847355334669195,	6.61899960431017,	0.734929696276340,	-9.16206191899107,	0.834436810183092,	0.529502904623074,	-1.57837923108935,	-13.8207621813401,	63.4543437566699,	-9.36017953455996,	-4.09951467606632,	-13.2646317261266,	8.33063636821186,	14.0207592632504,	-5.72207680398572,	-2.55288158410615,	3.77397009407302,	-3.79817492433794,	-22.0428125970470,	-0.464257626921813,	-6.18599195344283,	65.2657645609953,	-10.8845258140473,	-2.63492081971948,	-7.68833534983598,	12.3527635220414,	-4.67787290315325,	11.3142256878500,	-2.20406014363134,	5.19122885619072,	0.294885539820587,	-12.4741601898474,	-15.4251061866556,	-11.2308143897787,	64.6726289835588,	-3.58068650520658,	5.42726255105996,	3.91470712515250,	14.5732156052623,	4.55389694180087,	3.17150114757175,	-17.2654802233939,	-13.5204248666267,	-15.4632772006652,	-39.8848894796983,	-18.7707608047663,	-13.4353719986573,	68.6001311130567,	-7.16829007999645,	-8.62173801635298,	-17.4244045168329,	1.53422237928780,	2.88632635034174,	16.8055507640638,	11.2520151074302,	10.0093317752241,	17.5823246419358,	-18.7421275978168,	-11.1394434814478,	-2.94978744424598,	52.9669566580346,	-6.07229269303296,	-5.08855892067536,	-0.679754112753236,	10.4627679753587,	-7.05310785710277,	11.5845448558631,	6.04315783014072,	0.405880450525209,	-2.73882063707849,	-0.185903162565902,	7.93482455694736,	-0.421874218626066,	77.9271694832640,	-22.6004161155616,	0.985455017947356,	0.460532291017452,	5.14354582967000,	-8.08046359457380,	-3.01000596500417,	8.45483897361599,	-6.62173288722606,	8.86270909894305,	-14.1150431658979,	6.73272516779730,	-3.24400586283571,	50.8629590325315,	-27.4833894815062,	7.54643680698086,	-14.4111315481038,	-2.22446473400624,	-4.29432921448862,	-8.55858608928802,	-26.3798754981264,	24.9683225184879,	-3.89680739440767,	-15.0136040347639,	18.3932577928624,	-11.9253570419640,	12.2171834757496,	-16.2508301339204,	-0.719538526652396,	-4.88123946875573,	0.497940405144674,	-2.06585261281309,	-11.4680188232686,	-33.4603738338700,	-19.7847205253404,	-17.6306926939878,	-14.1156090770249,	-7.30556670467077,	-10.1406632405460,	63.3082201788168,	-9.91539829136430,	-7.57946923180157,	-15.8533235779672,	2.68523703437428,	-4.02685674624502,	1.17278715238798,	0.295676499267712,	7.60270112542132,	-11.2739274724909,	12.1453750915565,	0.969965281251907,	-4.59522242569622,	74.4467437578845,	7.05973215028241,	9.11940140958739,	4.54535574776446,	10.0195900321832,	-15.3436483849611,	4.93793585344862,	-5.66588835613373,	-3.71136604629395,	-19.3498162608091,	-22.8186019223866,	-24.5314450766444,	-22.6307303972942,	73.7111071184166,	9.87973177859669,	15.2421585431172,	0.265646013019190,	0.974810310189014,	-6.32647726544831,	-0.723007967965827,	3.47262468681487,	8.57737513073258,	-19.9135792360926,	-23.4124307885794,	-21.6157171346250,	-4.32615481461436,	96.0423812997089,	-6.28592718036991,	-3.13085864464165,	-5.72967433700041,	2.87952589380991,	-24.5589384518103,	-45.2544453587234,	-26.8719491797628,	-43.6952225463672,	6.06315229118423,	39.1867785616914,	5.93017977291670,	-24.0675887767420,	89.2097353761475,	-2.13048219055744,	5.55098385853187,	-11.1964356449040,	-6.11454874914046,	-33.9381882702258,	-39.1830689951566,	-16.6288892136634,	6.32510507367900,	45.2416809512345,	16.9564743802810,	-0.876019528542128,	25.0467840600814,	91.8773342405755,	-4.85698010342097,	1.17005580122193,	-8.69982608178751,	10.9852353382567,	-2.93290747325420,	-9.75241835644308,	-19.3271771242053,	-28.5776343001166,	-56.7620655134515,	-43.1180941705906,	1.77858814619633,	10.6269143698445,	65.2657645609953,	-10.8845258140473,	-2.63492081971948,	-7.68833534983598,	12.3527635220414,	-4.67787290315325,	11.3142256878500,	-2.20406014363134,	5.19122885619072,	0.294885539820587,	-12.4741601898474,	-15.4251061866556,	-11.2308143897787,	65.1750511245748,	-6.71071724813868,	-3.76629621812902,	-0.891401931941833,	9.37481082175479,	-5.32018283950788,	4.19455851581092,	-2.59816946222789,	3.43732277439644,	-3.79625431371953,	-15.7247852562083,	-17.2569541062567,	-27.2506041897383,	68.6001311130567,	-7.16829007999645,	-8.62173801635298,	-17.4244045168329,	1.53422237928780,	2.88632635034174,	16.8055507640638,	11.2520151074302,	10.0093317752241,	17.5823246419358,	-18.7421275978168,	-11.1394434814478,	-2.94978744424598,	85.1519209569966,	-8.42343160361651,	-0.126600361250192,	-4.96435298820164,	4.95246279896851,	-10.3452113562836,	-3.50356836267591,	-10.7191598157915,	-22.1036556754930,	-2.45180691622810,	3.21231870160160,	2.85278883425023,	-8.93835302883061,	85.0190102634021,	-5.70497360276273,	5.35752432412480,	-9.78963804646739,	-7.55780145157983,	-2.63998218831075,	-12.1808950323618,	-25.0329785464018,	-2.41844232003874,	16.0020846258080,	-6.47741138506695,	-10.6634752436085,	-6.84695122760284,	86.1197820972023,	-8.75033623272119,	-2.42748745643987,	-7.60259402361611,	4.95632273766754,	-4.42384418297874,	-14.1210731885181,	1.09366082351576,	-6.98575271822829,	1.53705061163455,	-1.03358731099419,	-12.6448180501216,	-9.24770380799764,	87.1950885243416,	-4.78414721165086,	-3.14766401814317,	-9.66703313919184,	-4.08528145295644,	-18.0633012170091,	-26.6882188288318,	-13.5290305284481,	1.39763945308297,	-6.25056604786233,	-3.17310466244474,	-11.8400931581298,	-6.00672525761392,	52.9669566580346,	-6.07229269303296,	-5.08855892067536,	-0.679754112753236,	10.4627679753587,	-7.05310785710277,	11.5845448558631,	6.04315783014072,	0.405880450525209,	-2.73882063707849,	-0.185903162565902,	7.93482455694736,	-0.421874218626066,	73.3900498224696,	-13.4784294512964,	-0.734193312971918,	1.00026203676060,	11.9718661308353,	0.340783009211005,	8.11031016037217,	13.3160747762629,	-10.4211324045480,	-2.46663334445433,	-26.5980654037973,	0.299426697954735,	3.38231940249934,	77.9271694832640,	-22.6004161155616,	0.985455017947356,	0.460532291017452,	5.14354582967000,	-8.08046359457380,	-3.01000596500417,	8.45483897361599,	-6.62173288722606,	8.86270909894305,	-14.1150431658979,	6.73272516779730,	-3.24400586283571,	56.3816194130893,	-31.3492162311303,	5.05045430048087,	-6.64440801207832,	-4.08756475713870,	0.368813259134789,	-7.69082174411459,	-19.5848410967572,	22.0913289520602,	-20.0030200505639,	-17.2131551035847,	7.39709684987185,	-13.2604863332182,	85.0002187423348,	-13.4265179467087,	-3.12199054439425,	-5.90427501638279,	-8.94509490551576,	-8.39358786142445,	-1.41008596713879,	-0.273587733745659,	-13.3757120073646,	-8.73054206498824,	-14.2146127445185,	7.16358125774508,	11.9874390209501,	84.9844816370437,	-8.39785880166701,	1.16164980012911,	2.59865400298402,	1.51858894439985,	-5.85226297937051,	-16.1305332773862,	-13.0605292224378,	-12.5352867993926,	-6.16275782856408,	-18.7794537961518,	-6.82795446556466,	-8.35309627646774,	86.1297770698283,	-10.8515031410618,	-6.32880550171940,	-1.10776709672396,	2.93494072381341,	-8.90519531810023,	2.93058718530111,	11.4186449019348,	-11.2085163852130,	-3.76017851533993,	6.55768467379424,	14.7393472135740,	5.74669835659428,	93.5638057374629,	-11.5600366941497,	-13.6744767217657,	10.4284529727034,	14.4173514612873,	-20.3823449851510,	1.97664311391157,	3.98458867338387,	-9.12271358813119,	-6.00087678665445,	-20.7979725592691,	-6.12992820375702,	-4.21252005076234,	85.5083910402600,	-9.52747742008505,	3.88067124945706,	-4.09076095610264,	3.00646908590972,	1.32559190448037,	7.89428671124574,	2.92260207962912,	5.15978731115476,	9.11665886681104,	-10.8570011817242,	-2.28977776655940,	-8.40432254469076};

int main()
{
  int N = 1;
  int D = 13;
  int C = 1;
  int K = 3;

  DSP_Init();

  Init_UART2(115200);

  //ReadParameters(params, 468); // receive SVM parameters from Matlab through UART

  int ii, mm, bb, ll;
  int cc = 0;

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
					   spectrum[mm] = B[mm].real;
				  for(coeff = 0; coeff < D; coeff++){
				  	  mfcc_result[D*cc+coeff] = GetCoefficient(spectrum, 12000, 48, M, coeff);
				  }
				  cc++;
        	  }

			  // Training
        	  if(cc<C) printf("Data Collection: %d blocks\n", cc);
			  else{
				  if(begining){
					  //printf("Training Starts\n");
					  // SVM Initialization
					  initialization();

					  if(cross_validation){
						   do_cross_validation();
					  }
					  else{
						  // save SVM parameters on a model
						  model = svm_read_model(params, K, &param);
					  }
					  begining = 0;
				  }
				  else{
					  // classification
					  predict(mfcc_result,C,D);
				  }
				  cc = 0;
			  }
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
	//double *target = Malloc(double,prob.l);
	double target[3];

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
	param.kernel_type = LINEAR;
	param.degree = 3;
	param.gamma = 0.001;	// 1/num_features
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

	double predict_label;

	int max_line_len = 1024;
	for(i=0;i<N;i++)
	{
		double target_label;

		for(j=0;j<D;j++)
		{
			if(i>=max_nr_attr-1)	// need one more for index = -1
			{
				max_nr_attr *= 2;
				//x = (struct svm_node *) realloc(x,max_nr_attr*sizeof(struct svm_node));
			}

			x[j].index = j;
			x[j].value = featureVector[i*D+j];

			//++i;
		}
		x[j].index = -1;

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
	else{
		info("Predicted Label = %f\n",predict_label);
		//info("Accuracy = %g%% (%d/%d) (classification)\n",
			//(double)correct/total*100,correct,total);
	}
	if(predict_probability)
		free(prob_estimates);
}
