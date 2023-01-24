En esta practica se plantea:

- Poner en práctica los conocimientos de redes inalámbricas WIFI

- Manejar comunicaciones IP a bajo nivel mediante sockets y entre diversas 
plataformas: PC, Móvil, CLOUD, Sensor

- Manejar protocolos de alto nivel: HTTP, FTP, NTP, MQTT y estándares de 
interoperabilidad: SENML

1.FECHA_Y_HORA: Monta un chat con una aplicacion software PC 
(https://www.hw-group.com/software/hercules-setup-utility) con uno de los extremos
siendo el modulo hardware como cliente y enviando cada segundo la hora local.

2. START_STOP: Se añade una capa de control al programa anterior para que cuando 
se mande "start" empiece a mandar la hora y cuando se escriba "stop" pare.

3. SERVIDOR_WEB: Monta un servidor web que muestre la hora y tenga un boton para resetear
la hora a las 0:00.

4. JSON: Basandose en el estandar SENML, se crea un fichero json que se genere cada 10 s
que contiene datos de temperatura aleatorios, las unidades y la marca temporal. Y se 
se sube el archivo a un servidor ftp con un nombre que sea grupoXX_ddmmss.jon.

5. ADAFRUIT: Se suscribe a un feed y va aumentando un contador para comprobar que recibe
la actualizacion al aumentarlo.

