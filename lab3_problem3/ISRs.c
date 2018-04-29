// Welch, Wright, & Morrow, 
// Real-time Digital Signal Processing, 2011

///////////////////////////////////////////////////////////////////////
// Filename: ISRs.c
//
// Synopsis: Interrupt service routine for codec data transmit/receive
//
///////////////////////////////////////////////////////////////////////

#include "DSP_Config.h" 
  
// Data is received as 2 16-bit words (left/right) packed into one
// 32-bit word.  The union allows the data to be accessed as a single 
// entity when transferring to and from the serial port, but still be 
// able to manipulate the left and right channels independently.

#define LEFT  0
#define RIGHT 1

//complex data structure used by FFT
extern int startflag;
extern int kk;
#define M 256
extern short X[M];

Int16 monoIn;

volatile union {
	Uint32 UINT;
	Int16 Channel[2];
} CodecDataIn, CodecDataOut;


struct cmpx {
	float real;
	float imag;
	};
typedef struct cmpx COMPLEX;

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

	if(CheckForOverrun()) // overrun error occurred (i.e. halted DSP)
		return; // so serial port is reset to recover

	CodecDataIn.UINT = ReadCodecData(); // get input data samples
	
	if(kk>=M){
		kk = 0;
		startflag = 1;
	}

	monoIn = 0.5*(CodecDataIn.Channel[LEFT] + CodecDataIn.Channel[RIGHT]);

	if(!startflag){
		// P3 Put a new data to the buffer X
		X[kk++] = monoIn;
	}
	CodecDataOut.Channel[LEFT] = monoIn;
	CodecDataOut.Channel[RIGHT] = monoIn;

	WriteCodecData(CodecDataOut.UINT); // send output data to  port
}

