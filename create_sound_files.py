# -*- coding: utf-8 -*-
"""
Created on Thu Jan 31 08:40:33 2019

@author: Pertum

# This code has the funcionality of create the sound files to the DSP test

"""
# includes
import math
import numpy as np        
from scipy import signal
import pathlib 
import librosa 

# diretório de trabalho/desenvolviemnto
pasta_de_trabalho = pathlib.Path('G:/Meu Drive/Adaptive Noise Canceling/TCC/Desenvolvimento/')

# pastas a serem geradas/utilizadas
audios = pathlib.Path('audios')
ruido_pasta= audios / 'ruido'

# pasta que contem os arquivos de áudio
ITU_audios = pathlib.Path('PORTUGUESE(BRAZILIAN)')
ruidos_Loizou = pathlib.Path('Loizou')

# inserir pastas necessárias
import sys
sys.path.insert(0 ,'functions/')

# importar as função criadas presente na pasta functions

import my_functions as mf

# limite inferior do teste em dB
lim_inf_dB = -20

# limite superior do teste em dB
lim_sup_dB = 20

# passo de dB em dB (de 5 em 5 --> jdB = 5)
step_dB = 2



# cria pasta audios e suas subpastas
pathlib.Path(audios).mkdir(parents=True, exist_ok=True)
pathlib.Path(ruido_pasta).mkdir(parents=True, exist_ok=True)



##                      GERAR RUÍDO BRANCO


# taxa de amostragem utilizada nos testes (8 kHz ou 16 kHz ???)
fs = 16000

# tempo dos arquivos gerados em segundos

t = 16

# tamanho do arquivo a ser gerado, 20 segundos.
N = t * fs

# gera o ruido desejado
varz = 1e-2
ruido = np.sqrt(varz)*np.random.randn(N)


# salva o arquivo de ruído gerado
librosa.output.write_wav(ruido_pasta/'wgn.wav', ruido, fs, norm=False)


##                      GERAR O RUÍDO DE OBRAS EM CONSTRUÇÃO


# carrega o arquivo de ruído de guindaste reamostrado na frequência fs, logo, sr=fs.
crane_noise, fs = librosa.core.load(ruidos_Loizou/'Construction_Crane_Moving.wav', sr=fs)

# carrega o arquivo de uma britadeira reamostrado na frequência fs.
jackhammer_noise, fs = librosa.core.load(ruidos_Loizou/'Construction_Jackhammer2.wav', sr=fs);    

# recorta os 15 ultimos segundos do arquivo (pois possui mais ruído!!!)        
crane_noise = crane_noise[-t * fs:]

# 15 segundos do ruído da britadeira
jackhammer_noise = jackhammer_noise[:t * fs]

# soma os dois sinais de ruído, para gerar um ruído composto
obras = crane_noise + jackhammer_noise

# normaliza o sinal para não haver clip
obras = obras/max(obras)

librosa.output.write_wav(ruido_pasta / 'obras.wav', obras, fs, norm=False)


##                      GERA RUÍDO DE BALBURDIA

# carrega o arquivo de ruído de balburdia reamostrado 
balburdia, fs = librosa.core.load(ruidos_Loizou/'cafeteria_babble.wav', sr=fs)

# recorta 20 segundos

balburdia = balburdia[:t * fs]

librosa.output.write_wav(ruido_pasta/'balburdia.wav', balburdia, fs, norm=False)

###############################################################################



# #                     ANALISA O 'NÚMERO MÁGICO' PARA GERAR
# #                          OS ARQUIVOS DE BAIXA SNR

# # analisa a composição com a menor SNR para determinar o fator de divisão 
# # adequado para todos os arquivos contaminados


# import soundfile as sf
# import sounddevice as sd

# #  importa a planta
# [w0, ERL, K ]= mf.echopath(4,0,6,128)

# #  garante que a planta possui norma unitária
# w0 = w0 / np.sqrt(np.dot(w0, w0))

# # lista do diretório dos audios
# speech_directory = sorted(list(ITU_audios.glob('*.wav')))
# noise_directory = sorted(list(ruido_pasta.glob('*.wav')))

# magic_number = 1;

# l=0
# for i in speech_directory:
#         for j in noise_directory:
            
#             cleanfile, fs = librosa.core.load(i, sr=16000)
#             noisefile, fs = sf.read(j)
            
            
#             cleanfile = cleanfile/7
#             noisefile = noisefile/7
            
#             noisefile = signal.lfilter(w0, 1, noisefile.copy()) 
            
            
#             del l
#             l = mf.addnoise_asl(cleanfile, noisefile, 16, 16000, -20) 
            
