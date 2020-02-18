# -*- coding: utf-8 -*-
"""
Created on Sat Mar  2 22:31:34 2019

@author: Pertum
"""
"""
        Edit: 15/02/2020 -> TCC

"""



# modulos importados
import numpy as np
import speech_recognition as sr
import os
import sys
import collections as cl
import matplotlib.pylab as plt
from matplotlib.ticker import PercentFormatter
from   scipy import signal
import soundfile as sf
import pathlib
import pickle

# diretório de trabalho
pasta_de_trabalho = pathlib.Path('C:/Users/RAFAEL/Desktop')

# pasta dos resultados
pasta_audios_resultados = pasta_de_trabalho / 'resultados'

# destino dos arquivos
resultados_metricas = pasta_de_trabalho / 'resultado_metricas'

# pasta dos resultados da metodologia utilizando Google API
pasta_API = resultados_metricas / 'Google_API'

pathlib.Path(pasta_API).mkdir(parents=True, exist_ok=True)

# audios de referencia
pasta_audios_de_referencia = pasta_de_trabalho / 'referencia' / 'audios_ajustados'

# adiciona a pasta functions aos diretórios de pesquisa
sys.path.insert(0 ,'./functions')

# importa o módulo my_functions (contido na pasta functions)
import my_functions as mf


# limite inferior do teste em dB
lim_inf_dB = -20

# limite superior do teste em dB
lim_sup_dB = 20

# passo de dB em dB 
step_dB = 2


# cria parametro de reconhecimento de fala
r = sr.Recognizer()


# cria arrays para os resultdos, transcrições e confiança
results = []
transcript=[]
confidence=[]

# dicionário
dicionario=[]

# arquivo que cotem as frases originais da itu-t p50
arq = pasta_de_trabalho / 'tcc' / 'ITUT_P50.txt'

# abre o arquivo no modo read (leitura)
file = open(arq, 'r')

# le as linhas do arquivo para a variável file
file = file.readlines()

# cria um dicionário com as palavras nas frases
# contabilizando como uma nova palavra aquela que possui:
# 1. mais de 3 letras
# 2. não esteja adicionada ao dicionário ainda

for line in file:
    sentence = line.lower().split()
    for word in sentence:
        if ((len(word) > 3) and (word not in dicionario)):
            dicionario.append(word)

# dicionário com as palavras 
dicionario =dict([(word, 0) for word in dicionario])

filename = pasta_API / 'dicionario'



with open(filename, 'wb') as f:
    pickle.dump(dicionario, f)
    

'''
 o dictionary possui as palavras identificaveis que possuem mais de 3 letras
 que estão presentes na transcrição original da ITU
'''


                # gerar os arquivos com as palavras de referencia

with open(filename, 'rb') as f:
    dicionario = pickle.load(f)

# arquivo que cotem as frases originais da itu-t p50
arq = "./tcc/ITUT_P50.txt"

# abre o arquivo no modo read (leitura)
file = open(arq, 'r')

# le as linhas do arquivo para a variável file
file = file.readlines()
# file = file[:-1]

diretorio = list(pasta_audios_de_referencia.glob('*/'))

for i in diretorio:
    
    ruido = i.parts[-1]
    
    pasta_ruido = list(i.glob('*/'))
    
    pasta_ruido_referencia = pasta_API / ruido 
    
    pathlib.Path(pasta_ruido_referencia).mkdir(parents=True, exist_ok=True)

    show = '\n\n%s em análise\n\n'%ruido
    print(show)
    
    for snr in range(lim_inf_dB, lim_sup_dB+1, step_dB):
        
        with open(filename, 'rb') as f:
            dicionario = pickle.load(f)
        
        # cria um dicionario vazio para:

        # palavras lidas
        words_read = dicionario.copy()
        
        # palavras erroneamente transcriptas
        wrong_words = dicionario.copy()
        
        # palavras corretamente transcriptas
        right_words = dicionario.copy()
    
        if snr <0 and snr > -10:
            # snr string
            snr = '-0%s'%(abs(snr))
        elif snr >= 0 and snr <10:
            snr = '0%s'%(snr)
        else:  
            # snr string
            snr = '%s'%(snr)
        
        cont = 0
        
        show = '\nSNR de %s dB\n'%snr
        print(show)
        
        for j in pasta_ruido:
            port = j.parts[-1]
            
            original = file[cont]
            
            filename = 'audios_ajustados_%s_dB.wav' %snr
            wav = pasta_audios_de_referencia / ruido / port / filename
    
            with sr.AudioFile(wav.as_posix()) as source:
                audio = r.record(source)  # read the entire audio file
            
            # utiliza a API de transcrição de fala para texto do google
            result = r.recognize_google(audio, language = 'pt-BR' , show_all=True)
            
            transcript.append(result["alternative"][0]["transcript"].lower())
            confidence.append(result["alternative"][0]["confidence"])
            
            print(transcript[-1])
            
            # compara a sentença original e a transcripta
            mf.sentence_compare(original, transcript[-1],words_read, wrong_words, right_words, dicionario)
                
            cont = cont + 1

        
                # elimina as palavras do dicionário que não foram transcriptas corretamente
        for item in wrong_words:
            if wrong_words[item]>0:
                del dicionario[item]
              
        
        show = '\n palavras no arquivo de referencia = %s \n'%len(dicionario)
        
        print(show)
        
        filename = 'dicionario_%s_dB'%(snr)
        
        filename = pasta_ruido_referencia / filename
        with open(filename, 'wb') as f:
            pickle.dump(dicionario, f)



