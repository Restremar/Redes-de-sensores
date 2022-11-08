import serial
import time 
import csv

ser = serial.Serial('COM4',115200)
ser.flushInput()
time.sleep(1) #Para asegurar la conexion modo sleep de 1 segundo

while True:
    ser_bytes = ser.readline()
    getData=ser.readline()
    dataString = getData.decode('utf-8')
    data=dataString[0:][:-2]
    print(data)
    with open("test_data.csv","a") as f:
        writer = csv.writer(f,delimiter=",")
        writer.writerow([time.time(),data])
