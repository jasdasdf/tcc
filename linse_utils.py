import matplotlib.pyplot as plt
import numpy as np
import locale
import matplotlib
matplotlib.rcParams['pdf.fonttype'] = 42
matplotlib.rcParams['ps.fonttype'] = 42

locale.setlocale(locale.LC_ALL, ("pt_BR.UTF-8"))
matplotlib.rcParams['axes.formatter.use_locale'] = True

def format_figure(fig, figsize=(9, 5.5), scilimits=(-1,1)):

    # figsize defaults:
    # 9cm x 4.5cm (paper)
    # 16cm x 10cm (screen)

    locale.setlocale(locale.LC_ALL, ("pt_BR.UTF-8"))
    matplotlib.rcParams['axes.formatter.use_locale'] = True
    
    # Assumes a single plot axes
    ax = fig.axes[0]

    if figsize[0] == 9 and figsize[1] == 5.5:
        # Default number ticks fonts
        par_ticks = {'fontname': 'Arial', 'fontsize': 8}
        
        # Default axis labels fonts
        par_labels = {'family': 'Times New Roman', 'fontsize': 10}
    elif figsize[0] == 16 and figsize[1] == 10:
        # Default number ticks fonts
        par_ticks = {'fontname': 'Arial', 'fontsize': 10}
        
        # Default axis labels fonts
        par_labels = {'family': 'Times New Roman', 'fontsize': 12}
    else:
        # Default number ticks fonts
        par_ticks = {'fontname': 'Arial', 'fontsize': 10}
        
        # Default axis labels fonts
        par_labels = {'family': 'Times New Roman', 'fontsize': 12}
    
    # Convert from cm to inch
    figsize = list(figsize)
    figsize[0] /= 2.54
    figsize[1] /= 2.54

    # Adjust figure size
    fig.set_size_inches(figsize)
    # fig.set_dpi(200)
    
    # Adjust the font of x and y labels
    ax.set_xlabel(ax.get_xlabel(), **par_labels)
    ax.set_ylabel(ax.get_ylabel(), **par_labels)
   
    # Set the font settings for axis tick labels
    for tick in ax.get_xticklabels():
        tick.set_fontname(par_ticks['fontname'])
        tick.set_fontsize(par_ticks['fontsize'])
        
    for tick in ax.get_yticklabels():
        tick.set_fontname(par_ticks['fontname'])
        tick.set_fontsize(par_ticks['fontsize'])
    
    # Add scientific notation ticks for axis in the (10 ** scilimits) interval
    ax.ticklabel_format(axis='both', style='scientific', scilimits=scilimits, useMathText=True, useOffset=True)

    # Set the font settings for axis offset text (a.k.a. scientific notation)
    ax.xaxis.offsetText.set_fontname(par_ticks['fontname'])
    ax.xaxis.offsetText.set_fontsize(par_ticks['fontsize'])
    ax.yaxis.offsetText.set_fontname(par_ticks['fontname'])
    ax.yaxis.offsetText.set_fontsize(par_ticks['fontsize'])

    # Really tight layout (default is ~1.0)
    fig.tight_layout(pad=0.1, h_pad=None, w_pad=None, rect=None)

    return fig