#             max_p = abs(max(l)) 
#             max_n = abs(min(l)) 
            
#             if max_p> magic_number :
#                 magic_number = max_p
#                 print(magic_number)
#                 print(i,j)
#             elif max_n > magic_number:
#                 magic_number = max_n
#                 print(magic_number)
#                 print(i,j)



# magic_number = int(np.ceil(magic_number))


###############################################################################



import soundfile as sf
import sounddevice as sd

#                   AJUSTA OS AUDIOS E OS RUIDOS


magic_number = 1

audios_ajustados = audios / 'audios_ajustados'
ruidos_ajustados = audios / 'ruidos_ajustados'

pathlib.Path(audios_ajustados).mkdir(parents=True, exist_ok=True)                    
pathlib.Path(ruidos_ajustados).mkdir(parents=True, exist_ok=True)

speech_directory = sorted(list(ITU_audios.glob('*.wav')))
noise_directory = sorted(list(ruido_pasta.glob('*.wav')))

for audio in speech_directory:
     
    file, fs = librosa.core.load(audio, 16000)
    
    file = file/magic_number
    
    librosa.output.write_wav(audios_ajustados / audio.parts[1], file, 16000)
    
for ruido in noise_directory:
    
    file, fs = sf.read(ruido, dtype='float32')
    
    file = file/magic_number
    
    librosa.output.write_wav(ruidos_ajustados / ruido.parts[2], file, 16000)
    
  

##      GERA OS ARQUIVOS 
                      # AUDIOS DE TESTE
                      # AUDIOS CONTAMINADOS
                      # AUDIOS AJUSTADOS
                      # RUIDOS AJUSTADOS

#  importa a planta
[w0, ERL, K ]= mf.echopath(4,0,6,128)

#  garante que a planta possui norma unitária
w0 = w0 / np.sqrt(np.dot(w0, w0))

# gera pastas
audios_contaminados = audios / 'audios_contaminados'
audios_teste = audios / 'audios_teste'

pathlib.Path(audios_contaminados).mkdir(parents=True, exist_ok=True)
pathlib.Path(audios_teste).mkdir(parents=True, exist_ok=True)       
                     
# lista os arquivos         
sinal_directory = sorted(list(audios_ajustados.glob('*.wav')))
ruido_directory = sorted(list(ruidos_ajustados.glob('*.wav')))


for ruido in ruido_directory:
    
    # Gera uma pasta para o tipo de ruído na pasta de audios_teste
    path_name = pathlib.Path(ruido.parts[2][:-4])    
    path_test_directory = audios_teste / path_name    
    path_test_directory.mkdir(parents = True, exist_ok = True)
    
    # Gera uma pasta para o tipo de ruído na pasta auidos_contaminados
    path_contaminados_directory = audios_contaminados / path_name    
    path_contaminados_directory.mkdir(parents=True, exist_ok=True)  
    
    # carrega o arquivo de ruído ajustado    
    ruido_data, fs = sf.read(ruido, dtype='float32')
     
    # copia para um auxiliar
    ruido_aux = ruido_data.copy()
    
    # filtra o ruido pela planta escolhida
    ruido_filtrado = signal.lfilter(w0, 1, ruido_aux)
    
    print(path_name)
    
    for audio in sinal_directory:
    
        sinal, fs = sf.read(audio, dtype='float32')
        
        name = audio.parts[2][:-4]
        
        path = path_contaminados_directory / name        
        path.mkdir(parents = True, exist_ok=True)
        
        
        path = path_test_directory / name
        path.mkdir(parents=True, exist_ok=True)
        
        print(name)
        
        for snr in range(lim_inf_dB, lim_sup_dB+1, step_dB):

            sinal_ruido = mf.addnoise_asl(sinal, ruido_filtrado, 16, 16000, snr)
            
            ruido_sinal_ruido = [[a, b] for a,b in zip(ruido_data, sinal_ruido)]
            
            if snr <0 and snr > -10:
                # nome
                file_name = '%s_SNR_-0%s_dB.wav'%(name,abs(snr))
            elif snr >= 0 and snr <10:
                file_name = '%s_SNR_0%s_dB.wav'%(name,snr)
            else:  
                # nome
                file_name = '%s_SNR_%s_dB.wav'%(name,snr)
            
            file_arq = path_contaminados_directory / name  / file_name
            sf.write(file_arq, sinal_ruido, 16000)
            
            file_arq = path / file_name
            sf.write(file_arq, ruido_sinal_ruido, 16000)
            
            print(snr)
 
    
    



