// This file is going to handle with the datas obtained from the ramp input test.
// We believe that the ramp test can give us all the imformation we want.
//
// Author: 	Zhen-Xiong Yuan
// Date:	2017/01/01

#include <iostream>
#include <vector>
#include "TGraph.h"
#include "TF1.h"
#include "TH1I.h"
#include "TCanvas.h"

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
		double	MADC_Offset;

		void get_MADC_Min_and_Max();
		void get_MADC_Boudary();
		void get_MADC_DNL_INL_by_Binsize();
		void get_MADC_DNL_INL_by_Count();
		void get_MADC_Offset();
		void get_MADC_Info();
		// After all the data is ready, just simply call this function to prepare everything
		void get_MADC_Cap_Info();	

		// Get the infomation of the LADC. This is much more difficult than that of the MADC.
		// And as you will see, this block will be much more disorder than the MADC
		double	LADC_LSB;	// Can the gain error of LADC be seperated from the gain error of Amp?
		double	LADC_Offset;	// Can the offset of the LADC be seperated from the offset of the Amp?

		// This part provided some methods to map the D0 and D1 into a single D(which is around 12bit)
		// The first methods is like : D = gain*D0 + D1 + Offset. 
		// I seriously doubt this methods work fine, due to the DNL of the MADC. But anyway, we do it!
		// The second methods is more complicated than the first one: D = f(D0) + D1
		// This is a modification of the first methods, and the determined of f(D0) is challenging.
		// -------------------------------------------------------------------------------------- //
		// This is the most important array for the second methods, cause it determine the f(x)
		// There are serveral ways to get this array.
		double MADC_Boundary_Step[64];
		// -------------------------------------------------------------------------------------- //
		double P0[64], P1[64];	// y = p0 + p1*x, this is where we put the fit_MADC_Branch result
		void fit_MADC_Branch();	// Now this is using the L_Mean to fit the result, I guess it will lose.
		void get_MADC_Boundary_Step();
		
		void get_ADC_DNL_by_Count();	// Only after you get the MADC_Boundary_Step array can you call this function.
};

// we are trying to make this function can de invoke as soon as finishing the read datas
void Ramp_Data_Process::get_ADC_DNL_by_Count(){
	get_MADC_Info();
	fit_MADC_Branch();
	get_MADC_Boundary_Step();
	
	cout<<endl<<"We are going to print out the f(x)"<<endl;
	double acc_step[64];
	for(int i=0;i<64;i++) acc_step[i] = 0.0;
	for(int i=2;i<64;i=i+2){ 
		acc_step[i] = acc_step[i-1] + MADC_Boundary_Step[i];
		acc_step[i+1] = acc_step[i];
		cout<<i<<"\t\t"<<acc_step[i]<<endl;
       	}

	cout<<endl<<"\t Hi Boy: we are going to plot the ADC code density"<<endl;
	for(int i=0;i<Ramp_Datas.size();i++){
		Ramp_Datas[i].process_Dout(acc_step);
	}
	TH1I* h1 = new TH1I("h1","12b ADC output code density",5000,0,5000);
	for(int i=0;i<Ramp_Datas.size();i++){
		for(int j=0;j<Ramp_Datas[i].Dout.size();j++){
			int dout = (int)( Ramp_Datas[i].Dout[j] );
			h1->Fill(dout);
		}
	}
	h1->Draw();
	cout<<endl<<"\t Finishing Draw the code density!"<<endl;
}

void Ramp_Data_Process::add_New_Data(Data_Under_Given_Input data){
	Ramp_Datas.push_back(data);
}

void Ramp_Data_Process::get_MADC_Min_and_Max(){
	// we assume the all the data was obtained ascending or descending
	//MADC_Min = Ramp_Datas[0].M_value[0];
	//MADC_Max = Ramp_Datas[Ramp_Datas.size()-1].M_value[0];
	MADC_Min = 0; MADC_Max = 63;
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
		cout<<"Fit the MADC Boundary\t"<<madc<<"\t"<<par[0]<<"\t"<<par[1]<<endl;
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
	if(MADC_Max - MADC_Min == 1) {cout<<"Error: Too less boundary to get a binsize. You are supposed to take more data!"<<endl;return;}
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
	if(MADC_Max - MADC_Min == 1) {cout<<"Error: Too less boundary to get a binsize. You are supposed to take more data!"<<endl;return;}
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
//	get_MADC_DNL_INL_by_Binsize();
//	get_MADC_DNL_INL_by_Count();
//	get_MADC_Cap_Info();
}

void Ramp_Data_Process::get_MADC_Cap_Info(){
	// TODO: From the DNL result to calculate the capacitor mismatches
}

// This method have some problems that will lead to urgly data output, we shall modified it!
void Ramp_Data_Process::fit_MADC_Branch(){
	cout<<endl<<"\t Call the function fit_MADC_Branch to get P1 and P0"<<endl;
	double vi;	// current data's Vi
	for(int i=0;i<64;i++){P1[i] = 0.0; P0[i] = 0.0;}	// reset the result
	TF1* func = new TF1("func","pol1");

	int start_point = 1;
	int num = 0;
	for(int madc=MADC_Min;madc<MADC_Max;madc++){
		TGraph* gr = new TGraph();		// fit graph
		num = 0;
		for(int i=start_point;i<Ramp_Datas.size();i++){
			if( (Ramp_Datas[i].M_value[0]==madc)&&(Ramp_Datas[i].M_rates[0]>0.95) ){
				if( (Ramp_Datas[i].L1_mean>5)&&(Ramp_Datas[i].L1_mean<250) ){
					gr->SetPoint(num,Ramp_Datas[i].Vi,Ramp_Datas[i].L1_mean);
					num++;
				}
			}
			else if(Ramp_Datas[i].M_value[0]==madc+1){
				start_point = i;
				cout<<num<<"\t";
				break;
			}
		}// get the data
		gr->Fit(func,"q");
		P0[madc] = func->GetParameter(0);
		P1[madc] = func->GetParameter(1);
		cout<<madc<<"\t"<<P0[madc]<<"\t"<<P1[madc]<<endl;
		gr->Delete();
	}	
	func->Delete();
}

// MADC_Boundary needs to be known, before call this function
void Ramp_Data_Process::get_MADC_Boundary_Step(){
	cout<<endl<<"\t Call get_MADC_Boundary_Step to get the MADC_Boundary_Step"<<endl;
	for(int i=0;i<64;i++) MADC_Boundary_Step[i]=0;
	double up,dn;
	int start = MADC_Min + (int)((MADC_Min+1)%2);
	for(int i=start;i<MADC_Max;i=i+2){
		double vb = MADC_Boundary[i];
		up = P0[i] + 1.0*P1[i]*vb;
		dn = P0[i+1] + 1.0*P1[i+1]*vb;
		MADC_Boundary_Step[i+1] = up - dn + 0.00;  // this parameters is somewhat annoying.
		MADC_Boundary_Step[i+2] = MADC_Boundary_Step[i+1];
	}
	cout<<endl<<"\t MADC_Boundary_Step"<<endl;
	for(int i = 0;i<64;i++) cout<<i<<"\t"<<MADC_Boundary_Step[i]<<endl;
	cout<<endl;
}









#endif

