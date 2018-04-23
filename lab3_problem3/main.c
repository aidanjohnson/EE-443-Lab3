// Welch, Wright, & Morrow, 
// Real-time Digital Signal Processing, 2011
 
///////////////////////////////////////////////////////////////////////
// Filename: main.c
//
// Synopsis: Main program file for demonstration code
//
///////////////////////////////////////////////////////////////////////

#include "DSP_Config.h"
#include "fft.h"
#include "gmm.h"
#include "libmfcc.h"
#include "math.h"

#define numFilters 48
#define FFTsize 256
COMPLEX spectrum[FFTsize];
COMPLEX frame[FFTsize];

int main()
{    
	
	// initialize DSP board
  	DSP_Init();

	// call StartUp for application specific code
	// defined in each application directory
	StartUp();
	
	Init_UART2(115200); // set baudrate

	// main stalls here, interrupts drive operation 
  	while(1) { 
  		GMM gmm;
		if(IsDataReady_UART2()) { // check if UART ready to receive data
			gmm[1] = Read_UART2(); // store received data
		}
		fft(spectrum, FFTsize, frame);
		int fs = GetSampleFreq();
		volatile float mfcc_result[13];
		int coeff;
		double abs_spectral[FFTsize];
		abs_spectral = (double) spectrum.real; //pow(pow(spectrum.real, 2) + pow(spectrum.imag, 2), 0.5);
		for (coeff = 0; coeff < 13; coeff++) {
			mfcc_result[coeff] = GetCoefficient(*abs_spectral, fs, numFilters, FFTsize, coeff);
		}
  	}   
}


