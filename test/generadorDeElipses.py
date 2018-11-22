import os
import glob
import re
import numpy as np
import math
def puntosY(a,b,x):
    hola = math.sqrt(1-((x**2)/(a**2))) * b
    if hola != 0:
        return [hola,-hola]
    else : 
        return [hola]
a=10
b=10000
incremento=1.25
b=10000*(a/incremento)
id = 0
cantidadNodosX = (2*a/incremento)+1
cantidadNodosTotal = (cantidadNodosX-2)*2+2
ids = []
for x in np.arange(-a,a+incremento,incremento):
    arreglo = puntosY(a,b,x)
    for i in arreglo:
        if i>=0:
            ids.append((id+2,x,i,90 if x!=a else 10))
            #print str(id+2)+" "+str(x)+" "+str(i)
            id=id+1
        else:
            ids.append(((cantidadNodosTotal - id +1)+2,x,i,10))
            #print str((cantidadNodosTotal - id +1)+2)+" "+str(x)+" "+ str(i)

ids.sort(key=lambda tup: tup[0])  # sorts in place
for tup in ids:
    print " "+str(int(tup[0]))+" "+str(tup[1])+" " + str(tup[2])
print "demands"
for tup in ids:
    print str(int(tup[0]))+" "+str(tup[3])    

  
    