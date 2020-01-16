# -*- coding: utf-8 -*-
"""
Created on Thu Feb 14 15:04:43 2019

@author: Pertum

adaptive

"""
'''
%--------------------------------------------------------------------------
% Inputs variables:
% w          Weight behavior
% x(n)       Input signal
% d(n)       Desired signal
% N          Number os iterations
% M          Filter order
%-------------------------------------------------------------------------
'''
#%%
import numpy as np
import math

def VSSNLMS_Algorithm_Zipf_AP2(N, M, beta, ep, x, d, hist):
    xtemp=np.zeros(M)
    w=np.zeros(M)
    e=np.zeros(N)
    y=np.zeros(N)
    

    if hist==1:
        whist=[]
    else:
        whist=0

    p=0.0
    q=0.0
    e_old=0.0
    for k in range(N):
        
        # shift of the vector xtemp
        xtemp = np.roll(xtemp,1)
        xtemp[0]=x[k]
        
        y[k] =  np.dot(w,xtemp)
        
        e[k]= d[k] - y[k]
        
        p = beta*p + (1-beta)*e[k]*e_old
        q = beta*q + (1-beta)*(e[k]**2)
        
        MU= (p/q)**2
        
        power_x = np.dot(xtemp,xtemp)
        
        fact = (MU * e[k])/(power_x + ep)
        aux = [data*fact for data in xtemp]
        w = [(a+b) for a,b  in zip(w, aux)]
        
        if hist==1:
            whist.append(w)
            
        e_old = e[k]
        
    J = np.power(e,2)
        
    return y, w, whist, e, J
    
def VSSNLMS_Algorithm_Shin(N,M,alfa, C, mu_max, ep, x, d,hist):
    xtemp=np.zeros(M)
    w=np.zeros(M)
    p=np.zeros(M)
    x_energy = 0
    
    e=[]
    y=[]
    
    
    if hist==1:
        whist=[]
    else:
        whist=0
    
    for k in range(N):
        
        # shift of the vector xtemp
        xtemp = np.roll(xtemp,1)
        xtemp[0]=x[k]
        
        y.append(np.dot(w,xtemp))
        
        e.append(d[k] - y[k])
        
        x_energy = np.dot(xtemp,xtemp)
        
        aux1 = [alfa*data for data in p]
        aux2 = [data*((1-alfa)*e[k]/x_energy) for data in xtemp]
        
        p = [(a+b) for a,b in zip(aux1,aux2)]

        p_energy = np.dot(p,p)
        
        mu = mu_max*p_energy/(p_energy + C)
        
        aux = [data*mu*e[k]/(x_energy+ep) for data in xtemp]
        w = [(a+b) for a,b in zip(w,aux)]
        
        if hist==1:
            whist.append(w)
            
    J = np.power(e,2)
    
    return y, w, whist, e, J


def NPVSS_NLMS_Algorithm(N,M,lbda,ep,x,d,hist):

    xtemp=np.zeros(M)
    etemp=np.zeros(M)
    cor_error_x=np.zeros(M)
    w=np.zeros(M)
    
    e=np.zeros(N)
    y=np.zeros(N)
    
    var_x=0
    var_e=0
    var_v=0
        
    if hist==1:
        whist=[]
    else:
        whist=0
    
    for k in range(N):
        
        # shift of the vector xtemp
        xtemp = np.roll(xtemp,1)
        xtemp[0]=x[k]
        
        etemp = np.roll(etemp,1)

        
        y[k] = np.dot(w,xtemp) 
                
        
        e[k] = d[k] - y[k]
                
        etemp[0] = e[k]

        delta = 20*var_x        
                    
        # update
        # =====================================================================
        power_x=np.dot(xtemp,xtemp)
        var_x = (1.0/M)*power_x
        
        aux1 = [data*lbda for data in cor_error_x]
        aux2 = [data*(1.0-lbda)*e[k] for data in xtemp]
        
        cor_error_x = [(a+b) for a,b in zip(aux1,aux2)]
        
        power_cor_error_x = np.dot(cor_error_x,cor_error_x)
        
#        var_e = etemp.var()
        var_e = np.dot(etemp, etemp)/M
       
        var_v = var_e - (power_cor_error_x/var_x)
        # =====================================================================
                       
        if (var_e < 0):
            var_e = 0
        if (var_v < 0):
            var_v = 0
 
        desv_e = math.sqrt(var_e)
        desv_v = math.sqrt(var_v)


        beta = (1.0/(delta + power_x))*(1.0-(desv_v/(ep+desv_e)))
        
        
        if desv_e >= desv_v:
            mu_npvss = beta 
        else:
            mu_npvss = 0.0         

                
        aux = [data*(mu_npvss*e[k]) for data in xtemp]
           


        w = [(a+b) for a,b in zip(w,aux)]
        
        if hist==1:
            whist.append(w)
            
    J = np.power(e,2)
    
    return y, w, whist, e, J


def JO_NLMS_Algorithm_2016(N,M,lbda,ep,x,d,hist):

    xtemp=np.zeros(M)
    etemp=np.zeros(M)
    cor_error_x=np.zeros(M)
    w=np.zeros(M)
    
    e=[]
    y=[]
    
    var_x=0
    var_e=0
    var_v=0
    var_w=0
    
    p=0
    q=0
    m=1
    
    
    if hist==1:
        whist=[]
    else:
        whist=0
    
    for k in range(N):
        
        # shift of the vector xtemp
        xtemp = np.roll(xtemp,1)
        xtemp[0]=x[k]
        
        # shift of the vector xtemp
        etemp = np.roll(etemp,1)

        
        y.append(np.dot(w,xtemp))
        
        e.append( d[k] - y[k])

        
        etemp[0] = e[k]
            
        p = m + (M*var_w)
        
        # update
        # =====================================================================
        power_x=np.dot(xtemp,xtemp)
        var_x = power_x/M
        
        aux1 = [data*lbda for data in cor_error_x]
        aux2 = [data*(1-lbda)*e[k] for data in xtemp]
        
        cor_error_x = [(a+b) for a,b in zip(aux1,aux2)]
        
        power_cor_error_x = np.dot(cor_error_x,cor_error_x)
        
        var_e = etemp.var()
        
        var_v = var_e - (power_cor_error_x/var_x)
        # =====================================================================
        
        q = p/(M*var_v + (M+2)*p*var_x)
        
        u = [data*q*e[k] for data in xtemp]
        
        w = [(a+b) for a,b in zip(w,u)]
        
        m = (1-q*var_x)*p
        
        power_u = np.dot(u,u)
        var_w = power_u/M
                
        if hist==1:
            whist.append(w)
            
    J = np.power(e,2)
    
    return y, w, whist, e, J
    

    
def NLMS_Algorithm(N, M, MU, ep, x, d, hist):
    xtemp=np.zeros(M)
    w=np.zeros(M) 
    e=np.zeros(N)
    y=np.zeros(N)
    
    
    if hist==1:
        whist=[]
    else:
        whist=0
    
    for k in range(N):
        
        # shift of the vector xtemp
        xtemp = np.roll(xtemp,1)
        xtemp[0]=x[k]
        
        y[k] = np.dot(w,xtemp)
        
        e[k]= d[k] - y[k]
        
        power_x = np.dot(xtemp,xtemp)
    
        aux = [data*((MU*e[k])/(ep + power_x)) for data in xtemp]
        
        w = [(a+b) for a,b in zip(w,aux)]
        
        if hist==1:
            whist.append(w)
            
    J = np.power(e,2)
    
    return y, w, whist, e, J