## teste 27/03/2019 - dicionário composto por 135 palavras das 147 originais
## teste 01/04/2019 - dicionário composto por 135 palavras das 147 originais 
## teste 20/07/2019 - dicionário composto por 135 palavras das 147 originais 
## tcc   17/02/2020 - 133, 134, 135 e 136 palavras das 147 originais
#%%
            
            
# lista o conteudo do diretório de resultados
diretorio = list(pasta_audios_resultados.glob('*/'))

resultado_API = []

for i in diretorio: 
    
    resultado_ruido = []

    # tipo do ruído vigente na análise
    ruido = i.parts[-1]
    
    # mostra o tipo de ruido em analise
    print("\n\nRuido de %s em análise\n\n "%ruido)
            
    # lista os algorimtos presentes na pasta do ruído analisado
    algoritmos = list(i.glob('*/'))
    
    for j in algoritmos:
        
        resultado_algoritmo =[]
        
        # algoritmo em analise
        algoritmo = j.parts[-1]
       
        # cria a pasta do algoritmo na pasta do ruido em analise         
        pasta_algoritmo = pasta_API / ruido /  algoritmo 
        pathlib.Path(pasta_algoritmo).mkdir(parents=True, exist_ok=True)
        
        # mostra o algoritmo em analise
        print("\nAlgorimto %s em análise\n"%algoritmo)
        
        # lista as pastas 'Port' no diretório do algoritmo analisado
        ports = list(j.glob('*/'))
        
        resultado_algoritmo = []
        
        for SNR in range(lim_inf_dB, lim_sup_dB+1, step_dB):
        
            print("\n%s dB\n"%SNR)
            
            snr = SNR
                
            if snr <0 and snr > -10:
                # snr string
                snr = '-0%s'%(abs(snr))
            elif snr >= 0 and snr <10:
                snr = '0%s'%(snr)
            else:  
                # snr string
                snr = '%s'%(snr)
                    
            filename = 'dicionario_%s_dB'%snr
            
            filename = pasta_API / ruido / filename
            
            with open(filename, 'rb') as f:
                dicionario = pickle.load(f)
            
            # cria um dicionario vazio para:

            # palavras lidas
            words_read = dicionario.copy()
            
            # palavras erroneamente transcriptas
            wrong_words = dicionario.copy()
            
            # palavras corretamente transcriptas
            right_words = dicionario.copy()
        
            cont = 0

            transcript = []
            confidence = []
            for k in ports:
                
                port = k.parts[-1]
                print(port + '\n')
              
            
                original = file[cont]
                
                filename = '%s_%s_dB.wav' %(algoritmo, snr)
                wav = pasta_audios_resultados / ruido / algoritmo / port / filename
        
                with sr.AudioFile(wav.as_posix()) as source:
                    audio = r.record(source)  # read the entire audio file
                
                # utiliza a API de transcrição de fala para texto do google
                result = r.recognize_google(audio, language = 'pt-BR' , show_all=True)
                
                if len(result) < 1:
                    transcript.append(' ')
                    print('nenhuma palavra foi identificada!')
                else:
                    transcript.append(result["alternative"][0]["transcript"].lower())
   
                
                print(transcript[-1])
                
                # compara a sentença original e a transcripta
                mf.sentence_compare(original, transcript[-1],words_read, wrong_words, right_words, dicionario)
                    
                cont = cont + 1

            total_read = 0.0
            total_right = 0.0
            
            for a,b in zip(words_read, right_words):
                total_read += words_read[a]
                total_right += right_words[b]
             
            print("total de palavras corretas = %d"%(total_right))
            print("total de palavras lidas = %d"%(total_read))
                    
            right_percent = 100 * (total_right / total_read)
            
                        
            print("\n\n HIT RATER %s dB SNR = %0.2f%s \n\n"%(snr, right_percent, "%"))
        
            
            resultado_algoritmo.append(right_percent)
        
        
            del total_read
            del total_right
        
            del right_words
            del wrong_words
            del words_read
            
        resultado_ruido.append([algoritmo, np.transpose(resultado_algoritmo)])
        
    resultado_API.append([ruido, resultado_ruido])
            
filename = pasta_API / 'API'
np.save(filename, resultado_API)


#%%
#    figure generation
    
    # introdução aos gráficos utilizando python  
# http://donald.catalao.ufg.br/up/633/o/introducaoAosGraficos.pdf?1417367842

# tutorial matplotlib
#https://matplotlib.org/tutorials/introductory/usage.html#sphx-glr-tutorials-introductory-usage-py

# cores hexadecimal
#http://erikasarti.com/html/tabela-cores/


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

API_result = resultado_API[ruido][1]

Title =  resultado_API[ruido][0]

for i in range(len(API_result)):
    legend = API_result[i][0]
    y = API_result[i][1][:]
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
plt.legend((line1, line2, line3, line4, line5), (legend1, legend2, legend3, legend4, legend5), loc='lower right')

# grades
#plt.grid()

snr = list(x)
#plt.xticks(snr,[str(i) for i in snr], fontsize=12)
plt.xticks(np.arange(-20, 21, 4 ), fontsize=8)
plt.yticks(np.arange(0, 101, 10), fontsize=8)

# limite dos eixos
plt.axis([lim_inf_dB, lim_sup_dB, 0, 100], option='image')

#plt.show()
