# -*- coding: utf-8 -*-
"""
Created on Thu Jan 31 10:41:08 2019

@author: Pertum
"""
"""
--------------------------------------------------------------------------
 Referências:
    [1] ITU-T. “Series G: Transmission systems and media, digital systems and
                       networks: Digital network echo cancellers”, 2009.
--------------------------------------------------------------------------
"""
"""    
 Dados de entrada:
 Numero do modelo - modelo [1,2,...,8]
 Ativa figura - figura [0 off, 1 on]
 Define ERL mas, se ERL=0 utiliza o padrão da tabela - ERL
 """ 
 
import numpy as np 

# import only system from os 
from os import system, name 
import struct
# import matplotlib.pyplot as plt

# inserir pastas
import sys
sys.path.insert(0 ,'functions')
import adaptive as adpt


def echopath(modelo,fig,ERL,Mord):

    D=[[1, 1.39*10**-5, 6],[ 2, 1.44*10**-5, 6.55],[ 3, 1.52*10**-5, 6],[ 4, 1.77*10**-5, 6],[5, 9.33*10**-6, 6],[ 6, 1.51*10**-5, 6],[ 7, 2.33*10**-5, 11.06],[ 8, 1.33*10**-5, 9.27]]
        
     # modelo 4
    M = [160,312,-241,-415,897,908,-1326,-1499,2405,3347,-3624,-7733,4041,14484,
        -1477,-21739,-4470,25356,11458,-19696,-11800,5766,789,6633,14624,-6975,
        -17156,-187,149,1515,14907,4345,-7128,-2757,-10185,-7083,6850,3944,6969,
        8694,-4068,-3852,-5793,-9371,453,1060,3965,9463,2393,2784,-892,-7366,
        -3376,-5847,-2399,3011,1537,6623,4205,1602,1592,-4752,-3646,-5207,-5577,
        -501,-1174,4041,5647,4628,7252,2123,2654,-881,-4113,-3244,-7289,-3830,
        -4600,-2508,431,-144,4184,2372,4617,3576,2382,2839,-404,539,-1803,-1401,
        -1705,-2269,-783,-1608,-220,-306,257,615,225,561,8,344,127,-57,182,41,
        203,-111,95,-79,30,84,-13,-68,-241,-68,-24,19,-57,-24,30,-68,84,-155,
        -68,19]
    
    
    if ERL ==0:
        ERL = D[modelo-1][2]
    elif ERL< D[modelo-1][2]:
        print('A ERL especificada deve ser maior do que: %0.02f', D[modelo-1][2]);
        ERL=0;
        K=0;
        return
    
    K = D[modelo-1][1]
    
    factor = ((10**(-ERL/20))*K)
    
    h = np.asarray(M)
    h = factor* h

    return h, ERL, K

def float2int16(data):
    data = (data*32767).astype(np.int16)
    return data

