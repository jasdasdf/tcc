# -*- coding: utf-8 -*-
"""
Created on Tue Feb 18 17:20:25 2020

@author: RAFAEL
"""

import pathlib
import numpy as np

# diretório de trabalho
pasta_de_trabalho = pathlib.Path('C:/Users/RAFAEL/Desktop')

arqvs = list(pasta_de_trabalho.glob('*.npy'))

resultado_complex = []

for arq in arqvs:
    data = np.load(arq, allow_pickle=True)
    
    resultado_complex.append([arq.parts[-1][:-4], data])
    
    
# print(len(resultado_complex))
    
resultado=[]

for algoritmo in resultado_complex:
    
    # print(algoritmo[0])
    
    aux = []
    
    for ruido in algoritmo[1]:
        
        cont=1
        
        for port in ruido:
                        
            if cont<9:                
                show = 'Port_f%s'%cont
            else:
                show = 'Port_m%s'%(cont-8)   
                
            # print(show)
            cont = cont+1
            
            
            # limite inferior do teste em dB
            lim_inf_dB = -20
            
            # limite superior do teste em dB
            lim_sup_dB = 20
            
            # passo de dB em dB (de 5 em 5 --> jdB = 5)
            step_dB = 2     
            
            cont1=0
            
            for SNR in range(lim_inf_dB, lim_sup_dB+1, step_dB):
        
                # print("\n%s dB\n"%SNR)
                
                snr = SNR
                    
                if snr <0 and snr > -10:
                    # snr string
                    snr = '-0%s'%(abs(snr))
                elif snr >= 0 and snr <10:
                    snr = '0%s'%(snr)
                else:  
                    # snr string
                    snr = '%s'%(snr)
                
                aux = aux + port[cont1]
                
                
                # print(snr)
                
    media =  round(np.mean(aux))
    maximo = np.max(aux)
    minimo = np.min(aux)
    delta = maximo - minimo
    
    resultado.append([algoritmo[0], media, maximo, minimo, delta])             
                
                
print(resultado)
                
                
import matplotlib.pyplot as plt
import linse_utils as utils

fig, ax = plt.subplots()
width_n = 0.3

x_labels = ['NLMS', 'Shin', 'Benesty', 'Zipf']
y_labels = [resultado[0][1], resultado[2][1], resultado[1][1], resultado[3][1]]
ax.bar(x_labels,y_labels , width=width_n, color='grey')
# ax.bar(resultado[2][0],resultado[2][1] , width=width_n, color='grey')
# ax.bar(resultado[1][0],resultado[1][1] , width=width_n, color='grey')
# ax.bar(resultado[3][0],resultado[3][1] , width=width_n, color='grey')

ax.set_ylabel('Clocks/Frame')                
                
fig = utils.format_figure(fig, figsize=(9,5.5), scilimits=None)             
                
                
                
                
                
                
                
                
                
                
                
                