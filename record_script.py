# -*- coding: utf-8 -*-
"""
Created on Fri Feb  1 16:14:18 2019

@author: Pertum

# This code has the functionality of record the sound files to the DSP test

"""
import os
import numpy as np
#import numpy       as np        
import sounddevice as sd
import soundfile as sf
import matplotlib.pyplot as plt
from   scipy import signal
import pyautogui
import pathlib

import serial

# dados da porta serial que o dispositivo está conectado
ser = serial.Serial()
ser.baudrate = 115200
ser.port= 'COM5'

# Tamanho do dado recebido via UART
uart_array_length = 10

# Tamanho do buffer utilizado no kit de desenvolvimento
buffer_size = 128

# diretório de trabalho/desenvolviemnto
pasta_de_trabalho = pathlib.Path('C:/Users/RAFAEL/Desktop')


# diretório dos audios
audios = pathlib.Path('audios')
audios_teste = audios / 'audios_teste'

resultados = pasta_de_trabalho / 'resultados'
resultados_balburdia = resultados / 'balburdia'
resultados_obras = resultados / 'obras'
resultados_wgn = resultados / 'wgn'

pathlib.Path(resultados).mkdir(parents=True, exist_ok=True)
pathlib.Path(resultados_balburdia).mkdir(parents=True, exist_ok=True)
pathlib.Path(resultados_obras).mkdir(parents=True, exist_ok=True)
pathlib.Path(resultados_wgn).mkdir(parents=True, exist_ok=True)


# inserir pastas
import sys
sys.path.insert(0 ,'functions/')


# cria a pasta dentro de /resultados
#os.mkdir('resultados')
#os.mkdir('resultados/vss_nlms_zipf')
#os.mkdir('resultados/jo_nlms_2016')
#os.mkdir('resultados/vss_nlms_shin')
#os.mkdir('resultados/npvss_nlms')
#os.mkdir('resultados/nlms')

# quantidade de delay para inserir no final em segundos
sec = 1
fs = 8000
delay = sec*fs
delay = np.zeros(delay)

#%%
# script                        GRAVAR OS AUDIOS 

algoritmo = 'vss_nlms_shin'

# limite inferior do teste em dB
lim_inf_dB = -20

# limite superior do teste em dB
lim_sup_dB = 20

# passo de dB em dB (de 5 em 5 --> jdB = 5)
step_dB = 2


# lista o conteudo do diretório dos audios de teste
directory = list(audios_teste.glob('*/'))

# listas
list_noise = []


