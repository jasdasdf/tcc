# -*- coding: utf-8 -*-
"""
Created on Fri Mar 29 12:37:29 2019

@author: Pertum
"""
"""


        Edit: 15/02/2020 -> TCC

"""

import numpy as np
import soundfile as sf
from pystoi.stoi import stoi ## https://github.com/mpariente/pystoi
import os
import numpy as np
import matplotlib.pyplot as plt
from scipy import signal
import sys
import pathlib
import librosa 

#%%

                                    # METRICA STOI

# diretório de trabalho/desenvolviemnto
pasta_de_trabalho = pathlib.Path('C:/Users/RAFAEL/Desktop')

pasta_audios_resultados = pasta_de_trabalho / 'resultados'

sys.path.insert(0 ,'functions/')

# destino dos arquivos
resultados_metricas = pasta_de_trabalho / 'resultado_metricas'

# pasta STOI
pasta_STOI = resultados_metricas / 'STOI'

# cria a pasta para o resultados das métricas e a pasta STOI
pathlib.Path(resultados_metricas).mkdir(parents=True, exist_ok=True)
pathlib.Path(pasta_STOI).mkdir(parents=True, exist_ok=True)


# audios de referencia
pasta_audios_de_referencia = pasta_de_trabalho / 'referencia' / 'audios_ajustados'

# frequenia de amostragem
fs = 8000

# limite inferior do teste em dB
lim_inf_dB = -20

# limite superior do teste em dB
lim_sup_dB = 20

# passo de dB em dB (de 5 em 5 --> jdB = 5)
step_dB = 2

# lista o conteudo do diretório de resultados
diretorio = list(pasta_audios_resultados.glob('*/'))

# lista o diretório dos arquivos de referência
diretorio_de_referencia = list(pasta_audios_de_referencia.glob('*/'))

arquivos_de_referencia_contaminados = pasta_de_trabalho / 'referencia' / 'audios_contaminados'

aux = list(arquivos_de_referencia_contaminados.glob('*/'))



resultado_STOI = []

for i in diretorio: 
    
    resultado_ruido = []

    # tipo do ruído vigente na análise
    ruido = i.parts[-1]
    
    # mostra o tipo de ruido em analise
    print("Ruido de %s em análise\n "%ruido)
    
    # pasta do tipo de ruído em análise
    pasta_ruido = pasta_STOI / ruido 
    
    # cria a pasta do ruído em analise
    pathlib.Path(pasta_ruido).mkdir(parents=True, exist_ok=True)
            
    # lista os algorimtos presentes na pasta do ruído analisado
    algoritmos = list(i.glob('*/'))
    
    for j in algoritmos:
        
        resultado_algoritmo =[]
        
        # algoritmo em analise
        algoritmo = j.parts[-1]
       
        # cria a pasta do algoritmo na pasta do ruido em analise         
        pasta_algoritmo = pasta_ruido / algoritmo
        # pathlib.Path(pasta_algoritmo).mkdir(parents=True, exist_ok=True)
        
        # mostra o algoritmo em analise
        print("Algorimto %s em análise\n"%algoritmo)
        
        # lista as pastas 'Port' no diretório do algoritmo analisado
        ports = list(j.glob('*/'))
        
        for SNR in range(lim_inf_dB, lim_sup_dB+1, step_dB):
        
            print("%s dB\n"%SNR)
            
            aux = []
            for k in ports:
                
                port = k.parts[-1]
                print(port + '\n')
                
                snr = SNR
                
                if snr <0 and snr > -10:
                    # snr string
                    snr = '-0%s'%(abs(snr))
                elif snr >= 0 and snr <10:
                    snr = '0%s'%(snr)
                else:  
                    # snr string
                    snr = '%s'%(snr)
                    
                degradado = '%s_%s_dB.wav'%(algoritmo, snr)                 
                arquivo_degradado = k / degradado
                
                referencia = 'audios_ajustados_%s_dB.wav'%snr                
                arquivo_de_referencia = pasta_audios_de_referencia / ruido / port / referencia
                                             
                
                denoised, fs = librosa.core.load(arquivo_degradado, sr=fs)
                
                clean, fs = librosa.core.load(arquivo_de_referencia, sr=fs)
                
                score = stoi( clean, denoised, fs, extended=False)
                
                # print(score)
                
                aux.append(score)
                
            resultado_algoritmo.append(np.mean(aux))
            
        resultado_ruido.append([algoritmo,np.transpose(resultado_algoritmo)])
        
    resultado_STOI.append([ruido, resultado_ruido])
