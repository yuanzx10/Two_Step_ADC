// This is the pragrams for process the two step ADC datas.
// We want to combine the two output code D0 and D1 from MADC and LADC respectively to get a output code.
// The basic idea is by this relationship: D = f(D0) + D1. 
// We want to find out the function f(). We believe that the ramp test we did can give us this information.
//
// 	write the data into a root file: *.root

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdio.h>
#include <cmath>
#include <stdlib.h>
using namespace std;

void get_DNL_INL_From_Count(int array[512],int start, int stop, double dnl[2], double inl[2]);

void read_Data_from_File(){
	const char *filename = "./noise_data.txt";

	ifstream fi;
	fi.open(filename);


	double vol, vres0, vres1;
	int	d0,d1,num=0;

	int	code_density[32][256];
	for(int i=0;i<32;i++){
		for(int j=0;j<256;j++) code_density[i][j] = 0;
	}

	string line;
	while(getline(fi,line)){
		std::istringstream data_str(line);
		if(!(data_str>>vol>>d0>>d1>>vres0>>vres1)) break;
		num++;
		if(num%2000==0) cout<<vol<<endl;
		if(d1>255|d1<0|d0>63|d0<0) continue;
		int label = (int)(d0/2);
		code_density[label][d1] = code_density[label][d1] + 1;	// count add 1
	}

	fi.close();
	

	double step[32];
	int tmp[512];	

	for(int i=0;i<31;i++){
//		TH1I* h1 = new TH1I("h1","h1",256,0,256);
//		TH1I* h2 = new TH1I("h2","h2",256,0,256);
//		for(int j=0;j<256;j++){
//			h1->Fill( code_density[i][j] );
//			h2->Fill( code_density[i+1][j] );
//		}
//		TCanvas c1; h1->Draw();
//		TCanvas c2; h2->Draw();
		// ready to determine the step of i-th branch
		// determine the edge of every branch
		int fall1=255; int rise1=0;
		int fall2=255; int rise2=0;
		for(int n=0;n<128;n++){ if(code_density[i][n]>5){ rise1 = n; break; } }
		for(int n=255;n>128;n--){ if(code_density[i][n]>5){ fall1 = n; break; } }
		for(int n=0;n<128;n++){ if(code_density[i+1][n]>5){ rise2 = n; break; } }
		for(int n=255;n>128;n--){ if(code_density[i+1][n]>5){ fall2 = n; break; } }
		cout<<rise1<<"\t"<<fall1<<"\t"<<rise2<<"\t"<<fall2<<endl;
		
		for(int m=0;m<512;m++) tmp[m] = 0;

		for(int j=0;j<10;j++){
			// Fill the tmp array;
			int delta = fall1 - rise2- j+1;
			for(int k=fall1-9;k<=fall1;k++) tmp[k] = tmp[k] + code_density[i][k];
			for(int k=rise2;k<rise2+10;k++) tmp[delta+k] = tmp[delta+k] + code_density[i+1][k];
			int start = fall1 - 9;
			int stop = delta + 9 + rise2;
			cout<<i<<"\t"<<start<<"\t"<<stop<<endl;
			double dnl[2], inl[2];
			get_DNL_INL_From_Count(tmp,start,stop,dnl,inl);
			double quality = dnl[0]*dnl[0] + dnl[1]*dnl[1]+  inl[0]*inl[0] + inl[1]*inl[1]; 
			cout<<i<<"\t ---------------------------\t"<<quality<<endl;
		}
		
		int hehe; cin>>hehe;
//		h1->Delete(); h2->Delete();
//		c1.Delete();  c2.Delete();
	}


}

void get_DNL_INL_From_Count(int array[512],int start, int stop, double dnl[2], double inl[2]){
	double ave_count = 0;
	for(int i=start;i<=stop;i++) ave_count += array[i];
	ave_count = 1.0*ave_count/(stop-start+1);
	double DNL[512], INL[512];
	for(int i=0;i<start;i++) DNL[i] = 0.0;
	for(int i=stop+1;i<512;i++) DNL[i] = 0.0;
	for(int i=start;i<=stop;i++) DNL[i] = 1.0*array[i]/ave_count - 1.0;
	for(int i=1;i<512;i++) INL[i] = INL[i-1] + DNL[i-1];
	double dnl_min=10, dnl_max=-10;
	double inl_min=10, inl_max=-10;
	for(int i=start;i<stop;i++){
		cout<<DNL[i]<<endl;
		dnl_max = (dnl_max>DNL[i])?dnl_max:DNL[i];
		dnl_min = (dnl_min<DNL[i])?dnl_min:DNL[i];
		inl_max = (inl_max>INL[i])?inl_max:INL[i];
		inl_min = (inl_min<INL[i])?inl_min:INL[i];
	}
	dnl[0] = dnl_max; dnl[1] = dnl_min;
	inl[0] = inl_max; inl[1] = inl_min;
}

void shift_code_test(){
	read_Data_from_File();
}




