def NRR(erro, sinal, ruido_filtrado, Fs):
    '''
         Performance assessment - Noise reduction ratio
     PERFORMANCE ANALYSIS OF ADAPTIVE NOISE CANCELLER EMPLOYING NLMS ALGORITHM
     Farhana Afroz, Asadul Huq, F. Ahmed, and Kumbesan Sandrasegaran
     International Journal of Wireless & Mobile Networks (IJWMN) Vol. 7, No. 2, April 2015 
     http://airccse.org/journal/jwmn/7215ijwmn04.pdf -> Eq. (7)
    
     ACREDITO EXISTIR UMA REFERÊNCIA MELHOR PARA A DEFINIÇÃO MATEMÁTICA DA
     NRR...MAS NÃO ACHEI AINDA!!!!!
     
     '''
    # auxiliar, erro - sinal ponto a ponto
    aux = [erro-sinal for (erro, sinal) in zip(erro, sinal)]
    
    # Determina o valor global de redução do ruído:
    NRR_global = 10*np.log10(np.var(aux) / np.var(ruido_filtrado))
    
    # auxiliar, erro - sinal ponto a ponto no final dos arquivos
    aux = [erro-sinal for (erro,sinal) in zip(erro[-512:], sinal[-512:])]
    
    # Determina o valor final de redução do ruído (após convergência):
    NRR_final = 10*np.log10(np.var(aux) / np.var(ruido_filtrado[-512:]))
    
    '''
        %--------------------------------------------------------------------------
    % O desempenho dos algortimos também pode ser avaliado em termos de
    % redução de ruído. Para tal, a métrica é avaliada em considerando
    % janelas/segmentos; portanto, ao invés de obter uma média global,
    % tem-se uma média temporal (horizontal).
    %--------------------------------------------------------------------------
    '''
    # tamanho dos arquivos
    N = len(sinal)
    
    # Parâmetros de janela
    Td = 20e-3      # comprimento da janela
    L = int(Fs*Td)       # comprimento da janela de amostra
    
    # verifica se o segmento de é inteiro; caso congtrário preenche com zeros
    if N%L != 0:
        npad = np.abs(N%L - L)
    else:
        npad = 0
        
    numseg = (N+npad)//L
    
    nseg = list(range(0,N,L))
    
    # insere zeros no final dos sinais avaliados
    
    erro = erro.tolist()
    erro = np.append(erro, np.zeros(npad))
    
    ruido_filtrado = ruido_filtrado.tolist()
    ruido_filtrado.extend(np.zeros(npad))
    
#    sinal = sinal.tolist()
    sinal = np.append(sinal, np.zeros(npad))
    
    # calcula a atenuação no segmento:
    NRR = []
    for k in range(0,numseg*L, L):
        #janela um segmento:
        erro_temp = erro[k:k+L-1]
        sinal_temp = sinal[k:k+L-1]
        ruido_filtrado_temp = ruido_filtrado[k:k+L]
        
        # auxiliar, erro_temp[L]-sinal[L]
        aux = [erro-sinal for (erro, sinal) in zip(erro_temp, sinal_temp)] 
        
        # valor da NRR
        valor = np.mean(np.power(aux,2)) / np.mean(np.power(ruido_filtrado_temp,2))
        
        # armazena o valor da NRR
        NRR.append(valor)
    
    NRR_dB = [10*np.log10(data) for data in NRR]
    
    return NRR_global, NRR_final, NRR, NRR_dB, nseg
    

def convert_coefs(fname):
    data=[]
        
    # contador de linhas
    cont = 0
    with open(fname, 'r') as file:
        for line in file:
            if cont != 0:
                # elimina o ':' 
                line = line[1:]
                
                # pega o numero de valores de 32-bit na linha
                N = int(line[:2], 16)/4      
                N = int(N)
                
                # descarta os próximos o 8 primeiros elementos
                line = line[8:]            
            
                if N>0:
                    for i in range(N):
                        # linha temporária (com valor hexa )
                        x = line[8*i:8*i+8]
                        
                        reordered = x[6] + x[7] + x[4] + x[5] + x[2] + x[3] + x[0] + x[1]
                        
                        #https://stackoverflow.com/questions/1592158/convert-hex-to-float                        
                        value = struct.unpack('!f', bytes.fromhex(reordered))[0]
                        del reordered
                        data.append(value)
            cont+=1       
        data = data[::-1]
    file.close()
    return data
    
    
def rebuild_filter_identification(time, fs, M):
    
    # gera um ruido de tamanho M
    N = time*fs
    varz = 1e-2
    prbs = np.sqrt(varz)*np.random.randn(N)
    
    rec = sd.playrec(prbs, fs, 1)
    rec = rec[:,0]
    
    sd.wait()
    
    # calcular o delay entre os arquivos de audio
    r = np.correlate(prbs, rec, 'full')
    r = r/max(abs(r))
    delay_file = np.argmax(abs(r)==1)
    
    a = np.linspace((-1)*N+1, N-1, len(r))
        
    a = a.tolist()
    ref = a.index(0)
    
    delay_file = abs(ref - delay_file)
    show = "delay corrigido = %d amostras"%(delay_file)
    print(show)
    
    prbs = prbs[:len(prbs)-delay_file]
    
    rec = rec [delay_file : len(prbs) + delay_file]

    # NLMS
    N = len(prbs)
    MU=0.05
    ep = 1.0
    hist = 0
    
