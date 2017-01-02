// As you see, we often run the Ramp input test to get the characteriaziong of the two-step ADC.
// Under a given input voltage, we perform the measurement for serveral times.
// I prefer to group the data first by the input voltage.
// By doing this, we can find out the noise of output data and so on. 
// Let's us just move on, and see what happens.
//
// Author: Zhen-Xiong Yuan

#include <iostream>
#include <vector>
#include <cmath>
#include "TGraph.h"
#include "TF1.h"
using namespace std;

#ifndef DATA_UNDER_GIVEN_INPUT_H
#define DATA_UNDER_GIVEN_INPUT_H

class	Data_Under_Given_Input{
	public:
		double Vi;		// this three lines are original datas
		vector<int> MADC;
		vector<int> LADC;

		int M_value[2];		// We suppose for a given input voltage, there are at most two MADC values
		double  M_rates[2];	// This is set when doing the process_MADC()
		int Counts;		// Numbers of effective events, some unreasonable data should throw away

		int L1[256];		// LADC data when MADC=M_value[0], this is set when doing the process_MADC()
		int L2[256];		// LADC data when MADC=M_value[1]
		double L1_mean;
		double L2_mean;
		double L1_sigma;
		double L2_sigma;
		
		void clear();
		void read_Data(int madc, int ladc);
		void process_Data();

	private:
		void process_MADC();
		void get_LADC_Mean_and_Sigma();
		void get_Mean_and_Sigma_by_Fit(int L[256], double par[3]);
		void get_Mean_and_Sigma_by_Cal(int L[256], double par[3]);
};

void Data_Under_Given_Input::clear(){
	MADC.clear();	LADC.clear();
	M_value[0] = -1; M_value[1] = -1;
	M_rates[0] = 0;  M_rates[1] = 0;
	Counts = 0;

	for(int i=0;i<256;i++){L1[i]=0;L2[i]=0;}
	L1_mean = 0.0; L1_sigma = 0.0; L2_mean = 0.0; L2_sigma = 0.0;
}

void Data_Under_Given_Input::read_Data(int madc, int ladc){
	MADC.push_back(madc);
	LADC.push_back(ladc);
}

void Data_Under_Given_Input::process_Data(){
	process_MADC();
	get_LADC_Mean_and_Sigma();
}

void Data_Under_Given_Input::process_MADC(){
	// get the average of the original MADC values
	double MADC_ave = 0.0;
	for(unsigned int i=0;i<MADC.size();i++) MADC_ave += MADC[i];
	MADC_ave = 1.0*MADC_ave/MADC.size();

	M_value[0] = (int)MADC_ave; M_value[1] = M_value[0]+1;
	//cout<<MADC_ave<<"\t"<<M_value[0]<<endl;
	int c0=0,c1=0;
	for(int i=0;i<256;i++) {L1[i]=0;L2[i]=0;}

	for(unsigned int i=0;i<MADC.size();i++){
		if(MADC[i]==M_value[0]){c0++;L1[LADC[i]]=L1[LADC[i]]+1;}
		else if(MADC[i]==M_value[1]){c1++;L2[LADC[i]]=L2[LADC[i]]+1;}
	}
	if((c1+c0)==0){std::cout<<"Error: We get no data for the input voltage"<<Vi<<std::endl;return;}
	M_rates[0] = 1.0*c0/(c0+c1); M_rates[1] = 1.0*c1/(c1+c0);
	Counts = c0 + c1;
}


void Data_Under_Given_Input::get_Mean_and_Sigma_by_Fit(int L[256], double par[3]){
	int x1 = 256, x2 = 0, num = 0;
	
	TGraph* gr_fit = new TGraph();
	for(int i=0;i<256;i++){
		if(L[i]>0){
			gr_fit->SetPoint(num,i,L[i]);
			num++;
			x1 = (x1<i)? x1:i;
			x2 = (x1>i)? x2:i;
		}
	}
	TF1 func("func","[0]*TMath::Gaus(x,[1],[2])");
	func.SetRange(x1-2,x2+2);
	func.SetParameters(num/8,(x1+x2)/2,8);
	func.SetParLimits(1,(x1+x2)/2-8,(x1+x2)/2+8);
	gr_fit->Fit(&func,"q");//"Q0");
	func.GetParameters(par);

	gr_fit->Delete();
	func.Delete();
}

void Data_Under_Given_Input::get_Mean_and_Sigma_by_Cal(int L[256], double par[3]){
	double mean = 0.0;
	int num = 0;
	for(int i=0;i<256;i++){
		num += L[i];
		mean += i*L[i];
	}
	if(num==0){std::cout<<"FUCK: why zero?"<<std::endl; return;}
	mean = 1.0*mean/num;

	double sigma = 0.0;
	for(int i=0;i<256;i++){
		sigma += L[i]*(i-mean)*(i-mean);
	}
	sigma = sqrt(sigma/num);
	par[0] = num;
	par[1] = mean;
	par[2] = sigma;
}

void Data_Under_Given_Input::get_LADC_Mean_and_Sigma(){
	// By hand calculation only
	double par[3];
	if(Counts*M_rates[0]>0){ get_Mean_and_Sigma_by_Cal(L1,par); L1_mean = par[1]; L1_sigma = par[2];}
	if(Counts*M_rates[1]>0){ get_Mean_and_Sigma_by_Cal(L2,par); L2_mean = par[1]; L2_sigma = par[2];}
}


#endif
