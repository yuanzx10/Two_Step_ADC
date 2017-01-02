
#include <vector>
#include <cmath>
#include "SAR_ADC.h"
#include "Two_Step_ADC.h"
using namespace std;

Two_Step_ADC::Two_Step_ADC(int nbit0, int nbit1):ADC0(nbit0),ADC1(nbit1){
	Nbit0 = nbit0;
	Nbit1 = nbit1;
	
	Gain = pow(2.0,Nbit0-2);
	Voff = 0.0;
}

void Two_Step_ADC::set_Cap_Mismatches(vector<double>& cmis0,vector<double>& cmis1)
{
	ADC0.set_Cap_Mismatch(cmis0);
	ADC1.set_Cap_Mismatch(cmis1);
}

void Two_Step_ADC::set_Parasitics(double cpar0, double cpar1){
	ADC0.set_Parasitic(cpar0);
	ADC1.set_Parasitic(cpar1);
}

void Two_Step_ADC::set_ADCs_Offset(double voff0, double voff1){
	ADC0.set_Offset(voff0);
	ADC1.set_Offset(voff1);
}

void Two_Step_ADC::set_Noise_Sigmas(double vsig0, double vsig1){
	ADC0.set_Noise_Sigma(vsig0);
	ADC1.set_Noise_Sigma(vsig1);
}

void Two_Step_ADC::set_Amplifier(double gain, double offset){
	Gain = gain;
	Voff = offset;
}

void Two_Step_ADC::gen_Code_without_Noise(double vi, int code[2], double vres[2]){
	double tmp_code, tmp_vres;
	code[0] = ADC0.gen_Code_and_Vres_without_noise(vi,vres[0]);
	double vi1 = Gain*vres[0] + Voff;
	code[1] = ADC1.gen_Code_and_Vres_without_noise(vi1,vres[1]);
}

void Two_Step_ADC::gen_Code_with_Noise(double vi, int code[2], double vres[2]){
	double tmp_code, tmp_vres;
	code[0] = ADC0.gen_Code_and_Vres_with_noise(vi,vres[0]);
	double vi1 = Gain*vres[0] + Voff;
	code[1] = ADC1.gen_Code_and_Vres_with_noise(vi1,vres[1]);
}
