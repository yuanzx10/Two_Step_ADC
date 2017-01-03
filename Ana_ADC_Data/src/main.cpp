// this is the test file
//

#include <iostream>
#include "Rect.h"

#include "TF1.h"
#include "TCanvas.h"
using namespace std;

int main(){
	Rect rect(2,3);
	double area = rect.get_Area();
	cout<<area<<endl;
	
	TCanvas c1;
	TF1* f1 = new TF1("f1","sin(x)/x",0,10);
	f1->Draw();

	int a; cin>>a;
}