#    # NPVSS-NLMS
#    lbda = 0.999

    x = rec
    d = prbs   
    
    y,w, whist, e, J = adpt.NLMS_Algorithm(N, M, MU, ep, x, d, hist)
        
    # Pad filter with zeros.
    h_padded = np.zeros(10*M)
    h_padded[0 : M] = w
     
    # Compute frequency response; only keep first half.
    H = np.abs(np.fft.fft(h_padded))[0 : 10*M// 2 + 1]
     
    # Plot frequency response (in dB).
    plt.figure()
    plt.plot(np.linspace(0, 0.5, len(H)), 20*np.log10(H))
    plt.xlabel('Normalized frequency')
    plt.ylabel('Gain [dB]')
    plt.ylim([-100, 10])
    plt.grid()
    plt.show()

    
    return w

def channel_identification(time, fs, M):
    
    # gera um ruido de tamanho M
    N = time*fs
    varz = 1e-2
    prbs = np.sqrt(varz)*np.random.randn(N)
    
    rec = sd.playrec(prbs, fs, 1)
    rec = rec[:,0]
    
    sd.wait()
    
    # calcular o delay entre os arquivos de audio
    r = np.correlate(prbs, rec, 'full')
    r = r/max(abs(r))
    delay_file = np.argmax(abs(r)==1)
    
    a = np.linspace((-1)*N+1, N-1, len(r))
        
    a = a.tolist()
    ref = a.index(0)
    
    delay_file = abs(ref - delay_file)
    show = "delay corrigido = %d amostras"%(delay_file)
    print(show)
    
    prbs = prbs[:len(prbs)-delay_file]
    
    rec = rec [delay_file : len(prbs) + delay_file]

    # NLMS
    N = len(prbs)
    MU=0.05
    ep = 1.0
    hist = 0
    
#    # NPVSS-NLMS
#    L = M
#    k_np = 2
#    lbda_np = 1.0 - 1.0/(k_np*L)
    
    
    x = prbs
    d = rec   
    
    y,w, whist, e, J = adpt.NLMS_Algorithm(N, M, MU, ep, x, d, hist)
#    y,w, whist, e, J = adpt.NPVSS_NLMS_Algorithm(N,M,lbda_np,ep,x,d,hist)
        
    # Pad filter with zeros.
    h_padded = np.zeros(10*M)
    h_padded[0 : M] = w
     
    # Compute frequency response; only keep first half.
    H = np.abs(np.fft.fft(h_padded))[0 : 10*M// 2 + 1]
     
    # Plot frequency response (in dB).
    plt.figure()
    plt.plot(np.linspace(0, 0.5, len(H)), 20*np.log10(H))
    plt.xlabel('Normalized frequency')
    plt.ylabel('Gain [dB]')
    plt.ylim([-100, 10])
    plt.grid()
    plt.show()

    
    return w
    
def sentence_compare(original, compared, words_read, wrong_words, right_words, dictionary):
    
    original = original.lower().split()
    compared = compared.lower().split()
    
    
    for word in original:
        if len(word) > 3 and word in dictionary:
            words_read[word] = words_read[word] + 1
            if word in compared:
                right_words[word] = right_words[word] + 1
            else:
                wrong_words[word] = wrong_words[word] + 1
        

def AR2(a1, a2, v):
    
    x=[]
    
    x.append(0 + v[0])
    x.append(0 + v[1])
    
    for i, data in zip(range(2,len(v)), v):
        x.append( a1*x[i-1] + a2*x[i-2] + v[i])
        
        
    return x

def addnoise_asl(cleanfile, noisefile, nbits, fs, snr):
    
    import numpy.random as rns
    
    # NÃO FOI IMPLEMENATADO O FILTRO IRS!!!
    
    outfile = np.zeros(cleanfile.size)
    
    Px, asl, c0 = asl_P56(cleanfile, fs, nbits)    
    x = cleanfile.copy()
    noise = noisefile.copy()
    
    try:
        noise.size <= x.size
    except ValueError:
        print('the noise length has to be greater than speech length!')
    
    rand_start_limit = noise.size - x.size
    
    rand_start = int(np.round((rand_start_limit - 1) * rns.rand() + 1))
    
    noise_segment = noise[rand_start : (rand_start + x.size )]
    
    Pn = np.sum(noise_segment*noise_segment) / x.size
    
    sf = np.sqrt(Px/Pn/(10**(snr/10)))
    
    noise_segment = noise_segment * sf
    
    noisy = x + noise_segment
    
    outfile = noisy.copy()
  
    return outfile



def bin_interp(upcount, lwcount, upthr, lwthr, Margin, tol):
    
    asl_ms_log = 0
    cc = 0
    
    if tol<0:
        tol = -tol
        
    interno = 1
    if (abs(upcount - upthr - Margin) < tol):
        asl_ms_log = upcount
        cc = upthr
        return asl_ms_log, cc
    if (abs(lwcount - lwthr - Margin) < tol):
        asl_ms_log = lwcount
        cc = lwthr
        return asl_ms_log, cc
  
    
    midcount = (upcount + lwcount) / 2.0
    midthr = (upthr + lwthr) / 2.0
    
    while(1):
        diff = midcount - midthr - Margin
        if abs(diff) <= tol:
            break
        
        interno = interno + 1
        
        if interno>20:
            tol = tol*1.1
        
        if diff > tol:
            midcount = (upcount + midcount) / 2.0
            midthr = (upthr + midthr) / 2.0
        elif diff < -tol:
            midcount = (midcount + lwcount) / 2.0
            
            midthr = (midthr + lwthr) / 2.0
            
    asl_ms_log = midcount
    cc = midthr    
        
    return asl_ms_log, cc


def asl_P56(x, fs, nbits):
    
    asl_ms = 0
    asl = 0
    c0 = 0
    
    
    T = 0.03
    H = 0.2
    M = 15.9
    
    thres_no = nbits-1
    
    I = np.ceil(fs * H)
    g = np.exp(-1/(fs*T))
    
    
    c = (2*np.ones(thres_no))**np.arange(-15,0)
    a = np.zeros(thres_no)
    h = np.ones(thres_no)
    
    sq = np.sum(np.power(x,2))
    x_len = x.size
    x_abs = np.abs(x)
    
    asl_rms = 0
    
    p = 0
    q = 0
        
    for k in range(x_len):
        
        p = g*p + (1-g)*abs(x[k])
        q = g*q + (1-g)*abs(p)
        
        for j in range(thres_no):
            if (q>=c[j]):
                a[j] = a[j] + 1
                h[j] = 0
            elif h[j]< I:
                a[j] = a[j] + 1
                h[j] = h[j] + 1
            else:
       
                break
 
    eps = np.finfo(float).eps
        
    
  
    AdB = np.zeros(thres_no)
    CdB = np.zeros(thres_no)
    
    if a[0] == 0:
        return asl_ms, asl, c0

    else:
        AdB[0] = 10 * np.log10(sq/a[0] + eps)
    
    CdB[0] = 20 * np.log10(c[0] + eps)     
    
    if (AdB[0] - CdB[0]) < M:
        return asl_ms, asl, c0
    

    Delta = np.zeros(thres_no)
    Delta[0] = AdB[0] - CdB[0]
    
    for j in range(1,thres_no):
        AdB[j] = 10 * np.log10( sq/(a[j] + eps) + eps)
        CdB[j] = 20 * np.log10( c[j] + eps)
        
    for j in range(1, thres_no):
        if a[j] != 0:
            Delta[j] = AdB[j] - CdB[j]
            if Delta[j] <= M:
                asl_ms_log, c10 = bin_interp(AdB[j], AdB[j-1], CdB[j], CdB[j-1], M, 0.5)
                # print(AdB[j], AdB[j-1], CdB[j], CdB[j-1], M, 0.5)
                
                asl_ms = 10 ** (asl_ms_log/10)
                asl = (sq/ x_len) / asl_ms
                c0 = 10** (c10/ 20)
                break
                
    return asl_ms, asl, c0
            
            

