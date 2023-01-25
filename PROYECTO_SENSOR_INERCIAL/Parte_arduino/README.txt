En este programa se muestrea cada 40ms mediante freeRTOS, y se van almacenando los datos 
hasta que cada 10 segundos se genera un archivo Json con los datos recogidos y se
envia a un servidor FTP para su posterior analisis. 

Al mismo tiempo se envian mediante puerto serie los datos cada 40ms para que con el programa
de python se ploteen los datos y se haga un recuento de cuantas repeticiones se han hecho
de cierto ejercicio.