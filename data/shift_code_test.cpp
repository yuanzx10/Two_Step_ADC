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

typedef struct{ Float_t	Vi; Int_t	D0; Int_t	D1; } Event;

void get_DNL_INL_From_Count(int array[512],int start, int stop, double dnl[2], double inl[2]);

void write_Array_into_File(int arr[][256]){
	ofstream fout;
	fout.open("array.txt");
	for(int i=0;i<32;i++){
		for(int j=0;j<256;j++) fout<<i<<"\t"<<j<<"\t"<<arr[i][j]<<endl;
	}
	fout.close();
}

void read_Array_from_File(int arr[][256]){
	ifstream fin;
	fin.open("array.txt");
	int i,j,d;
	while(fin>>i>>j>>d){
		arr[i][j] = d;
	}
//	for(int i=0;i<31;i++){
//		for(int j=0;j<256;j++) fin>>arr[i][j];
//	}
	fin.close();
}

void read_Data_from_File(){
	const char *filename = "./noise_data.txt";

	ifstream fi;
	fi.open(filename);
	
	Event evt;
	TFile *file = new TFile("result.root","recreate");
	TTree *tree = new TTree("dump","dump");
	tree->Branch("Event",&evt,"Vi/F:D0/I:D1/I");

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
		evt.Vi = vol; evt.D0 = d0; evt.D1 =d1;	tree->Fill();
		if(d1>255|d1<0|d0>63|d0<0) continue;
		int label = (int)(d0/2);
		if(num%2000==0) cout<<vol<<"\t"<<d0<<"\t"<<d1<<"\t\t"<<label<<endl;
		code_density[label][d1] = code_density[label][d1] + 1;	// count add 1
	}
	file->Write();
	file->Close();

	fi.close();

	write_Array_into_File(code_density);
	
/*
*/
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
		dnl_max = (dnl_max>DNL[i])?dnl_max:DNL[i];
		dnl_min = (dnl_min<DNL[i])?dnl_min:DNL[i];
		inl_max = (inl_max>INL[i])?inl_max:INL[i];
		inl_min = (inl_min<INL[i])?inl_min:INL[i];
	}
	dnl[0] = dnl_max; dnl[1] = dnl_min;
	inl[0] = inl_max; inl[1] = inl_min;
}

void get_Step(int step[32]){
	int code_density[32][256];
	read_Array_from_File(code_density);

	int tmp[512];	
	for(int i=0;i<31;i++){
		int fall1=255; int rise1=0; int fall2=255; int rise2=0;
		for(int n=0;n<128;n++){ if(code_density[i][n]>5){ rise1 = n; break; } }
		for(int n=255;n>128;n--){ if(code_density[i][n]>5){ fall1 = n; break; } }
		for(int n=0;n<128;n++){ if(code_density[i+1][n]>5){ rise2 = n; break; } }
		for(int n=255;n>128;n--){ if(code_density[i+1][n]>5){ fall2 = n; break; } }

		for(int m=0;m<512;m++) tmp[m] = 0;
		
		double min_delta=500;
		double min_factor=100;
		for(int j=0;j<10;j++){
			// Fill the tmp array;
			int delta = fall1 - rise2- j+1;
			for(int k=0;k<256;k++) tmp[k] = tmp[k] + code_density[i][k];
			for(int k=0;k<256;k++) tmp[delta+k] = tmp[delta+k] + code_density[i+1][k];
			int start = fall1 - 14;
			int stop = delta + 14 + rise2;
	//		cout<<i<<"\t"<<start<<"\t"<<stop<<endl;
			double dnl[2], inl[2];
			get_DNL_INL_From_Count(tmp,start,stop,dnl,inl);
			double quality = (dnl[0]-dnl[1])*(dnl[0]-dnl[1]);//+  (inl[0]-inl[1])*(inl[0]-inl[1]); 
			if(min_factor>quality){
				min_factor = quality;
				min_delta = delta;
			}
	//		cout<<i<<"\t"<<delta<<"\t->\t"<<quality<<endl;
		}
		cout<<i<<"\t -> \t"<<min_delta<<"\t->\t"<<min_factor<<endl;
		step[i] = min_delta+1;
	}
}



void shift_code_test(){
//	read_Data_from_File();
	
	int step[32];
	get_Step(step);

	int acc_step[32];
	acc_step[0] = 0;
	for(int i=1;i<32;i++) acc_step[i] = acc_step[i-1] + step[i-1];


	const char *filename = "./noise_data.txt";
	ifstream fi;
	fi.open(filename);

	double vol, vres0, vres1;
	int	d0,d1,num=0;
	
	TH1I *h1 = new TH1I("h1","h1",5000,0,5000);
	string line;
	while(getline(fi,line)){
		std::istringstream data_str(line);
		if(!(data_str>>vol>>d0>>d1>>vres0>>vres1)) break;
		int x = (int)(d0/2);
		int d = acc_step[x] + d1;
		if(d1==0|d1==255) continue;
		h1->Fill(d);
	//	num++; if(num%4000==0) cout<<vol<<"\t"<<d0<<"\t"<<d1<<"\t"<<d<<endl;
	}

	h1->Draw();
}
























