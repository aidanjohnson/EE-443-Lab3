#include "DSP_Config.h"
#include <stdio.h>

int startflag=0;
int kk;
int M=256;

short X[1024];

volatile union {
	Uint16 sh;
	Uint8 i8[2];
} UARTout, UARTin;

char *hello = {"Hello World."}; // data to send

int main()
{
  DSP_Init();

  int mm, uu;

  Init_UART2(115200);
  Puts_UART2(hello); // send the data

  // main stalls here, interrupts drive operation
  while(1) {
	  if(IsDataReady_UART2()){
		  for(uu=0;uu<2;uu++)
			  UARTin.i8[uu] = Read_UART2();
		  printf("%c\n",UARTin.sh);
	  }
	  if(startflag){
		  for(mm=0;mm<256;mm++){
			  UARTout.sh = (Uint16)X[mm];
			  for(uu=0;uu<2;uu++){
				  while(IsTxReady_UART2()==0) ;
				  Write_UART2(UARTout.i8[uu]);
				  wait(10000);
			  }
		  }
	      startflag = 0;
	  }
  }
}
