// This is the test program of the SAR_ADC and Two_Step_ADC

#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include "Two_Step_ADC.h"

using namespace std;

struct Event{
	double	voltage;
	int	D0;
	int	D1;
};

int main(){

	cout<<"WTF"<<endl;
	int Nbit0 = 6, Nbit1 = 8;
	Two_Step_ADC ADC(Nbit0,Nbit1);	
	
	// Set the mismtach array
	double a[10]={0.05,0.05,-0.10,0.10,-0.10,0.10,0.10,0.05,-0.25};
	vector<double> Cmis0(a,a+10);
	
	double b[10]={0.05,0.10,0.10,-0.05,-0.20,0.15,-0.15,0.35,-0.35};
	vector<double> Cmis1(b,b+10);

	ADC.set_Cap_Mismatches(Cmis0,Cmis1);
	
	// after set the parasitic and mismatches of the ADCs, do this step!
	ADC.ADC0.gen_VDAC_and_LSB();
	ADC.ADC1.gen_VDAC_and_LSB();
	
	// set noise
	//double Vsig0 = ADC.ADC0.LSB/20;	// LSB0/10
	//double Vsig1 = ADC.ADC1.LSB/5;	// LSB1/5
	double Vsig0 = 0.001/5.0;
	double Vsig1 = 0.001;
	ADC.set_Noise_Sigmas(Vsig0,Vsig1);

	// set ADC offsets
	double Voff0 = pow(2.0,-Nbit0)*0.2;	// 0.1*LSB0
	double Voff1 = pow(2.0,-Nbit1)*1.2;	// 0.6*LSB0
	ADC.set_ADCs_Offset(Voff0,Voff1);		// offset of ADC1 is not used

	// set amplifier
	double gain_error = 0.1;
	double gain_off	  = Voff1;
	double gain = pow(2.0,Nbit0-2)*(1.0+gain_error);
	ADC.set_Amplifier(gain,0.0);

	// Generate data into a txt file

	ofstream fout;
	fout.open("../data/noise_data.txt");

	//int npoints = (int)(4*pow(2.0,Nbit0+Nbit1));
	int npoints = 5000*5;
	double vi;
	int d[2];
	double vres[2];
	for(int i=-npoints;i<npoints;i++){
		vi = 1.05*i/npoints;
		for(int j=0;j<200;j++){
			ADC.gen_Code_with_Noise(vi,d,vres);
			fout<<vi<<"\t"<<d[0]<<"\t"<<d[1]<<"\t"<<vres[0]<<"\t"<<vres[1]<<endl;
		}
		if(i%1000==0) cout<<vi<<endl;
	}
	fout.close();
}


















