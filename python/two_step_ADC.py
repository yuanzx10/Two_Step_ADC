#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
This is the class of the two step SAR ADC, redundancy is enable.
For a two step SAR ADC, the most important parameters are:
    Nbit0, Nbit1: Number of bits of the ADC0 and ADC1
	Cmis0, Cmis1: Mismatches from the ideal case
	Cpar0, Cpar1: Parasitic capacitance at the comparators input node
	Voff0, Voff1: Offset voltage of the comparators
	Vsig0, Vsig1: Sigma of noise of the ADC
	Gain		: Gain of the residue amplifier
	Voff		: Offset of the residue amplifier

From the above parameters, we can fully define the two-step SAR ADC. Based on that, we can get some important features of the ADC, like:
	DNL/INL		: DNL/INL caused by the capacitor mismistch and parasitics
	
In order to get all the above advanced parameters, some methods are needs:


KEEP IN MIND:
    [0] is the LSB!

@author: yuanzx
"""

from SAR_ADC import SAR_ADC

class Two_Step_ADC(object):
    # init the class
    def __init__(self, Nbit0, Nbit1):
        self.Nbit0 = Nbit0
        self.Nbit1 = Nbit1
        self.ADC0 = SAR_ADC(Nbit0)  # ADC0
        self.ADC1 = SAR_ADC(Nbit1)  # ADC1
        self.Gain = 0.0             # amplifier
        self.Voff = 0.0

    # set the parameters of the two-step ADCs
    def set_Cap_Mismatches(self, Cmis0, Cmis1):
        self.ADC0.set_Cap_Mismatch(Cmis0)
        self.ADC1.set_Cap_Mismatch(Cmis1)

    def set_Parasitics(self, Cpar0, Cpar1):
        self.ADC0.set_Parasitic(Cpar0)	
        self.ADC1.set_Parasitic(Cpar0)	

    def set_ADCs_Offset(self, Voff0, Voff1):
        self.ADC0.set_Offset(Voff0)	
        self.ADC1.set_Offset(Voff1)
        
    def set_Noise_Sigmas(self, Vsig0, Vsig1):
        self.ADC0.set_Noise_Sigma(Vsig0)	
        self.ADC1.set_Noise_Sigma(Vsig1)

    def set_Amplifier(self, Gain, Voff):
        self.Gain = Gain
        self.Voff = Voff

    def gen_Code_without_Noise(self, vi):
        # ADCs preparations
        self.ADC0.gen_VDAC_and_LSB()
        self.ADC1.gen_VDAC_and_LSB()
        code0, vres0 = self.ADC0.gen_Code_and_Vres_Without_Noise(vi)
        vres = self.Gain*vres0 + self.Voff
        code1, vres1 = self.ADC1.gen_Code_and_Vres_Without_Noise(vres)
        return code0, code1

    def gen_Code_with_Noise(self, vi):
        # ADCs preparations
        self.ADC0.gen_VDAC_and_LSB()
        self.ADC1.gen_VDAC_and_LSB()
        code0, vres0 = self.ADC0.gen_Code_and_Vres_With_Noise(vi)
        vres = self.Gain*vres0 + self.Voff
        code1, vres1 = self.ADC1.gen_Code_and_Vres_With_Noise(vres)
        return code0, code1




















