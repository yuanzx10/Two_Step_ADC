// this is the first step when you get a .txt file generated from the ramp input test.
// This file read the datas in the file, and statistic the data into a two deminsion array
// then write the array into another txt file.

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdio.h>
#include <cmath>
#include <stdlib.h>
using namespace std;

typedef struct{ Float_t	Vi; Int_t	D0; Int_t	D1; } Event;

void write_Array_into_File(int arr[][256]){
	ofstream fout;
	fout.open("array.txt");
	for(int i=0;i<32;i++){
		for(int j=0;j<256;j++) fout<<i<<"\t"<<j<<"\t"<<arr[i][j]<<endl;
	}
	fout.close();
}

void data_process_step1(){
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
		if(num%20000==0) cout<<vol<<"\t"<<d0<<"\t"<<d1<<"\t\t"<<label<<endl;
		code_density[label][d1] = code_density[label][d1] + 1;	// count add 1
	}
	file->Write();
	file->Close();
	fi.close();

	write_Array_into_File(code_density);
}
