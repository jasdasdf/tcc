# -*- coding: utf-8 -*-
"""
Created on Thu Jan 16 15:31:26 2020


@author: RAFAEL
"""
# includes
import math
import numpy as np        
from scipy import signal
import pathlib 
import librosa 
#%%
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


mf.channel_identification(10, 8000, 128)

#%%


import serial

ser = serial.Serial()
ser.baudrate = 115200
ser.port= 'COM5'

l = []

ser.open()

ser.write('1'.encode())

a=ser.read(5300)

ser.close()

b = [int(s) for s in a.split() if s.isdigit()]

c = np.array(b)
print(c.size)
print(c.mean())

l.append(c)

np.save("test", l)


#%%


from scipy.io import wavfile
from pesq import pesq

rate, deg = wavfile.read("C:/Users/RAFAEL/Desktop/resultados/balburdia/nlms/Port_f1/nlms_00_dB.wav")
rate, ref = wavfile.read("C:/Users/RAFAEL/Desktop/referencia/audios_ajustados/balburdia/Port_f1/audios_ajustados_00_dB.wav")

# print(pesq(rate, ref, deg, 'wb'))
print(pesq(rate, ref, deg, 'nb'))