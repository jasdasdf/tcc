# -*- coding: utf-8 -*-
"""
Created on Fri Feb  1 23:55:53 2019

@author: Pertum

Avaliação

"""
"""
 referência de como compilar e utilizar PESQ.exe
 https://stackoverflow.com/questions/2329403/how-to-start-a-voice-quality-pesq-test
 
"""
"""

        Edit: 13/02/2020 -> TCC
"""
#%%


# modulos e bibliotecas
import numpy     as np
# from scipy.io import wavfile
import os
import shutil
import re
import sys
import matplotlib.pyplot as plt
# import soundfile as sf # https://pysoundfile.readthedocs.io/en/0.9.0/
import pathlib
import pathlib

#%%

# diretório de trabalho/desenvolviemnto
pasta_de_trabalho = pathlib.Path('C:/Users/RAFAEL/Desktop')

pasta_audios_resultados = pasta_de_trabalho / 'resultados'

sys.path.insert(0 ,'functions/')

# destino dos arquivos
resultados_metricas = pasta_de_trabalho / 'resultado_metricas'

# pasta PESQ
pasta_PESQ = resultados_metricas / 'PESQ'

# cria a pasta para o resultados das métricas e a pasta PESQ
pathlib.Path(resultados_metricas).mkdir(parents=True, exist_ok=True)
pathlib.Path(pasta_PESQ).mkdir(parents=True, exist_ok=True)

# vetor de resultados
results = []

# frequenia de amostragem
fs = 8000

# executavel PESQ.exe
PESQ = pasta_de_trabalho / 'PESQ' / 'PESQ.exe'

# audios de referencia
pasta_audios_de_referencia = pasta_de_trabalho / 'referencia' / 'audios_ajustados'

sample_rate = "+8000"

# # nome dado ao arquivo de resultados pelo PESQ.exe
name = "pesq_results.txt"

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

# copia executavel PESQ.exe para a pasta de trabalho
shutil.copy(PESQ, pasta_de_trabalho)

#%%
for i in diretorio: 

    # tipo do ruído vigente na análise
    ruido = i.parts[-1]
    
    # mostra o tipo de ruido em analise
    print("Ruido de %s em análise\n "%ruido)
    
    # pasta do tipo de ruído em análise
    pasta_ruido = pasta_PESQ / ruido
    
    # cria a pasta do ruído em analise
    pathlib.Path(pasta_ruido).mkdir(parents=True, exist_ok=True)
            
    # lista os algorimtos presentes na pasta do ruído analisado
    algoritmos = list(i.glob('*/'))
    
    for j in algoritmos:
        
        # algoritmo em analise
        algoritmo = j.parts[-1]
       
        # cria a pasta do algoritmo na pasta do ruido em analise         
        pasta_algoritmo = pasta_ruido / algoritmo
        pathlib.Path(pasta_algoritmo).mkdir(parents=True, exist_ok=True)
        
        # mostra o algoritmo em analise
        print("Algorimto %s em análise\n"%algoritmo)
        
        # lista as pastas 'Port' no diretório do algoritmo analisado
        ports = list(j.glob('*/'))
        
        for SNR in range(lim_inf_dB, lim_sup_dB+1, step_dB):
        
            print("%s dB\n"%SNR)
            
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
                                             
                # copia arquivo degradado para a pasta de trabalho
                shutil.copy(arquivo_degradado, pasta_de_trabalho)
                
                # copia arquivo de referencia para a pasta de trabalho
                shutil.copy(arquivo_de_referencia, pasta_de_trabalho)
                
                rename = '%s_%s_%s_dB.wav'%( algoritmo, port, snr)  
                os.rename(degradado, rename)
                degradado = rename                
                
                rename = 'audios_ajustados_%s_%s_dB.wav'%(port, snr)                
                os.rename(referencia, rename)
                referencia = rename
                
                # executa PESQ.exe 
                execute = 'PESQ.exe %s %s %s'%(sample_rate, referencia, degradado)
                os.system(execute)
                
                os.remove(referencia)
                os.remove(degradado)
            
            # renomeia o arquivo txt
            rename = "pesq_results_%s_%s_dB.txt"%(algoritmo, snr)
            os.rename(name, rename) 
            
            # copia o arquivo renomeado para a pasta resultados da métrica
            shutil.copy(rename, pasta_algoritmo)
            
            # remove o arquivo
            os.remove(rename)
    
