// this is the test program 

#include <iostream>
using namespace std;

#ifndef RECT_H
#define RECT_H

class	Rect{
	public:
		double width;
		double length;

		Rect(double wid, double len);
		double get_Area();
};

#endif
