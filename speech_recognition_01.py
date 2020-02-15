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

# diretório de trabalho
os.chdir("G:/Meu Drive/Adaptive Noise Canceling/Estágio/Python_04")

# adiciona a pasta functions aos diretórios de pesquisa
sys.path.insert(0 ,'./functions')

# importa o módulo my_functions (contido na pasta functions)
import my_functions as mf



# limite inferior do teste em dB
lim_inf_dB = -20

# limite superior do teste em dB
lim_sup_dB = 20

# passo de dB em dB (de 5 em 5 --> jdB = 5)
step_dB = 2


# cria parametro de reconhecimento de fala
r = sr.Recognizer()

# lista os audios da itu-t p50
directory = os.listdir('./ITUT_P50_PT_BR_BOARD/')

# apaga o dado "desktop.init"
directory = directory[:-1]

# organiza a lista dos audios
directory = sorted(directory)

# cria arrays para os resultdos, transcrições e confiança
results = []
transcript=[]
confidence=[]

# dicionário
dictionary=[]

# arquivo que cotem as frases originais da itu-t p50
arq = "./scripts/ITUT_P50.txt"

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
        if ((len(word) > 3) and (word not in dictionary)):
            dictionary.append(word)

# dicionário com as palavras 
dictionary =dict([(word, 0) for word in dictionary])



        # TRANSCRIÇÃO DA REFERÊNCIA (DICIONÁRIO)

        
# cria um dicionario vazio para:

# palavras lidas
words_read = dictionary.copy()

# palavras erroneamente transcriptas
wrong_words = dictionary.copy()

# palavras corretamente transcriptas
right_words = dictionary.copy()

# coloca a pasta dos autios itu-t p50 como diretório de trabalho
os.chdir("./ITUT_P50_PT_BR_BOARD/")

# executa a transcrição de audio para texto utilizando a 
# Google API speech-to-text
for line,wav in zip(file[:-1], directory):
    original = line.lower()
    
    with sr.AudioFile(wav) as source:
        audio = r.record(source)  # read the entire audio file
    
    # utiliza a API de transcrição de fala para texto do google
    result = r.recognize_google(audio, language = 'pt-BR' , show_all=True)
    
    transcript.append(result["alternative"][0]["transcript"].lower())
    confidence.append(result["alternative"][0]["confidence"])
    
    print(transcript[-1])
    
    # compara a sentença original e a transcripta
    mf.sentence_compare(original, transcript[-1],words_read, wrong_words, right_words, dictionary)
        
reference = [words_read.copy(), wrong_words.copy(), right_words.copy()] 

#%%

# elimina as palavras do dicionário que não foram transcriptas corretamente
for item in wrong_words:
    if wrong_words[item]>0:
        del dictionary[item]
        del reference[0][item]
        del reference[1][item]
        del reference[2][item]


## teste 27/03/2019 - dicionário composto por 135 palavras das 147 originais
## teste 01/04/2019 - dicionário composto por 135 palavras das 147 originais 
## teste 20/07/2019 - dicionário composto por 135 palavras das 147 originais 
#%%
        
        # AVALIAÇÃO PARA O PIOR CASO  (WORST_CASE)
      
# diretório dos arquivos de audio 
local = "G:/Meu Drive/Adaptive Noise Canceling/Estágio/Python_04/audios/white_noise/sinal_ruido_board"
os.chdir(local)

# lista o diretório
directory = os.listdir("./")
directory = directory[:-1]
directory = sorted(directory)

# cria parametro de reconhecimento de fala
r = sr.Recognizer()

worst_case = []
worst_error = []

for snr in range(lim_inf_dB, lim_sup_dB+1, step_dB):
    
    if snr <0 and snr > -10:
        # snr string
        snr = '-0%s'%(abs(snr))
    elif snr >= 0 and snr <10:
        snr = '0%s'%(snr)
    else:  
        # snr string
        snr = '%s'%(snr)
            
        
    print("\n\n AVALIANDO %s dB SNR\n\n"%snr)
    # cria um dicionario vazio para:

    # palavras lidas
    words_read = dictionary.copy()
    
    # palavras erroneamente transcriptas
    wrong_words = dictionary.copy()
    
    # palavras corretamente transcriptas
    right_words = dictionary.copy()
    
    # para cada frase da ITU-T P50 e cada pasta do diretório executa
    for line, path in zip(file, directory):        
        
        # frase original do arquivo a ser lido
        original = line.lower()
    
        # pasta que que contem os arquivos de áudio
        sub_local = "%s"%path
        
        wav = "./" + path + "/" + path + "_SNR_" + snr + "_dB.wav"
        
        # abre o arquivo de áudio como atributo para o speech recognition
        with sr.AudioFile(wav) as source:
            audio = r.record(source)  # read the entire audio file
        
        # resultado da transcrição de fala para texto
        result = r.recognize_google(audio, language = 'pt-BR' , show_all=True)
        
        # se resultado = [] ( o speech recognition não conseguiu transcrever)
        if result == []:
            # atribui ao resultado a seguinte frase:
            result = "nothing recognized"
            transcript.append(result)
            # com grau de confiança confiânça igual a 0.0 
            confidence.append(0.0)
        # senão (o speech recognition conseguiu transcrever)
        else:
            # armazena a transcrição no array transcript
            transcript.append(result["alternative"][0]["transcript"].lower())
            #confidence.append(result["alternative"][0]["confidence"])
        
        # mostra a transcrição
        print(transcript[-1])
        
        # compara a transcrição com a frase original da itu-t p50
        mf.sentence_compare(original, transcript[-1],words_read, wrong_words, right_words, dictionary)
        
        
                # ORDENA A TAXA DE ERRO DO PIOR CASO (WORST_ERROR) 
    
    worst_case.append([words_read.copy(), wrong_words.copy(), right_words.copy()])
   
    total_read = 0.0
    total_right = 0.0
    
    for a,b in zip(words_read, right_words):
        total_read += words_read[a]
        total_right += right_words[b]
     
    print("total de palavras corretas = %d"%(total_right))
    print("total de palavras lidas = %d"%(total_read))
            
    right_percent = 100 * (total_right / total_read)
    
    
    
    print("\n\n HIT RATER %s dB SNR = %0.2f%s \n\n"%(snr, right_percent, "%"))

    
    worst_error.append(right_percent)


    del total_read
    del total_right

    del right_words
    del wrong_words
    del words_read

 #%%
    
                # GERA FIGURA DO PIOR CASO (ERROR REFERENCE)
    
