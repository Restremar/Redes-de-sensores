En esta practica se plantea:

- Familiarizarse con Python como lenguaje de programación

- Gestionar datos procedentes de comunicaciones serie

- Trabajar con estos datos en formato csv y json para almacenarlos en ficheros y redireccionarlos vía internet

- Operar con los datos y representarlos gráficamente

1. DATALOGGER: 

	a. GRAFICAS:Lee por el puerto serie los datos del acelerometro, y va mostrando 
	en tiempo
	real como cambia el parametro, ademas de crear un archivo csv y escribir los datos.

	b. NO_TIEMPO_REAL (VARIAS GRAFICAS): Lee por el puerto serie los datos, los divide 
	en distintos arrays, acumula durante 10 segundos y plotea los parametros leidos, 
	ademas de contar las repeticiones hechas de un cierto movimiento, y escribe en un 
	archivo de texto los datos adquiridos. 

2. SENSOR_ARDUINO: Lee los parametros del sensor y cada 40 ms los envia por el puerto serie, para 
ser procesador por python.