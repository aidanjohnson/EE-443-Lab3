// Welch, Wright, & Morrow, 
// Real-time Digital Signal Processing, 2012

///////////////////////////////////////////////////////////////////////
// Filename: ISRs.c
//
// Synopsis: Interrupt service routine for codec data transmit/receive
//
///////////////////////////////////////////////////////////////////////

#include "DSP_Config.h" 
//#include "fft.h"
  
// Data is received as 2 16-bit words (left/right) packed into one
// 32-bit word.  The union allows the data to be accessed as a single 
// entity when transferring to and from the serial port, but still be 
// able to manipulate the left and right channels independently.

#define LEFT  0
#define RIGHT 1

volatile union {
	Uint32 UINT;
	Int16 Channel[2];
} CodecDataIn, CodecDataOut;

struct cmpx                       //complex data structure used by FFT
    {
    float real;
    float imag;
    };
typedef struct cmpx COMPLEX;


/* add any global variables here */
extern int startflag;
extern int kk;
extern int M;

extern short X[1024];

interrupt void Codec_ISR()
///////////////////////////////////////////////////////////////////////
// Purpose:   Codec interface interrupt service routine  
//
// Input:     None
//
// Returns:   Nothing
//
// Calls:     CheckForOverrun, ReadCodecData, WriteCodecData
//
// Notes:     None
///////////////////////////////////////////////////////////////////////
{                    
	/* add any local variables here */

 	if(CheckForOverrun())					// overrun error occurred (i.e. halted DSP)
		return;								// so serial port is reset to recover

  	CodecDataIn.UINT = ReadCodecData();		// get input data samples

	/* add your code starting here */

	// I added my code here
	if(kk>M-1){
         /* (1). Initialize index kk                                            */
		kk=0;
         /* (2). Change startflag to start processing in while loop in main()   */
		startflag = 1;
	}

	if(!startflag){
         /* (1). Put a new data to the buffer X    */
		X[kk] = CodecDataIn.Channel[0];
         /* (2). Update index kk                   */
		kk++;
       }
	// end of my code

	/* end your code here */

	WriteCodecData(CodecDataIn.UINT);		// send output data to  port
}
