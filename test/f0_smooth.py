#!/usr/bin/env python

"""Usage:
     python f0_smooth.py infile outfile
"""

import sys
from scipy.signal import butter
from scipy.signal import filtfilt
#from scipy.signal import array
from numpy import array

def interpolation(infile):
    f = open(infile, 'r')
    lines = f.readlines()
    f.close()
    i = 0
    f0s = []
    while (i < len(lines)):
        if (float(lines[i].split()[0]) == 0):
            st = i
            i += 1
            while ((i < len(lines)) and (float(lines[i].split()[0]) == 0)):
                i += 1
            en = i
            if (st <> 0) and (en <> len(lines)):
                # do interpolation
                beg = float(lines[st-1].split()[0])
                end = float(lines[en].split()[0])
                for m in range(st, en):
                    f0 = beg + (end-beg)/(en-st+1)*(m-st+1)
                    f0s.append(f0)
            #the 0s at the beginning and the the end
            else:
                for k in range(st, en):
                    f0s.append(float(lines[k].split()[0]))
        else:
            f0s.append(float(lines[i].split()[0]))
            i += 1
    return f0s

if __name__ == '__main__':
    try:
        infile = sys.argv[1]
        outfile = sys.argv[2]
    except IndexError:
        print __doc__
    
    f0s = interpolation(infile)
    i = 0
    while (f0s[i] == 0):
         i += 1
    j = len(f0s) - 1
    while (f0s[j] == 0):
        j -= 1
    [b, a] = butter(3, 0.1)
    y = filtfilt(b, a, array(f0s[i:j+1]))
    fw = open(outfile, 'w')
    for k in range(0, i):
        fw.write('0.0\n')
    for k in range(len(y)):
        fw.write(str(y[k]) + '\n')
    for k in range(j+1, len(f0s)):
        fw.write('0.0\n')
    fw.close()