# módulos importados
import matplotlib.ticker as ticker
import matplotlib.pylab as plt
from scipy.optimize import curve_fit
import math
from sklearn.metrics import r2_score


# função exponencial do ajuste de curva
def func(x,a,b,c):    
    y=[]
    for data in x:
        y.append(a*math.exp(-b*data) + c)
    return y


# tamanho da figura
#plt.rcParams['figure.figsize'] = (9,6)

# tipo de fonte times new roman    
plt.rcParams["font.family"] = "Times New Roman"

# tamanho da fonte da legenda
plt.rcParams['legend.fontsize'] = 12
    
 
 
x = range(lim_inf_dB,lim_sup_dB + 1, step_dB)
    
y = worst_error.copy()  


# ajuste de curva para os dados do pior caso
popt, pcov = curve_fit(func, x, y)

# dados do ajuste de curva para o pior caso (referencia)
y_line = func(x, popt[0], popt[1], popt[2])

r =  r2_score(y, y_line)



label_error = "Referência - REF"
label_error_ex = "Log(REF) - R$^{2}$ = %0.3f"%(r)


fig, ax = plt.subplots(figsize=(9,6))


plt.scatter(x, y, label=label_error, color='b')
plt.plot(x, y_line, label=label_error_ex, color='b')


plt.title("REFERÊNCIA", loc='center')  

ax.set_xlabel('SNR (dB)', fontsize=12)
ax.set_ylabel('Taxa de acerto (%)',fontsize=12)

snr = list(x)
plt.xticks(snr,[str(i) for i in snr])

ax.set_xlim([min(x), max(x)])
#ax.set_ylim(0, 100)

plt.legend()
#plt.grid()
  
#%%

            # TRANSCRIÇÃO DOS AUDIOS COLETADOS

    # observação: Trocar o nome do diretório e das variáveis finais.

# local das pastas dos audios coletados
local = "G:/Meu Drive/Adaptive Noise Canceling/Estágio/Python_04/audios/white_noise/resultados/nlms"
os.chdir(local)

# listagem das pastas
directory = os.listdir("./")
directory = directory[:-1]
directory = sorted(directory)

# cria parametro de reconhecimento de fala
r = sr.Recognizer()

nlms_case = []
nlms_error = []

