// This file is going to hanld on the datas obtained from the ramp input test.
// We believe that the ramp test can give us all the imformation we want.
//
// Author: 	Zhen-Xiong Yuan
// Date:	2017/01/01

#include <iostream>
#include <vector>
#include "TGraph.h"
#include "TF1.h"
#include "Data_Under_Given_Input.h"
using namespace std;

#ifndef RAMP_DATA_PROCESS
#define RAMP_DATA_PROCESS

class	Ramp_Data_Process{

	public:
		vector<Data_Under_Given_Input> Ramp_Datas;	// this is all the magic datas
		// This is used to add the data into the Ramp_Datas 
		void add_New_Data(Data_Under_Given_Input data);
		
		// Get the voltage boundary of MADC. The result may sit between two adjacent input voltages
		// This is done by fitting with the error function!
		// By doing this, we can also get the sigma of the MADC, we store it into the MADC_Sigma[64]
		// We can also use this result to calculate the DNL of the MADC, refer to MADC_DNL_by_Binsize[64]
		// Certainly, we can calculate the DNL by count, which is stored into MADC_DNL_by_Count[64]
		int	MADC_Min;		// minimum MADC value store in the database
		int	MADC_Max;		
		double	MADC_Boundary[64];	// up boundary
		double	MADC_Sigma[64];		// noise of the boundary
		double	MADC_DNL_by_Binsize[64];	// DNL by the boundary and binsize
		double	MADC_INL_by_Binsize[64];
		double	MADC_DNL_by_Count[64];	// DNL by the counting
		double	MADC_INL_by_Count[64];
		double	MADC_LSB;

		void get_MADC_Min_and_Max();
		void get_MADC_Boudary();
		void get_MADC_DNL_INL_by_Binsize();
		void get_MADC_DNL_INL_by_Count();
		void get_MADC_Info();
		void get_MADC_Cap_Info();

};

void Ramp_Data_Process::add_New_Data(Data_Under_Given_Input data){
	Ramp_Datas.push_back(data);
}

void Ramp_Data_Process::get_MADC_Min_and_Max(){
	// we assume the all the data was obtained ascending or descending
	MADC_Min = Ramp_Datas[0].M_value[0];
	MADC_Max = Ramp_Datas[Ramp_Datas.size()-1].M_value[0];
}

void Ramp_Data_Process::get_MADC_Boudary(){
	double vi;	// current data's Vi
	for(int i=0;i<64;i++) MADC_Boundary[i] = 0.;
	TF1 func("f","0.5*(1+TMath::Erf(-(x-[0])/[1]))");	// fit function
	//for(int madc=11;madc>=2;madc--){
	for(int madc=MADC_Min;madc<MADC_Max;madc++){
		TGraph* gr_scurve = new TGraph();		// fit graph
		double x1=2000,x2=-2000;			// sepecify the fit boundary
		int num=0;					// numbers of the events gets
		for(int i=0;i<Ramp_Datas.size();i++){
			// the madc value, and the counts is rational higher 
			if( (Ramp_Datas[i].M_value[0]==madc) && (Ramp_Datas[i].M_rates[0]>0.01) && (Ramp_Datas[i].M_rates[1]>0.01) ){
				vi = Ramp_Datas[i].Vi;
				gr_scurve->SetPoint(num, vi, Ramp_Datas[i].M_rates[0]);
				x1 = (x1<vi)?x1:vi;
				x2 = (x2>vi)?x2:vi;
				num++;
				if(num>100) break;
			}
		}
		func.SetRange(2*x1-x2,2*x2-x1);
		func.SetParameters((x1+x2)/2,10);
		func.SetParLimits(0,2*x1-x2,2*x2-x1);
		func.SetParLimits(1,0,30);
		gr_scurve->Fit(&func,"q0");//"q0");
		double par[2];
		func.GetParameters(par);
		MADC_Boundary[madc] = par[0];
		MADC_Sigma[madc] = par[1];
		cout<<madc<<"\t"<<par[0]<<"\t"<<par[1]<<endl;
		gr_scurve->Delete();
	}	
	func.Delete();
}

void Ramp_Data_Process::get_MADC_DNL_INL_by_Binsize(){
	double binsize[64];
	double LSB;
	for(int i=MADC_Min+1;i<MADC_Max;i++){
		binsize[i] = MADC_Boundary[i] - MADC_Boundary[i-1];
		LSB += binsize[i];
	}
	if(MADC_Max - ADC_Min == 1) {cout<<"Error: Too less boundary to get a binsize. You are supposed to take more data!"<<endl;return;}
	LSB = 1.0*LSB/(MADC_Max-MADC_Min-1);
	MADC_LSB = LSB;		// get the MADC_LSB
	// get the MADC_DNL_by_Binsize and MADC_INL_by_Binsize
	// The DNL of MADC without any datas are set to 0
	for(int i=0;i<=MADC_Min;i++) binsize[i] = LSB;
	for(int i=MADC_Max;i<64;i++) binsize[i] = LSB;

	for(int i=0;i<64;i++) MADC_DNL_by_Binsize[i] = 1.0*binsize[i]/LSB - 1.0;
	MADC_INL_by_Binsize[0] = 0.0;
	for(int i=1;i<64;i++) MADC_INL_by_Binsize[i] = MADC_INL_by_Binsize[i-1] + MADC_DNL_by_Binsize[i-1];
}

void Ramp_Data_Process::get_MADC_DNL_INL_by_Count(){
	double count[64];
	for(unsigned int i=0;i<64;i++) count[i]=0.0;

	for(unsigned int i=0;i<Ramp_Datas.size();i++){
		count[Ramp_Datas[i].M_value[0]] += Ramp_Datas[i].M_rates[0]*Ramp_Datas[i].Counts;
		count[Ramp_Datas[i].M_value[1]] += Ramp_Datas[i].M_rates[1]*Ramp_Datas[i].Counts;
	}
	double count_sum = 0.0;
	for(unsigned int i=MADC_Min+1;i<MADC_Max;i++) count_sum += count[i];
	if(MADC_Max - ADC_Min == 1) {cout<<"Error: Too less boundary to get a binsize. You are supposed to take more data!"<<endl;return;}
	double ave_sum = 1.0*count_sum/(MADC_Max-MADC_Min-1);
	
	// set the undedermined MADC count to be average to give a 0 DNL
	for(int i=0;i<=MADC_Min;i++) count[i] = ave_sum;
	for(int i=MADC_Max;i<64;i++) count[i] = ave_sum;

	for(unsigned int i=0;i<64;i++) MADC_DNL_by_Count[i] = 1.0*count[i]/ave_sum - 1.0;
	MADC_INL_by_Count[0] = 0.0;
	for(unsigned int i=1;i<64;i++) MADC_INL_by_Count[i] = MADC_INL_by_Count[i-1] + MADC_DNL_by_Count[i-1];

}

void Ramp_Data_Process::get_MADC_Info(){
	get_MADC_Min_and_Max();
	get_MADC_Boudary();
	get_MADC_DNL_INL_by_Binsize();
	get_MADC_DNL_INL_by_Count();
	get_MADC_Cap_Info();
}

void Ramp_Data_Process::get_MADC_Cap_Info(){
	// TODO: From the DNL result to calculate the capacitor mismatches
}

#endif











