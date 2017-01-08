#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
This is the definition of binary MCS SAR ADC.

Please Keep In Mind: [0] represents the LSB!

Author: Zhen-Xiong Yuan
"""

import math
import random

class SAR_ADC(object):
    def __init__(self, Nbit):
        self.Nbit = Nbit
        self.codes = int(math.pow(2,Nbit))
        self.Cap = [math.pow(2,i) for i in range(Nbit-1)]
        self.Cmis = [0.0*i for i in range(Nbit-1)]
        self.Cpar = 0.0
        self.Voff = 0.0
        self.Vsig = 0.0

        self.VDAC = [0.0*i for i in range(Nbit-1)]
        self.DNL = [0.0*i for i in range(self.codes)]
        self.INL = [0.0*i for i in range(self.codes)]
        self.LSB = 0.0

    def set_Cap_Mismatch(self, Cmis):
        self.Cmis = Cmis

    def set_Offset(self, Voff):
        self.Voff = Voff

    def set_Parasitic(self, Cpar):
        self.Cpar = Cpar

    def set_Noise_Sigma(self, Vsig):
        self.Vsig = Vsig

    def gen_VDAC_and_LSB(self):
        Csum = self.Cpar + 1.0
        for i in range(self.Nbit-1): Csum += self.Cap[i] + self.Cmis[i]
        VDAC_sum = 0.0
        for i in range(self.Nbit-1):
            self.VDAC[i] = 1.0*(self.Cap[i] + self.Cmis[i])/Csum
            VDAC_sum += self.VDAC[i]
    
        self.LSB = 2.0*VDAC_sum/(self.codes - 2)

    def sort_Code(self,code):
        num = int(code)
        bit0 = num%2
        for i in range(self.Nbit):
            if num%2 != bit0:
                return i-1
            num = int(num/2)
        
    def gen_DNL(self):
        delta_VDAC = [0.0*i for i in range(self.Nbit-1)]
        delta_VDAC[0] = self.VDAC[0]
        delta_sum = delta_VDAC[0]
        for i in range(1,self.Nbit-1):
            delta_VDAC[i] = self.VDAC[i] - delta_sum
            delta_sum += self.VDAC[i]
            print("delta_VDAC[",i,"]=",delta_VDAC[i])
        self.DNL[0] = 0.0
        self.DNL[self.codes-1] = 0.0
        for i in range(1,self.codes-1):
            sort_code = self.sort_Code(i)
            self.DNL[i] = 1.0*delta_VDAC[sort_code]/self.LSB - 1.0

    def gen_INL(self):
        self.INL[0] = 0.0
        for i in range(1,self.codes):
            self.INL[i] = self.INL[i-1] + self.DNL[i-1]

    def gen_Code_and_Vres_Without_Noise(self, vi):
        code = [0*i for i in range(self.Nbit)] # keep in mind MSB is [0]
        vres = vi
        for i in range(self.Nbit-1):
            if (vres>self.Voff): code[i]=1
            vres -= (2*code[i]-1)*self.VDAC[self.Nbit-2-i]
        if (vres>self.Voff): code[self.Nbit-1] = 1
        value = 0
        for i in range(self.Nbit): 
            value += math.pow(2,self.Nbit-1-i)*code[i]
        return int(value), vres 

    def gen_Code_and_Vres_With_Noise(self, vi):
        code = [0*i for i in range(self.Nbit)] # keep in mind MSB is [0]
        vres = vi
        for i in range(self.Nbit-1):
            if (vres>self.Voff+random.gauss(0.0,self.Vsig)): code[i]=1
            vres -= (2*code[i]-1)*self.VDAC[self.Nbit-2-i]
        if (vres>self.Voff+random.gauss(0.0,self.Vsig)): 
            code[self.Nbit-1] = 1
        value = 0
        for i in range(self.Nbit): 
            value += math.pow(2,self.Nbit-1-i)*code[i]
        return int(value), vres 





