for snr in range(lim_inf_dB, lim_sup_dB+1, step_dB):
    
    if snr <0 and snr > -10:
        # snr string
        snr = '-0%s'%(abs(snr))
    elif snr >= 0 and snr <10:
        snr = '0%s'%(snr)
    else:  
        # snr string
        snr = '%s'%(snr)
            
        
    print("\n\n AVALIANDO %s dB SNR\n\n"%snr)
    # cria um dicionario vazio para:

    # palavras lidas
    words_read = dictionary.copy()
    
    # palavras erroneamente transcriptas
    wrong_words = dictionary.copy()
    
    # palavras corretamente transcriptas
    right_words = dictionary.copy()
    
    # para cada frase da ITU-T P50 e cada pasta do diretório executa
    for line, path in zip(file, directory):        
        
        # frase original do arquivo a ser lido
        original = line.lower()
    
        # pasta que que contem os arquivos de áudio
        sub_local = "%s"%path
        
        wav = "./" + path + "/" + "nlms_" + snr + "_dB.wav"
        
        # abre o arquivo de áudio como atributo para o speech recognition
        with sr.AudioFile(wav) as source:
            audio = r.record(source)  # read the entire audio file
        
        # resultado da transcrição de fala para texto
        result = r.recognize_google(audio, language = 'pt-BR' , show_all=True)
        
        # se resultado = [] ( o speech recognition não conseguiu transcrever)
        if result == []:
            # atribui ao resultado a seguinte frase:
            result = "nothing recognized"
            transcript.append(result)
            # com grau de confiança confiânça igual a 0.0 
            confidence.append(0.0)
        # senão (o speech recognition conseguiu transcrever)
        else:
            # armazena a transcrição no array transcript
            transcript.append(result["alternative"][0]["transcript"].lower())
            #confidence.append(result["alternative"][0]["confidence"])
        
        # mostra a transcrição
        print(transcript[-1])
        
        # compara a transcrição com a frase original da itu-t p50
        mf.sentence_compare(original, transcript[-1],words_read, wrong_words, right_words, dictionary)
        
        
                # ORDENA A TAXA DE ERRO DO PIOR CASO (WORST_ERROR) 
    
    nlms_case.append([words_read.copy(), wrong_words.copy(), right_words.copy()])
   
    total_read = 0.0
    total_right = 0.0
    
    for a,b in zip(words_read, right_words):
        total_read += words_read[a]
        total_right += right_words[b]
     
    print(total_right)
    print(total_read)
            
    error_percent = 100 * (total_right / total_read)
    
    
    print("\n\n HIT RATE %s dB SNR = %0.2f%s \n\n"%(snr, error_percent, "%"))

    
    nlms_error.append(error_percent)


    del total_read
    del total_right

    del right_words
    del wrong_words
    del words_read


 #%%
    
                # GERA A FIGURA DOS RESULTADOS
    
# módulos importados
from sklearn.metrics import r2_score
from matplotlib import font_manager
from matplotlib.ticker import (MultipleLocator, FormatStrFormatter, AutoMinorLocator)
import matplotlib.pylab as plt
import matplotlib

#Corrige o problema com as fontes em "negrito"
matplotlib.font_manager._rebuild()

# tipo de fonte times new roman    
plt.rcParams["font.family"] = "Times New Roman"

#Eixo X
x = range(lim_inf_dB,lim_sup_dB + 1, step_dB)
    
# Conteúdo repassado para variáveis (por conveniência)
y1 = nlms_error.copy()
y2 = vss_nlms_shin_error.copy()
y3 = npvss_nlms_error.copy()
y4 = vss_nlms_zipf_error.copy()
y5 = worst_error.copy() 

#Gera a Figura do tamanho desejado
fig, ax = plt.subplots(figsize=(3.5,2.62))

plt.plot(x, y1, '-', marker = None,  color=[0,0,0], linewidth=1.1, label="NLMS")
plt.plot(x, y2,'-.', dashes=[6,3,1,3],  color=[0,0,0], linewidth=1.1,  label="VSS-NLMS de Shin")
plt.plot(x, y3, '--', dashes = [6,4],  color=[0,0,0], linewidth=1.1, label="VSS-NLMS de Benesty")
plt.plot(x, y4,  '--', dashes = [1,2],  color=[0,0,0], linewidth=1.1, label="VSS-NLMS de Zipf")
plt.plot(x, y5, '-', marker ='^', markersize=3,  color=[0,0,0], linewidth=1.1, label="ANC desligado")


#Fonte dos Ticks
ticks_font = font_manager.FontProperties(family='Arial', style='normal', weight='normal', stretch='normal', size=8)

#Fonte dos Labes
labels_font = font_manager.FontProperties(family='Times New Roman', style='normal', weight='normal', stretch='normal', size=10)

#Propriedades dos números/ticks eixo X
for label in ax.get_xticklabels():
    label.set_fontproperties(ticks_font)

#Propriedades dos números/ticks eixo Y    
for label in ax.get_yticklabels():
    label.set_fontproperties(ticks_font)
    

#Label do eixo X
ax.set_xlabel('SNR (dB)',fontproperties = labels_font)

#Label do eixo Y  
ax.set_ylabel('Taxa de Acerto (%)', fontproperties = labels_font)

#limites do eixo X
ax.set_xlim([lim_inf_dB, lim_sup_dB])
ax.set_ylim(0, 101)

#Legenda
plt.legend(handlelength=3.5, fontsize=7.7, labelspacing=0.25, edgecolor=[0,0,0], loc='lower right')


#plt.grid()
#ajuste da localização dos ticks do eixo X
ax.xaxis.set_major_locator(MultipleLocator(4))
ax.xaxis.set_minor_locator(MultipleLocator(2))

#ajuste da localização dos ticks do eixo y
ax.yaxis.set_major_locator(MultipleLocator(20))
ax.yaxis.set_minor_locator(MultipleLocator(10))

# ajuste automático das bordas brancas...
#plt.tight_layout()
plt.gcf().subplots_adjust(left=0.137, right=0.98, bottom=0.15, top=0.98, wspace=0, hspace=0)

# salva como figura
os.chdir("G:/Meu Drive/Adaptive Noise Canceling/Estágio/Python_04/audios/white_noise")
plt.savefig("google_result.svg")
plt.savefig("google_result.jpg")
#plt.savefig("google_result.pdf")

        
        