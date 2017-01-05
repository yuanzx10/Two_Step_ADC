//
//


#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdio.h>
#include <cmath>
#include <stdlib.h>
using namespace std;

typedef struct{ Float_t	Vi; Int_t	D0; Int_t	D1; } Event;

void read_Array_from_File(int arr[][256]){
	ifstream fin;
	fin.open("array.txt");
	int i,j,d;
	while(fin>>i>>j>>d){
		arr[i][j] = d;
	}
	fin.close();
}


void get_Step(int step[32]){
	int arr[32][256];
	read_Array_from_File(arr);

	for(int i=0;i<31;i++){
		int fall1=255; int rise1=0; int fall2=255; int rise2=0;
		for(int n=0;n<128;n++){ if(arr[i][n]>5){ rise1 = n; break; } }
		for(int n=255;n>128;n--){ if(arr[i][n]>5){ fall1 = n; break; } }
		for(int n=0;n<128;n++){ if(arr[i+1][n]>5){ rise2 = n; break; } }
		for(int n=255;n>128;n--){ if(arr[i+1][n]>5){ fall2 = n; break; } }
		cout<<"\t"<<i<<"\t"<<fall1-rise2+1<<endl;
		// calculate tha average counts of arr[i][n] in the n range of rise1+10, fall1-10
		
		double ave_count = 0.0;
		for(int j=rise1+10;j<=fall1-10;j++) ave_count += arr[i][j];
		ave_count = ave_count/(fall1-rise1-19);

		// calculate the counts between [fall1-9,fall1] 
		int num_code = 12;
		double res_count = 0.0;
		for(int j=fall1-num_code+1;j<=fall1;j++) res_count += arr[i][j];
		res_count = num_code*ave_count - res_count;
		cout<<endl<<i<<"\t"<<ave_count<<"\t"<<res_count<<endl;		
		double add_count[100];
		for(int m=0;m<100;m++) add_count[i] = 0;

		for(int j=-num_code;j<=2*num_code;j++){
			cout<<j+num_code<<"\t\t";
			int sum_count = 0;
			for(int k=rise2+j;k<rise2+j+num_code;k++){
				sum_count += arr[i+1][k];
				cout<<arr[i+1][k]<<"\t";
			}
			
			cout<<"\t"<<j+num_code<<"\t";
			add_count[j+num_code] = sum_count;
			cout<<add_count[j+num_code]<<endl;
		}
		cout<<endl;
		for(int m=0;m<3*num_code;m++){
			cout<<"("<<m<<","<<add_count[m]<<")\t";
			add_count[m] = abs(add_count[m]-res_count);
		}
		cout<<endl;
		int index = 0; double value = add_count[0];
		for(int m=1;m<2*num_code;m++){
			if(value>add_count[m]){ value = add_count[m]; index = m; }
		}
		step[i] = fall1 - rise2 + 1 - index ;
		cout<<i<<"\t -> \t"<<index<<"\t"<<step[i]<<endl<<endl;

	}
}

void get_Step_by_Area(){
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
	}
	h1->Draw();
}
