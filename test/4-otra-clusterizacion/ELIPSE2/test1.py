#TEST ELIPSE
import os
import glob
import re
i = 0
cantidadRepeticiones = 1
fi = open("tiempo.csv",'w+')
fi.write("n,tiempo,resultado\n")
fi.close()
fi = open("optimo.csv",'w+')
fi.write("n,optimo\n")
path = "../../../input/ELIPSE/elipse2.vrp"

for filename in glob.glob(path):
    print "Ejecutando otra-clusterizacion en " + filename
    os.system("../../../build/bin/./4-otra-clusterizacion rutas"+str(i)+".csv tabla"+str(i)+".csv tiempo.csv "+ str(cantidadRepeticiones) +" < " + filename)
    i=i+1
    file = open(filename)
    for line in file:
        if re.match("DIMENSION : ", line) is not None:
            fi.write(line[12:len(line.strip())])
            fi.write(",")
    fi.write("\n")
fi.close()