#%%
        
#                           PESQ REFERÊNCIA
        
pasta_audios_resultados = pasta_de_trabalho / 'referencia' / 'audios_contaminados'

diretorio = list(pasta_audios_resultados.glob('*/'))

for i in diretorio: 

    # tipo do ruído vigente na análise
    ruido = i.parts[-1]
    
    # mostra o tipo de ruido em analise
    print("Ruido de %s em análise\n "%ruido)
    
    # pasta do tipo de ruído em análise
    pasta_ruido = pasta_PESQ / ruido
    
    # cria a pasta do ruído em analise
    pathlib.Path(pasta_ruido).mkdir(parents=True, exist_ok=True)
            
    # lista os algorimtos presentes na pasta do ruído analisado
    ports = list(i.glob('*/'))
    
    algoritmo = 'audios_contaminados'
    
    # cria a pasta do algoritmo na pasta do ruido em analise         
    pasta_algoritmo = pasta_ruido / algoritmo
    pathlib.Path(pasta_algoritmo).mkdir(parents=True, exist_ok=True)
        
    for SNR in range(lim_inf_dB, lim_sup_dB+1, step_dB):
    
        print("%s dB\n"%SNR)
        
        for k in ports:
            
            port = k.parts[-1]
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
                                         
            # copia arquivo degradado para a pasta de trabalho
            shutil.copy(arquivo_degradado, pasta_de_trabalho)
            
            # copia arquivo de referencia para a pasta de trabalho
            shutil.copy(arquivo_de_referencia, pasta_de_trabalho)
            
            rename = '%s_%s_%s_dB.wav'%(algoritmo, port,  snr)  
            os.rename(degradado, rename)
            degradado = rename                
            
            rename = 'audios_ajustados_%s_%s_dB.wav'%(port, snr)                
            os.rename(referencia, rename)
            referencia = rename
            
            # executa PESQ.exe 
            execute = 'PESQ.exe %s %s %s'%(sample_rate, referencia, degradado)
            os.system(execute)
            
            os.remove(referencia)
            os.remove(degradado)
        
        # renomeia o arquivo txt
        rename = "pesq_results_%s_%s_dB.txt"%(algoritmo, snr)
        os.rename(name, rename) 
        
        # copia o arquivo renomeado para a pasta resultados da métrica
        shutil.copy(rename, pasta_algoritmo)
        
        # remove o arquivo
        os.remove(rename)

        

                       ## EXTRAIR OS DADOS DOS ARQUIVOS .txt
            
# diretório de trabalho/desenvolviemnto
pasta_de_trabalho = pathlib.Path('C:/Users/RAFAEL/Desktop')

# destino dos arquivos
resultados_metricas = pasta_de_trabalho / 'resultado_metricas'

# pasta PESQ
pasta_PESQ = resultados_metricas / 'PESQ'
 

# limite inferior do teste em dB
lim_inf_dB = -20

# limite superior do teste em dB
lim_sup_dB = 20

# passo de dB em dB (de 5 em 5 --> jdB = 5)
step_dB = 2       

resultado_PESQ = []

ruidos = list(pasta_PESQ.glob('*/'))

for i in ruidos:
    
    resultado_ruido = []
    
    ruido = i.parts[-1]
    
    algoritmos  = list(i.glob('*/'))
    
    print(ruido)
    
    for j in algoritmos:
        
        resultado_algoritmo = []
        
        algoritmo = j.parts[-1]
        
        print(algoritmo)
        
        for SNR in range(lim_inf_dB, lim_sup_dB + 1, step_dB):
            
            snr = SNR
            print(snr)
         
            if snr <0 and snr > -10:
                # snr string
                snr = '-0%s'%(abs(snr))
            elif snr >= 0 and snr <10:
                snr = '0%s'%(snr)
            else:  
               # snr string
               snr = '%s'%(snr)
            
            txt = 'pesq_results_%s_%s_dB.txt'%(algoritmo, snr)
            
            arq = j / txt
            
            file = open(arq, 'r')
            
            # armazen linha por linha no array text
            text = file.readlines()
            text = text[1:]
            pesqmos=[]
            moslqo=[]
            
            for line in text:
                # identifica os números
                data = [s for s in re.findall( "[+-]?\d+\.\d+\d+\d+", line)]
                
                pesqmos.append(data[0])
                moslqo.append(data[1])
                
               
            file.close()
                
            pesqmos = np.float32(pesqmos)
            moslqo = np.float32(moslqo)
            m1 = np.mean(pesqmos)      
            m2 = np.mean(moslqo)
            
            resultado_algoritmo.append(m1)
            
        resultado_ruido.append([algoritmo,np.transpose(resultado_algoritmo)])
        
    resultado_PESQ.append([ruido, resultado_ruido])
