
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdio.h>
#include <cmath>
#include <stdlib.h>
using namespace std;


void read_Array_from_File(int arr[][256]){
	ifstream fin;
	fin.open("array.txt");
	int i,j,d;
	while(fin>>i>>j>>d){
		arr[i][j] = d;
	}
	fin.close();
}

void get_Step_by_Minimize_Res_Area2(int step[32]){
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
		int num_code = 10;
		double res_count = 0.0;
		for(int j=fall1-num_code+1;j<=fall1+num_code;j++) res_count += arr[i][j];
		res_count = 2*num_code*ave_count - res_count;

		cout<<endl<<i<<"\t"<<ave_count<<"\t"<<res_count<<endl;		
		double add_count[100];
		for(int m=0;m<100;m++) add_count[i] = 0;

		for(int j=-num_code;j<=2*num_code;j++){
			cout<<j+num_code<<"\t\t";
			int sum_count = 0;
			for(int k=rise2+j;k<rise2+j+2*num_code;k++){
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

		// find the step that minimized the res
		int index = 0; double value = add_count[0];
		for(int m=1;m<2*num_code;m++){
			if(value>add_count[m]){ value = add_count[m]; index = m; }
		}
		step[i] = fall1 - rise2 + 1 - index;
		cout<<i<<"\t -> \t"<<index<<"\t"<<step[i]<<endl<<endl;
	}
}

void get_Step_by_Nearest_Area(int step[32]){
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
		int num_code = 4;
		double res_count = 0.0;
		for(int j=fall1-num_code+1;j<=fall1;j++) res_count += arr[i][j];
		res_count = (num_code+0)*ave_count - res_count;
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
		step[i] = fall1 - rise2 + 1 - index;
		cout<<i<<"\t -> \t"<<index<<"\t"<<step[i]<<endl<<endl;
	}
}

void generate_DNL_INL(int bin_count[5000], int start, int stop){
	double ave_count = 0.0;
	for(int i=start;i<stop;i++) ave_count += bin_count[i];
	ave_count = ave_count/(stop-start);

	double DNL[5000], INL[5000];
	for(int i=start;i<stop;i++) DNL[i] = 1.0*bin_count[i]/ave_count - 1.0;
	INL[start] = 0.0;
	for(int i=start+1;i<stop;i++) INL[i] = INL[i-1] + DNL[i-1];

	TGraph *g_dnl = new TGraph();
	TGraph *g_inl = new TGraph();
	for(int i=start;i<stop;i++){ g_dnl->SetPoint(i-start,i,DNL[i]); g_inl->SetPoint(i-start,i,INL[i]); }
	
	TCanvas *cn = new TCanvas("cn","DNL and INL Plot",0,0,700,500);
	cn->Divide(2);
	cn->cd(1);
	g_dnl->Draw("APL");
	cn->cd(2);
	g_inl->Draw("APL");
}

void data_process_step2(){
	int step[32];
	//get_Step_by_Nearest_Area(step);
	get_Step_by_Minimize_Res_Area2(step);

	int acc_step[32];
	acc_step[0] = 0;
	for(int i=1;i<32;i++) acc_step[i] = acc_step[i-1] + step[i-1];
	const char *filename = "./noise_data.txt";
	ifstream fi;
	fi.open(filename);

	double vol, vres0, vres1;
	int	d0,d1,num=0;
	int bin_count[5000];
	for(int i=0;i<5000;i++){ bin_count[i] = 0; }

	TH1I *h1 = new TH1I("h1","h1",5000,0,5000);
	string line;
	while(getline(fi,line)){
		std::istringstream data_str(line);
		if(!(data_str>>vol>>d0>>d1>>vres0>>vres1)) break;
		int x = (int)(d0/2);
		int d = acc_step[x] + d1;
		if(d1==0|d1==255) continue;
		h1->Fill(d);
		bin_count[d] = bin_count[d] + 1;
	}
	h1->Draw();

	int max_code = acc_step[31]+254;
	generate_DNL_INL(bin_count,1,max_code);
}


