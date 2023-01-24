En esta practica se plantea la configuracion y control de diferentes modulos de hardware:

1. POTENCIOMETRO: Con una periodicidad de un segundo, lee el valor del ADC y muéstralo por
consola. Utiliza la salida de 3V3 de la placa con un potenciómetro para generar la tensión 
analógica. Antes de conectarla al módulo comprueba que no superas 3V3 con un multímetro.

2. INTERRUPCION: Utilizando los timer hardware genera una interrupción cada 10s que lea el 
ADC y lo muestre por pantalla.

3. PROTOCOLO UART: Comanda por la UART los periféricos mediante un protocolo de tal modo que
si le mandas:

a. ADC: Envíe la lectura del ADC actual

b. ADC(x): envíe la lectura del ADC cada x segundos. Si x=0, deja de mandar datos. 
   Utiliza interrupción de Timer para la temporalización.

c. PWM(x): comanda el duty cycle del módulo PWM con números del 0 al 9.

4. SENSOR_INERCIAL: Conecta un sensor inercial por I2C (o SPI), muestrea la aceleración
cada 100 ms y manda los datos cada segundo vía UART (cada vez que envíes activa un LED durante 200ms).