import serial
import time 
import csv


samples = 10
fileName = "test_data.csv"
print_labels = False 
sensor_data = []

ser = serial.Serial('COM4',115200)
ser.flushInput()
file = open(fileName,"w")
time.sleep(1) #Para asegurar la conexion modo sleep de 1 segundo

line = 0
while line <= samples:

    if print_labels:
        if line==0:
            print("Printing column headers")
        else:
            print("Line " + str(line) + ": writing...")
    ser_bytes = ser.readline()
    getData=ser.readline()
    dataString = getData.decode('utf-8')
    data=dataString[0:][:-2]
    print(data)

    readings = data.split(",")
    readings2 = readings.split(",")

    line = line+1

    with open(fileName,'a', encoding='UTF8', newline='') as f:
        writer = csv.writer(f, delimiter= ",")
        writer.writerow(readings2)
        

print("Data collection complete!")
file.close()

