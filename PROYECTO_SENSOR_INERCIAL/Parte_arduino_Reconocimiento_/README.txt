Segundo programa de arduino, en el cual no requiere ningun programa secundario de python.

En adición al otro programa de arduino este añade un boton para empezar a contar, de modo que
trascurridos 10 segundos calcula la media, desviacion tipica y varianza de la aceleracion
del acelerometro en los ejes x, y, z y la de su sqrt de los mismos, de modo que con esos
parametros es capaz de distinguir hasta dos movimientos precisos, hombro lateral y press
militar. 

Como el anterior programa, se crea un archivo json con los resultados obtenidos, y se envia
por wifi a un servidor FTP.