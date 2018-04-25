#include "DSP_Config.h"
#include <stdio.h>


volatile union {
	double sh;
	Uint8 i8[8];
} UARTdouble;

double rec[39];
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

int main()
{
  DSP_Init();

  Init_UART2(115200);

  double params[39];

  ReadParameters2(params, 39);


  // main stalls here, interrupts drive operation
  while(1) {
	  ;
  }
}
