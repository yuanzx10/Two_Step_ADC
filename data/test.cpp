
#include <iostream>
#include <fstream>
using namespace std;


void write_Array_into_File(int arr[][3]){
	ofstream fout;
	fout.open("array.txt");
	for(int i=0;i<3;i++){
		for(int j=0;j<3;j++) fout<<arr[i][j]<<"\t";
		fout<<endl;
	}
	fout.close();
}

void read_Array_from_File(int arr[][3]){
	ifstream fin;
	fin.open("array.txt");
	
	for(int i=0;i<3;i++){
		for(int j=0;j<3;j++) fin>>arr[i][j];
	}
	fin.close();
}

void test(){
	int arr[3][3] = { {1,2,3},{4,5,6},{7,8,9}};
	write_Array_into_File(arr);

	int array[3][3];
	read_Array_from_File(array);
	for(int i=0;i<3;i++){
		for(int j=0;j<3;j++) cout<<array[i][j]<<"\t";
		cout<<endl;
	}

}