# print(resultado_STOI)
   

filename = pasta_STOI / 'STOI'
np.save(filename, resultado_STOI)            
        

#%%
#    figure generation
    
import linse_utils as utils
import pickle
import numpy as np

import matplotlib.pyplot as plt


# limite inferior do teste em dB
lim_inf_dB = -12

# limite superior do teste em dB
lim_sup_dB = 20

# passo de dB em dB (de 5 em 5 --> jdB = 5)
step_dB = 2

# diretório de trabalho
pasta_de_trabalho = pathlib.Path('C:/Users/RAFAEL/Desktop')

# pasta dos resultados
pasta_audios_resultados = pasta_de_trabalho / 'resultados'

# destino dos arquivos
resultados_metricas = pasta_de_trabalho / 'resultado_metricas'

# pasta dos resultados da metodologia utilizando STOI
pasta_STOI = resultados_metricas / 'STOI'

filename = pasta_STOI / 'STOI.npy'


resultado_STOI = np.load(filename, allow_pickle=True)

# tamanho da figura
# plt.rcParams['figure.figsize'] = (5.54, 3.3622)

# tamanho da fonte da legenda
plt.rcParams['legend.fontsize'] = 10

label_error = "ANC Desligado"
label_nlms = "NLMS"
label_npvss = "VSS-NLMS de Benesty"
label_jo_nlms = "JO-NLMS"
label_vss_nlms = "VSS NLMS de Shin"
label_vss_nlms_zipf = "VSS NLMS de Zipf"

for ruido in range(len(resultado_STOI)):

    STOI_result = resultado_STOI[ruido][1]
    
    Title =  resultado_STOI[ruido][0]
    
    print(Title)
    
    fig, ax = plt.subplots()
    
    for i in range(len(STOI_result)):
        legend = STOI_result[i][0]
        y = STOI_result[i][1][:]
        y = y[4:]
        x = range(lim_inf_dB, lim_sup_dB + 1, step_dB)
        
        
        if legend =='nlms':
            legend1 = label_nlms
            line1, = ax.plot(x, y, '-', marker = None,  color=[0,0,0], linewidth=1.2)
            
        elif legend =='vss_nlms_shin':
            legend2 = label_vss_nlms
            line2, =ax.plot(x, y, '-.', marker = None,  color=[0,0,0], linewidth=1.2)        
        
        elif legend == 'npvss_nlms':
            legend3 = label_npvss
            line3, = ax.plot(x, y, '--', marker = None,  color=[0,0,0], linewidth=1.2)
            
        elif legend == 'vss_nlms_zipf':
            legend4 = label_vss_nlms_zipf
            line4, = ax.plot(x, y, ':', marker = None,  color=[0,0,0], linewidth=1.2)        
                
        elif legend =='audios_contaminados':
            legend5 = label_error
            line5, = ax.plot(x, y, '-', marker = '^',  markersize=3.5, color=[0,0,0], linewidth=1.2)
    
    ax.set_ylabel('STOI')
    ax.set_xlabel('SNR (dB)')
    ax.axis([lim_inf_dB, lim_sup_dB,0,1])
    
    ax.set_xticks(np.arange(lim_inf_dB, lim_sup_dB + 1, 4 ))
    ax.set_yticks(np.arange(0, 1.01, 0.25))
    
    ax.legend((line1, line2, line3, line4, line5), (legend1, legend2, legend3, legend4, legend5),
              loc='lower right', fontsize=7)
    
    
    
    fig = utils.format_figure(fig, figsize=(9,5.5))
    
    filename = "STOI_%s.pdf"%Title
    fig.savefig(filename, format='pdf')

# fig.savefig('plot_paper(9x5.5).pdf', format='pdf')
# fig.savefig('plot_paper(9x5.5).png', format='png')
# fig.savefig('plot_paper(9x5.5).eps', format='eps')
# fig.savefig('plot_screen(16x10).pdf', format='pdf')
# fig.savefig('plot_screen(16x10).png', format='png')
    