#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
The first sub-ADC's comparator offset
@author: yuanzx
"""

from two_step_ADC import Two_Step_ADC
import matplotlib.pyplot as plt
import math

Nbit0 = 4
Nbit1 = 4
ADC = Two_Step_ADC(Nbit0,Nbit1)

gain_error = 0.0
gain = math.pow(2,Nbit0-2)*(1+gain_error)
ADC.set_Amplifier(gain,0)

Vsig0 = 1/200
Vsig1 = 0.04
ADC.set_Noise_Sigmas(Vsig0,Vsig1)

vin = []
dout0 = []
dout1 = []
dout = []

npoints = 1000
for i in range(-npoints,npoints,1):
    vin_tem = 1.15*i/npoints
    for j in range(200):
        vin.append(vin_tem)
        d0,d1 = ADC.gen_Code_with_Noise(vin_tem)
        dout0.append(d0)
        dout1.append(d1-16)
        dout.append(int(d0/2)*math.pow(2,Nbit0-1)+d1)

plt.close('all')
plt.figure(figsize=(7,5),dpi=150)
plt.plot(vin,dout0,color="green",  linewidth=1., linestyle="-",label=r'w/ 1st ADC offset')
plt.legend(loc='upper left', frameon=False)
plt.plot(vin,dout1,color="blue",  linewidth=1.5, linestyle="-",label='wo/ 1st ADC offset')
plt.ylabel(r'$2^{nd} ADC\; Code\qquad \qquad \qquad 1^{st} ADC\; codes$')
plt.ylim(-16,16)
plt.yticks([-14,-12,-10,-8,-6,-4,-2,2,4,6,8,10,12,14],
           [r'0010',r'0100',r'0110',r'1000',r'1010',r'1100',r'1110',r'0010'
            ,r'0100',r'0110',r'1000',r'1010',r'1100',r'1110'])
plt.xlabel(r'$V_{in}$')
plt.xlim(-1.15,1.15)
plt.xticks([-1,-0.75,-0.5,-0.25,0,0.25,0.5,0.75,1],
           [r'-$V_r$',r'-$3V_r/4$',r'-$V_r/2$',r'-$V_r/4$',r'$0$'
            ,r'$V_r/4$',r'$V_r/2$',r'$3V_r/4$',r'$V_r$'])
plt.grid(True)

plt.figure(figsize=(7,5),dpi=150)
plt.plot(vin,dout,color="green", linewidth=2, linestyle="-",label=r'wo/ 1st ADC Offset')
plt.legend(loc='upper left', frameon=False)
plt.xlabel(r'$V_{in}$')
plt.xlim(-1.15,1.15)
plt.xticks([-1,-0.75,-0.5,-0.25,0,0.25,0.5,0.75,1],
           [r'-$V_r$',r'-$3V_r/4$',r'-$V_r/2$',r'-$V_r/4$',r'$0$'
            ,r'$V_r/4$',r'$V_r/2$',r'$3V_r/4$',r'$V_r$'])
plt.grid(True)