# percorre cada pasta dos audios de teste (balburdia, obras e wgn)
for path in directory:
    
    list_Ports  = []
    
    # gera pasta com /'tipo de ruido' / algoritmo
    pasta_algoritmo = resultados / path.parts[2] / algoritmo    
    pathlib.Path(pasta_algoritmo).mkdir(parents = True, exist_ok = True)
    
    # lista as pastas Port_fx presentes no diretório do tipo de ruído
    path_directory = list(path.glob('*/'))    

    
    # print(path_directory)

    # percorre todas as pastas do diretório (Port_f1, Port_f2,...)
    for Port_audio in path_directory:
        
        list_snr    = []

        # lista os arquivos de audio de forma crescente -20 dB para 20 dB
        files = list(Port_audio.glob('*/'))

        files.sort()
        files[0:10]=sorted(files[0:10], reverse=True)
         
        # pasta Port_fx na pasta resultados
        port_audio_path = pasta_algoritmo / Port_audio.parts[3]
       
        show = Port_audio.parts[3]
        print(show)
        
        # cria pasta Port_fx
        pathlib.Path(port_audio_path).mkdir(parents=True, exist_ok=True)
        
        for file in files:
            
            filename = file
            
            snr_file = file.parts[4][12:-4]
            
            # importa o arquivo
            data, fs = sf.read(filename, dtype='float32')
            
            ruido = data[:,0]
            sinal_ruido = data[:,1]
            
            ruido_temp = np.concatenate((ruido,delay), axis=None)
            sinal_ruido_temp = np.concatenate((sinal_ruido,delay), axis=None )
            
            data_temp = [[a,b] for a,b in zip (ruido_temp, sinal_ruido_temp)]
                
            # configura o sounddevice
            sd.default.samplerate = fs    
            sd.default.channels = 2
                
            show = "recording %s"%(snr_file)
            print(show)
            
            # tamanho do buffer a ser coletado da comunicação uart
            uart_length = int (round(sinal_ruido_temp.size / buffer_size * uart_array_length))
        
            # "abre" a comunicação UART
            ser.open()
            
            # envia o comando '1' via UART para resetar as variáveis do filtro
            ser.write('1'.encode())
            
            # reproduz e grava
            rec = sd.playrec(data_temp, fs, 1)
            rec = rec[:,0]
            
            # coleta via uart
            uart_data = ser.read(uart_length)
            
            # fecha a porta de comunicação UART
            ser.close()
            
            # processa os dados recebidos via UART (pega somente os números)
            uart_data = [int(s) for s in uart_data.split() if s.isdigit()]
            
            # calcular o delay entre os arquivos de audio
            r = np.correlate(sinal_ruido_temp, rec, 'full')
            r = r/max(abs(r))
            delay_file = np.argmax(abs(r)==1)
            
            a = np.linspace((-1)*len(sinal_ruido_temp)+1, len(sinal_ruido_temp)-1, len(r))
        
            a = a.tolist()
            ref = a.index(0.0)
            
            delay_file = abs(ref - delay_file)
            show = "corrected delay = %d samples"%(delay_file)
            print(show)
                
            # retira a parte do dalay
            rec = rec [delay_file: len(sinal_ruido) + delay_file]
            
            # retira o delay dos dados coletados via UART
            uart_delay =int (round(delay_file/(fs/buffer_size)))
            uart_data = uart_data[uart_delay : int(round(ruido.size/buffer_size)) + uart_delay ]
                       
            filename_aux = '%s_%s.wav' %(algoritmo, snr_file)
            filename = port_audio_path / filename_aux
            
            # salva o arquivo gravado   
            sf.write(filename, rec, fs)
            
            # apaga a variavel rec (por precaução)
            del rec
            
            show = "File %s saved\nnext!\n"%(snr_file)
            print(show)
            
            list_snr.append(uart_data)
        
        list_Ports.append(list_snr)
   
    list_noise.append(list_Ports)
    
np.save(algoritmo, list_noise)
    
#%%
        #%%

                    ## GRAVAR OS ARQUIVOS DE REFERÊNCIA (CODEC PLACA)


os.chdir("G:/Meu Drive/Adaptive Noise Canceling/Estágio/Python_04")

# diretório dos arquivos de audio 
local = "G:/Meu Drive/Adaptive Noise Canceling/Estágio/Python_04/audios/white_noise/sinal_ruido"

# lista o diretório
directory = os.listdir(local)
directory = directory[:-1]
directory = sorted(directory)

#destiny = "G:/Meu Drive/Adaptive Noise Canceling/Estágio/Python_04/audios/white_noise/sinal_ruido_board"
#os.mkdir(destiny)
#
#for d in directory:
#    name = destiny + "/%s"%d
#    os.mkdir(name)   
        
# limite inferior do teste em dB
lim_inf_dB = -20

# limite superior do teste em dB
lim_sup_dB = 20

# passo de dB em dB (de 5 em 5 --> jdB = 5)
step_dB = 2

for path in directory:
    name_path = "./audios/white_noise/sinal_ruido/%s"%path
    path_directory = os.listdir(name_path)    
    path_directory = path_directory[:-1]
    path_directory.sort()
    path_directory[0:10]=sorted(path_directory[0:10], reverse=True)
    
    dir_name1= destiny + "/%s"%(path)
    os.mkdir(dir_name1)
    
    for file in path_directory:
        print(file)
        # nome do arquivo a ser importado
        filename = "./audios/white_noise/sinal_ruido/%s/%s"%(path, file)
        
        snr_file = file[12:-4]
        
        # importa o arquivo
        data, fs = sf.read(filename, dtype='float32')
                
        
        data_temp = np.concatenate((data,delay), axis=None)
        
        # configura o sounddevice
        sd.default.samplerate = fs    
        sd.default.channels = 1
            
        show = "recording %s"%(snr_file)
        print(show)
        
        # reproduz e grava
        rec = sd.playrec(data_temp, fs, 1)
        rec = rec[:,0]
        
        sd.wait()
       
        # calcular o delay entre os arquivos de audio
        r = np.correlate(data_temp, rec, 'full')
        r = r/max(abs(r))
        delay_file = np.argmax(abs(r)==1)
        
        a = np.linspace((-1)*len(data_temp)+1, len(data_temp)-1, len(r))
    
        a = a.tolist()
        ref = a.index(0.0)
        
        delay_file = abs(ref - delay_file)
        show = "delay corrected = %d samples"%(delay_file)
        print(show)
            
        rec = rec [delay_file: len(data) + delay_file]
                   
        filename = destiny + "/%s/%s"%(path, file)
        
        # salva o arquivo gravado   
        sf.write(filename, rec, fs)
        
        # apaga a variavel rec (por precaução)
        del rec
        
        show = "File %s saved\n\n"%(snr_file)
        print(show)
     
        #%%

                    ##   AUDIOS ITUT P50 NO DOMÍNIO DA PLACA 


