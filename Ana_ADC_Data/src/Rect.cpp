//
// this is also a test program

#include <iostream>
#include "Rect.h"
using namespace std;

Rect::Rect(double wid, double len){
	width = wid;
	length = len;
}

double Rect::get_Area(){
	return width*length;
}
