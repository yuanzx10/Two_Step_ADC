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

# ideal case
gain_error = 0.0
gain = math.pow(2,Nbit0-2)*(1+gain_error)
ADC.set_Amplifier(gain,0)

ivin = []
idout0 = []
idout1 = []
idout = []

npoints = 20000
for i in range(-npoints,npoints,1):
    vin_tem = 1.15*i/npoints 
    ivin.append(vin_tem)
    d0,d1 = ADC.gen_Code_without_Noise(vin_tem)
    idout0.append(d0)
    idout1.append(d1-16)
    idout.append(int(d0/2)*math.pow(2,Nbit0-1)+d1)

# without gain error
gain_error = 0.0
amp_offset = 0.0
gain = math.pow(2,Nbit0-2)*(1+gain_error)
ADC.set_Amplifier(gain,amp_offset)
Voff1 = math.pow(2,-Nbit0)/1.5
ADC.set_ADCs_Offset(Voff1,0)


rvin = []
rdout0 = []
rdout1 = []
rdout = []
npoints = 20000
for i in range(-npoints,npoints,1):
    vin_tem = 1.15*i/npoints 
    rvin.append(vin_tem)
    d0,d1 = ADC.gen_Code_without_Noise(vin_tem)
    rdout0.append(d0)
    rdout1.append(d1-16)
    rdout.append(int(d0/2)*math.pow(2,Nbit0-1)+d1)

plt.close('all')
plt.figure(figsize=(7,5),dpi=150)
plt.plot(rvin,rdout0,color="red",  linewidth=1.5, linestyle="-",label=r'wo/ 1st ADC offset')
plt.plot(ivin,idout0,color="green",  linewidth=1., linestyle="-",label=r'w/ 1st ADC offset')
plt.legend(loc='upper left', frameon=False)
plt.plot(rvin,rdout1,color="red",  linewidth=1.5, linestyle="-",label='wo/ 1st ADC offset')
plt.plot(ivin,idout1,color="green",  linewidth=1., linestyle="-",label='w/ 1st ADC offset')
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
plt.plot(ivin,idout,color="green",  linewidth=2, linestyle="-",label=r'wo/ 1st ADC Offset')
plt.plot(rvin,rdout,color="red",  linewidth=1., linestyle="-",label=r'wo/ 1st ADC Offset')
plt.legend(loc='upper left', frameon=False)
plt.xlabel(r'$V_{in}$')
plt.xlim(-1.15,1.15)
plt.xticks([-1,-0.75,-0.5,-0.25,0,0.25,0.5,0.75,1],
           [r'-$V_r$',r'-$3V_r/4$',r'-$V_r/2$',r'-$V_r/4$',r'$0$'
            ,r'$V_r/4$',r'$V_r/2$',r'$3V_r/4$',r'$V_r$'])
plt.grid(True)