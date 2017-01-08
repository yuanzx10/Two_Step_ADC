// This is the test program of the SAR_ADC and Two_Step_ADC

#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>

#include "Data_Under_Given_Input.h"
#include "Ramp_Data_Process.h"

using namespace std;

void tester(){
	Data_Under_Given_Input data;
	Ramp_Data_Process tests;

	ifstream fin;
	fin.open("../../data/noise_data.txt");
		
	int num = 0;
	double voltage, vres0, vres1;
	int D0, D1;
	while(fin>>voltage>>D0>>D1>>vres0>>vres1){
		if(num==0) data.Vi = voltage;
		if(data.Vi == voltage){
			data.read_Data(D0,D1);	
		}
		else{
			data.process_Data();
			tests.add_New_Data(data);
			data.clear();
			data.Vi = voltage; data.read_Data(D0,D1);
		}
		
		num++; if(num%40000==0) cout<<num<<endl;
	}
	fin.close();
	
	cout<<"Starting Processing the datas.....Take a deep breathe!"<<endl;
	tests.get_ADC_DNL_by_Count();
}


















