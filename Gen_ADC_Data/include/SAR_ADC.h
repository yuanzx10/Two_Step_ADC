// This is the definition of binary MCS SAR ADC
// Please Keep in Mind: [0] represents the LSB

#include <vector>
#include <random>
using namespace std;

#ifndef SAR_ADC_H
#define SAR_ADC_H

class	SAR_ADC{
	private:
		int Nbit;			// number of bit of the ADC
		int codes;			// number of codes
		double Cpar;			// Parasitic capacitance 
		double Voff;			// Comparator's offset
		double Vsig;			// sigma of noise during compare
		//std::default_random_engine generator;
		vector<double> Cap ;		// nominal capacitor values
		vector<double> Cmis;		// mismatches of the capacitors
		vector<double> VDAC;		// voltage change due to bit conversion
		int  sort_Code(int icode);	// sort the code		
		double get_Up_Limit_for_Code(int icode);	

	public:
		double LSB;			// LSB of the ADC
		vector<double> DNL;		// DNL
		vector<double> INL;		// INL
		vector<double> Up_Limit;	// Up boundary of the code
		
		SAR_ADC(int nbit);		// constructor
	
		void set_Cap_Mismatch(vector<double>& cmis);
		void set_Offset(double voff);
		void set_Parasitic(double cpar);
		void set_Noise_Sigma(double vsig);
		void gen_VDAC_and_LSB();
		
		void gen_DNL_and_INL();
		int  gen_Code_and_Vres_without_noise(double vi, double& vres);
		int  gen_Code_and_Vres_with_noise(double vi, double& vres);
		
		void get_Up_Limit();
};

#endif
