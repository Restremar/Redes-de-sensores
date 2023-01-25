Programa en python que se divide en dos programas:

-TIEMPO_REAL(UNA_GRAFICA): Recoge los datos en crudo que le vienen por el puerto serie, 
y plotea 1 parametro en tiempo real, de modo que se puede ver como varia. Ademas de escrbir
en un csv lo que va leyendo y poniendo una stamp de tiempo por cada parametro que lee.

-NO_TIEMPO_REAL(VARIAS_GRAFICAS): En este programa recoge hasta 5 datos a la vez y los 
va almacenando hasta que pasan 10 segundos, de modo que una pasado ese tiempo plotea 5
graficas con la evolucion de cada parametro y determina la cantidad de repeticiones
que se han hecho de uno de los parametros, en función de la altura máxima, la distancia
entre picos y la prominencia.