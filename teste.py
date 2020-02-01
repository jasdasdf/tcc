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