# diretório dos arquivos de audio 
local = "G:/Meu Drive/Adaptive Noise Canceling/Estágio/Python_04/ITUT_P50_PT_BR"

# lista o diretório
directory = os.listdir(local)
directory = directory[:-1]
directory = sorted(directory)

destiny = "G:/Meu Drive/Adaptive Noise Canceling/Estágio/Python_04/ITUT_P50_PT_BR_BOARD"
#os.mkdir(destiny)

fs = 8000

# para cada frase da ITU-T P50 e cada pasta do diretório executa
for wav in directory:

    print(wav)
    
    # arquivo de áudio
    file = "%s/%s"%(local, wav)
            
      
    # importa o arquivo
    data, fs = sf.read(file, dtype='float32')
            
    
    data_temp = np.concatenate((data,delay), axis=None)
    
    # configura o sounddevice
    sd.default.samplerate = fs    
    sd.default.channels = 1
        
    show = "recording"
    
    # reproduz e grava
    rec = sd.playrec(data_temp, fs, 1)
    rec = rec[:,0]
    
    sd.wait()
   
    # calcular o delay entre os arquivos de audio
    r = np.correlate(data_temp, rec, 'full')
    r = r/max(abs(r))
    delay_file = np.argmax(abs(r)==1)
    
    a = np.linspace((-1)*len(data_temp)+1, len(data_temp)-1, len(r))

    a = a.tolist()
    ref = a.index(0.0)
    
    delay_file = abs(ref - delay_file)
    show = "delay corrected = %d samples"%(delay_file)
    print(show)
        
    rec = rec [delay_file: len(data) + delay_file]
               
    filename = destiny + "/%s"%(wav)
    
    print(len(data))
    print(len(rec))
    
    # salva o arquivo gravado   
    sf.write(filename, rec, fs)
    
    # apaga a variavel rec (por precaução)
    del rec
    
    show = "File %s saved\n\n"%wav
    print(show)
        
    
#%%
    
                        ## RUIDO 0 dB para avaliação NRR
                        
destiny = "G:/Meu Drive/Adaptive Noise Canceling/Estágio/Python_02/audios/white_noise/ruido_00_dB_NRR"

os.chdir(destiny)                       

file = "Noise2Port_f1_00_dB_filtered.wav"

data, fs = sf.read(file, dtype='float32') 


sec = 1
fs = 8000
delay = sec*fs
delay = np.zeros(delay)

   
data_temp = np.concatenate((data,delay), axis=None)

# configura o sounddevice
sd.default.samplerate = fs    
sd.default.channels = 1
    
show = "recording"

# reproduz e grava
rec = sd.playrec(data_temp, fs, 1)
rec = rec[:,0]

sd.wait()

 # calcular o delay entre os arquivos de audio
r = np.correlate(data_temp, rec, 'full')
r = r/max(abs(r))
delay_file = np.argmax(abs(r)==1)

a = np.linspace((-1)*len(data_temp)+1, len(data_temp)-1, len(r))

a = a.tolist()
ref = a.index(0.0)

delay_file = abs(ref - delay_file)
show = "delay corrected = %d samples"%(delay_file)
print(show)
    
rec = rec [delay_file: len(data) + delay_file]

print(len(data))
print(len(rec))
filename = "Noise2Port_f1_00_dB_filtered_board.wav"

sf.write(filename, rec, fs)