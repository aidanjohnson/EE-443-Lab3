#include "DSP_Config.h"
#include <stdio.h>


volatile union {
	double sh;
	Uint8 i8[8];
} UARTdouble;

int main()
{
  DSP_Init();

  Init_UART2(115200);

  int iter = 0;

  // main stalls here, interrupts drive operation
  while(1) {
	  if(IsDataReady_UART2()){
		  UARTdouble.i8[iter++] = Read_UART2();
		  printf("Received uint8: %d \n", UARTdouble.i8[iter-1]);
		  if(iter>7){
			  iter = 0;
			  printf("Received double: %lf \n", UARTdouble.sh);
		  }
		  while(IsTxReady_UART2()==0) ;
		  Write_UART2(1);
		  wait(10000);
	  }
  }
}
