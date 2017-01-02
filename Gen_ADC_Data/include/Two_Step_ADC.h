/*
This is the class of the two step SAR ADC, redundancy is enable.
For a two step SAR ADC, the most important parameters are:
    Nbit0, Nbit1: Number of bits of the ADC0 and ADC1
	Cmis0, Cmis1: Mismatches from the ideal case
	Cpar0, Cpar1: Parasitic capacitance at the comparators input node
	Voff0, Voff1: Offset voltage of the comparators
	Vsig0, Vsig1: Sigma of noise of the ADC
	Gain	: Gain of the residue amplifier
	Voff	: Offset of the residue amplifier

From the above parameters, we can fully define the two-step SAR ADC. 
Based on that, we can get some important features of the ADC, like:
	DNL/INL	: DNL/INL caused by the capacitor mismistch and parasitics
	
In order to get all the above advanced parameters, some methods are needs:

KEEP IN MIND:
    [0] is the LSB!

@author: yuanzx
*/
#include <vector>
#include "SAR_ADC.h"
using namespace std;

#ifndef TWO_STEP_ADC_H
#define TWO_STEP_ADC_H

class	Two_Step_ADC{
	private:
		int Nbit0;
		int Nbit1;
		double Gain;
		double Voff;

	public:
		SAR_ADC	ADC0;
		SAR_ADC ADC1;
		
		Two_Step_ADC(int nbit0, int nbit1);
		
		void set_Cap_Mismatches(vector<double>& cmis0,vector<double>& cmis1);
		void set_Parasitics(double cpar0, double cpar1);
		void set_ADCs_Offset(double voff0, double voff1);
		void set_Noise_Sigmas(double vsig0, double vsig1);
		void set_Amplifier(double gain, double offset);
		
		void gen_Code_without_Noise(double vi, int code[2], double vres[2]);
		void gen_Code_with_Noise(double vi, int code[2], double vres[2]);
};

#endif















