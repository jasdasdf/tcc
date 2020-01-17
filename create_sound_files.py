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
pasta_de_trabalho = pathlib.Path('C:/Users/RAFAEL/Desktop')

# pastas a serem geradas/utilizadas
audios = pasta_de_trabalho / 'audios'
ruido_pasta= audios / 'ruido'


# pasta que contem os arquivos de áudio
ITU_audios = pasta_de_trabalho / 'PORTUGUESE(BRAZILIAN)'
ruidos_Loizou = pasta_de_trabalho / 'Loizou'

# inserir pastas necessárias
import sys
sys.path.insert(0 ,'tcc/scripts/functions/')

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
# obras = obras/max(obras)

librosa.output.write_wav(ruido_pasta / 'obras.wav', obras, fs, norm=False)


##                      GERA RUÍDO DE BALBURDIA

# carrega o arquivo de ruído de balburdia reamostrado 
balburdia, fs = librosa.core.load(ruidos_Loizou/'cafeteria_babble.wav', sr=fs)

# recorta 20 segundos

balburdia = balburdia[:t * fs]

librosa.output.write_wav(ruido_pasta/'balburdia.wav', balburdia, fs, norm=False)



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
ruidos_ajustados = audios / 'ruidos_ajustados'
audios_ajustados = audios / 'audios_ajustados'

pathlib.Path(audios_contaminados).mkdir(parents=True, exist_ok=True)
pathlib.Path(audios_teste).mkdir(parents=True, exist_ok=True)       
pathlib.Path(ruidos_ajustados).mkdir(parents = True, exist_ok = True);
pathlib.Path(audios_ajustados).mkdir(parents = True, exist_ok = True);
                    
# lista os arquivos         
sinal_directory = sorted(list(ITU_audios.glob('*.wav')))
ruido_directory = sorted(list(ruido_pasta.glob('*.wav')))


for ruido in ruido_directory:
    
    # Ruido analizado  (obras, balburdia, wgn)
    noise_name = pathlib.Path(ruido.parts[6][:-4])    
    
    path_test_directory = audios_teste / noise_name 
    path_ruido_ajustado = ruidos_ajustados / noise_name
    path_audio_ajustado = audios_ajustados / noise_name
    path_audios_contaminados = audios_contaminados / noise_name
    
    pathlib.Path(path_test_directory).mkdir(parents = True, exist_ok = True)
    pathlib.Path(path_ruido_ajustado).mkdir(parents = True, exist_ok = True)
    pathlib.Path(path_audio_ajustado).mkdir(parents = True, exist_ok = True)
    pathlib.Path(path_audios_contaminados).mkdir(parents = True, exist_ok = True)
    
    
    print(noise_name)
    
    for audio in sinal_directory:
    
        sinal, fs = librosa.core.load(audio, sr = 16000)
        
        audio_name = audio.parts[5][:-4]
        
        path = path_test_directory / audio_name
        path.mkdir(parents=True, exist_ok=True)
        
        path = path_ruido_ajustado / audio_name
        path.mkdir(parents=True, exist_ok=True)
        
        path = path_audio_ajustado / audio_name
        path.mkdir(parents=True, exist_ok=True)
        
        path = path_audios_contaminados / audio_name       
        path.mkdir(parents = True, exist_ok=True)
        
        print(audio_name)
        
        for snr in range(lim_inf_dB, lim_sup_dB+1, step_dB):
            
            # carrega o arquivo de ruído    
            ruido_data, fs = librosa.core.load(ruido, sr=16000)
             
            # copia para um auxiliar
            ruido_aux = ruido_data.copy()
            
            sinal_aux = sinal.copy()
            
            # filtra o ruido pela planta escolhida
            ruido_filtrado = signal.lfilter(w0, 1, ruido_aux)    
            
            sinal_ruido = mf.addnoise_asl(sinal_aux, ruido_filtrado, 16, 16000, snr)
            
            
            magic_number = 1.0
            while(1):
                # copia para um auxiliar
                ruido_aux = ruido_data.copy()
                
                sinal_aux = sinal.copy()
                
                if magic_number != 1.0:
                    sinal_aux = (sinal_aux/magic_number)*(32767/32768)
                        
                    ruido_aux = (ruido_aux/magic_number)*(32767/32768)
                
                # filtra o ruido pela planta escolhida
                ruido_filtrado = signal.lfilter(w0, 1, ruido_aux)    
                
                sinal_ruido = mf.addnoise_asl(sinal_aux, ruido_filtrado, 16, 16000, snr)
            
                if (max(abs(min(sinal_ruido)), max(sinal_ruido)) >= 1.0):
                    magic_number = magic_number+1
                                        
                else:
                    break
                
            
            print(magic_number)
            
            print("abs = %f " % (max(abs(min(sinal_ruido)), max(sinal_ruido))))
            
            ruido_sinal_ruido = [[a, b] for a,b in zip(ruido_aux, sinal_ruido)]
            
            ruido_sinal_ruido = np.array(ruido_sinal_ruido)
            
            if snr <0 and snr > -10:
                # nome
                file_name = '%s_SNR_-0%s_dB.wav'%(audio_name,abs(snr))
            elif snr >= 0 and snr <10:
                file_name = '%s_SNR_0%s_dB.wav'%(audio_name,snr)
            else:  
                # nome
                file_name = '%s_SNR_%s_dB.wav'%(audio_name,snr)
            
            
            file_arq = path_test_directory / audio_name / file_name
            librosa.output.write_wav(file_arq, ruido_sinal_ruido, 16000, norm=False)
            
            file_arq = path_ruido_ajustado / audio_name / file_name
            librosa.output.write_wav(file_arq, ruido_aux, 16000, norm=False)
            
            file_arq = path_audio_ajustado / audio_name / file_name
            librosa.output.write_wav(file_arq, sinal_aux, 16000, norm=False)            
            
            file_arq = path_audios_contaminados / audio_name / file_name
            librosa.output.write_wav(file_arq, sinal_ruido, 16000, norm=False) 
            
            print(snr)
 
    
    



