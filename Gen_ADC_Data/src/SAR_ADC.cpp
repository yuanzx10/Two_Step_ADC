
#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include "SAR_ADC.h"

using namespace std;

SAR_ADC::SAR_ADC(int nbit){
	Nbit = nbit;
	codes = (int)pow(2.0,Nbit);	

	LSB = 0.0;
	Vsig = 0.0;
	Voff = 0.0;
	Cpar = 0.0;
	
	for(int i=0;i<Nbit-1;i++){
		VDAC.push_back(0.0);
		Cmis.push_back(0.0);
		double cap = pow(2.0,i);
		Cap.push_back(cap);
	}
	for(int i=0;i<codes;i++){
		INL.push_back(0.0);
		DNL.push_back(0.0);
		Up_Limit.push_back(0.0);
	}
}


int  SAR_ADC::sort_Code(int icode){
	int num = icode;
	int bit0 = num%2;
	for(int i=0;i<Nbit;i++){
		if(num%2 != bit0) return i-1;
		num = (int)(num/2);
	}
}

void SAR_ADC::set_Cap_Mismatch(vector<double>& cmis){
	if(cmis.size()<Nbit-1){
		std::cout<<"Error: The input Mismatch array is too short";
		return;
	}
	for(int i=0;i<Nbit-1;i++){
		Cmis[i] = cmis[i];
	}
}

void SAR_ADC::set_Offset(double voff){
	Voff = voff;
}

void SAR_ADC::set_Parasitic(double cpar){
	Cpar = cpar;
}

void SAR_ADC::set_Noise_Sigma(double vsig){
	Vsig = vsig;
}

void SAR_ADC::gen_VDAC_and_LSB(){
	double Csum = Cpar + 1.0;
	for(int i=0;i<Nbit-1;i++) Csum += Cap[i] + Cmis[i];
	double VDAC_sum = 0.0;
	for(int i=0;i<Nbit-1;i++){
		VDAC[i] = 1.0*(Cap[i]+Cmis[i])/Csum;
		VDAC_sum += VDAC[i];
	}
	LSB = 2.0*VDAC_sum/(codes-2);
}

void SAR_ADC::gen_DNL_and_INL(){
	vector<double> delta_VDAC;
	for(int i=0;i<Nbit-1;i++) delta_VDAC.push_back(0.0);
	delta_VDAC[0] = VDAC[0];
	double delta_sum = 0.0;
	for(int i=1;i<Nbit-1;i++){
		delta_VDAC[i] = VDAC[i] - delta_sum;
		delta_sum += VDAC[i];
		// for Debug
		std::cout<<"delta VDAC["<<i<<"]="<<delta_VDAC[i]<<endl;
	}
	DNL[0] = 0.0; DNL[codes-1] = 0.0;
	for(int i=1;i<codes-1;i++){
		double sort_code = sort_Code(i);
		DNL[i] = 1.0*delta_VDAC[sort_code]/LSB - 1.0;
	}
	INL[0] = 0.0;
	for(int i=1;i<codes-1;i++){
		INL[i] = INL[i-1] + DNL[i-1];
	}
}

int  SAR_ADC::gen_Code_and_Vres_without_noise(double vi, double& vres){
	vector<double> bit;
	for(int i=0;i<Nbit;i++) bit.push_back(0);
	vres = vi;
	for(int i=0;i<Nbit-1;i++){
		if(vres>Voff) bit[i] = 1;
		vres -= (2*bit[i]-1)*VDAC[Nbit-2-i];
	}
	if(vres>Voff) bit[Nbit-1] = 1;
	int value = 0;
	for(int i=0;i<Nbit;i++) value += pow(2.0,Nbit-1-i)*bit[i];
	return (int)value;
	
}

int  SAR_ADC::gen_Code_and_Vres_with_noise(double vi, double& vres){
	std::random_device rdev{};
	std::default_random_engine generator{rdev()};
	std::normal_distribution<double> distribution(0.0,Vsig);

	vector<double> bit;
	for(int i=0;i<Nbit;i++) bit.push_back(0);
	vres = vi;
	double noise; 
	for(int i=0;i<Nbit-1;i++){
		noise = distribution(generator);
		vres = vres + noise;
		if(vres > Voff) bit[i] = 1;
		vres -= (2*bit[i]-1)*VDAC[Nbit-2-i];
	}
	noise = distribution(generator);
	vres = vres + noise;
	if(vres>Voff) bit[Nbit-1] = 1;
	int value = 0;
	for(int i=0;i<Nbit;i++) value += pow(2.0,Nbit-1-i)*bit[i];
	return (int)value;
	
}

double SAR_ADC::get_Up_Limit_for_Code(int icode){
	vector<int> bits;
	int num = icode;
	int bit;
	for(int i=0;i<Nbit;i++){
		bit = num%2;
		bits.push_back(bit);
		num = (int)(num/2);
	}
	double res = 0.0, up = 2.0, dn = -2.0;
	for(int i=Nbit-1;i>=1;i--){
		if(bits[i]>0){
			dn = (dn>res)?dn:res;
			res += VDAC[i-1];
		}
		else{
			up = (up<res)?up:res;
			res -= VDAC[i-1];
		}
	}
	if(bits[0]<0) up = (up<res)?up:res;
	if(bits[0]>0) dn = (dn>res)?dn:res;

	if(up<dn) return -2;	// In this case, this code is missing!
	else return up;
}

void SAR_ADC::get_Up_Limit(){
	for(int i=0;i<codes-1;i++){
		double up = get_Up_Limit_for_Code(i);
		Up_Limit[i] = up;
	}
	Up_Limit[codes-1] = 1.0;
}










