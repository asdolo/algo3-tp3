#TEST tiempos A
import os
import glob
import re
i = 0
cantidadRepeticiones = 1
fi = open("tiempo1.csv",'w+')
fi.write("n,tiempo,resultado\n")
fi.close()
fi = open("tiempo2.csv",'w+')
fi.write("n,tiempo,resultado\n")
fi.close()
fi = open("tiempo3.csv",'w+')
fi.write("n,tiempo,resultado\n")
fi.close()
fi = open("optimo.csv",'w+')
fi.write("n,optimo\n")
path = "../../../input/B/*.vrp"
R = 1
for VECINDAD in range(1,4):
    for filename in glob.glob(path):
        print "Ejecutando simulated-annealing en " + filename + " con vecindad="+str(VECINDAD)
        os.system("../../../build/bin/./5-simulated-annealing rutas"+str(i)+".csv tabla"+str(i)+".csv tiempo"+str(VECINDAD)+".csv "+ str(cantidadRepeticiones) +" "+str(R)+ " "+str(VECINDAD)+" < " + filename)
        i=i+1
        file = open(filename)
        for line in file:
            if re.match("DIMENSION : ", line) is not None:
                fi.write(line[12:len(line.strip())])
                fi.write(",")
        if VECINDAD==1:
            filename=filename.replace(".vrp",".sol")
            file = open(filename)
            for line in file:
                if re.match("cost", line) is not None:
                    indice=0
                    j = 0
                    for caracter in line:
                        if caracter=='\n':
                            indice=j
                        j=j+1
                    if indice==0:
                        fi.write(line[5:])
                    else:
                        fi.write(line[5:indice])
                if re.match("Cost", line) is not None:
                    indice=0
                    j = 0
                    for caracter in line:
                        if caracter=='\n':
                            indice=j
                        j=j+1
                    if indice==0:
                        fi.write(line[5:])
                    else:
                        fi.write(line[5:indice])
            file.close()
            fi.write("\n")
fi.close()
