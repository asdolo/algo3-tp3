#TEST Complejidad
import os
import glob
import re
def find_nth(haystack, needle, n):
    start = haystack.find(needle)
    while start >= 0 and n > 1:
        start = haystack.find(needle, start+len(needle))
        n -= 1
    return start
i = 0
fi = open("tiempo.csv",'w+')
fi.write("n,tiempo,resultado\n")
fi.close()
fi = open("optimo.csv",'w+')
fi.write("optimo\n")
path = "../../input/*/*.vrp"
for filename in glob.glob(path):
    print "Ejecutando savings en " + filename
    os.system("../../build/bin/./1-savings rutas"+str(i)+".csv tabla"+str(i)+".csv tiempo.csv 3 < " + filename)
    i=i+1
    filename=filename.replace(".vrp",".sol")
    file = open(filename)
    for line in file:
        if re.match("cost", line) is not None:
            fi.write(line[5:])
    file.close()
    fi.write("\n")
fi.close()
