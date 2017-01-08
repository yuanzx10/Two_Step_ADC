#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sat Dec 31 09:27:36 2016

@author: yuanzx
"""

from two_step_ADC import Two_Step_ADC
import math

Nbit0 = 6
Nbit1 = 8
ADC = Two_Step_ADC(Nbit0,Nbit1)

gain_error = 0.0
gain = math.pow(2,Nbit0-2)*(1+gain_error)
ADC.set_Amplifier(gain,0)

Vsig0 = 1.0/12/math.pow(2,Nbit0-1)
Vsig1 = 1.0/math.pow(2,Nbit1-1)
ADC.set_Noise_Sigmas(Vsig0,Vsig1)

file = open("ADC_Ramp_Data.txt","w")
file.truncate()

npoints = int(12*math.pow(2,Nbit0+Nbit1))
for i in range(-npoints,npoints,1):
    vin_tem = 1.05*i/npoints
    if(int(i%1000)): print("Read %d datas",i)
    for j in range(200):
        d0,d1 = ADC.gen_Code_with_Noise(vin_tem)
        myStr = str(vin_tem) + "\t" + str(d0) + "\t" + str(d1) + "\n"
        file.write(myStr)

file.close()