# print(resultado_PESQ)

filename = pasta_PESQ / 'PESQ'
np.save(filename, resultado_PESQ)
  
#%%
#    figure generation
    
    # introdução aos gráficos utilizando python  
# http://donald.catalao.ufg.br/up/633/o/introducaoAosGraficos.pdf?1417367842

# tutorial matplotlib
#https://matplotlib.org/tutorials/introductory/usage.html#sphx-glr-tutorials-introductory-usage-py

# cores hexadecimal
#http://erikasarti.com/html/tabela-cores/

# abre o arquivo dos resultados da PESQ
# pasta_PESQ = resultados_metricas / 'PESQ'
# filename = pasta_PESQ / 'PESQ.npy'
# resultado_PESQ = np.load(filename)

# tamanho da figura
plt.rcParams['figure.figsize'] = (5.54, 3.3622)

# tipo de fonte times new roman    
plt.rcParams["font.family"] = "Times New Roman"

# tamanho da fonte da legenda
plt.rcParams['legend.fontsize'] = 10

label_error = "ANC Desligado"
label_nlms = "NLMS"
label_npvss = "VSS-NLMS de Benesty"
label_jo_nlms = "JO-NLMS"
label_vss_nlms = "VSS NLMS de Shin"
label_vss_nlms_zipf = "VSS NLMS de Zipf"

ruido = 2

PESQ_result = resultado_PESQ[ruido][1]

Title =  resultado_PESQ[ruido][0]

for i in range(len(PESQ_result)):
    legend = PESQ_result[i][0]
    y = PESQ_result[i][1][:]
    x = range(lim_inf_dB, lim_sup_dB + 1, step_dB)
    
    if legend =='nlms':
        legend1 = label_nlms
        line1, = plt.plot(x, y, '-', marker = None,  color=[0,0,0], linewidth=1.2)
        
    elif legend =='vss_nlms_shin':
        legend2 = label_vss_nlms
        line2, = plt.plot(x, y, '-.', marker = None,  color=[0,0,0], linewidth=1.2)        
    
    elif legend == 'npvss_nlms':
        legend3 = label_npvss
        line3, = plt.plot(x, y, '--', marker = None,  color=[0,0,0], linewidth=1.2)
    
    elif legend == 'vss_nlms_zipf':
        legend4 = label_vss_nlms_zipf
        line4, = plt.plot(x, y, ':', marker = None,  color=[0,0,0], linewidth=1.2)        
    

        
    elif legend =='audios_contaminados':
        legend5 = label_error
        line5, = plt.plot(x, y, ':', marker = '+',  color=[0,0,0], linewidth=1.2)
    
#ax1.plot(x, y1, color=[0.5, 0.5, 0.5], linewidth=1.2)
plt.xlabel('SNR (dB)', fontsize=10), plt.ylabel('PESQ - LQO', fontsize=10)

# 
plt.title(Title, loc='center')  


# melhor lugar de legenda
#
plt.legend((line1, line2, line3, line4, line5), (legend1, legend2, legend3, legend4, legend5), loc='best')

# grades
#plt.grid()

snr = list(x)
#plt.xticks(snr,[str(i) for i in snr], fontsize=12)
plt.xticks(np.arange(-20, 21, 4 ), fontsize=8)
plt.yticks(np.arange(1, 5, 0.5), fontsize=8)

# limite dos eixos
plt.axis([lim_inf_dB, lim_sup_dB, 1, 4.5], option='image')

#plt.show()

#%%
#salvar figura
#https://matplotlib.org/api/_as_gen/matplotlib.pyplot.savefig.html
#plt.savefig('result.jpg')

#plt.savefig("./PESQ_result.tiff")

#plt.savefig("./PESQ_result.jpg")


plt.savefig("./PESQ_result.tiff", dpi=None, quality=100 , facecolor=None, edgecolor=None,
        orientation='portrait', papertype=None, format='tiff',
        transparent=True, bbox_inches=False,
        frameon=None, metadata=None)