import serial
import time
import csv
import matplotlib
matplotlib.use("tkAgg")
import matplotlib.pyplot as plt
import numpy as np

plot_window = 20
y_var = np.array(np.zeros([plot_window]))

plt.ion()
fig, ax = plt.subplots()
line, = ax.plot(y_var)

while True:
    try:
        ser = serial.Serial("COM4", 115200)
        cont = 0
        while True: 
            ser.flushInput() #Limpio el buffer, sobre todo para cuando escribamos en fichero
            try:
                    ser_bytes = ser.readline()
                    try:
                        decoded_bytes = float(ser_bytes[0:len(ser_bytes)-2].decode("utf-8"))
                        print(decoded_bytes)
                    except:
                        continue
                    with open("test_data.csv","a") as f:
                        writer = csv.writer(f,delimiter=",")
                        writer.writerow([time.time(),decoded_bytes])
                    y_var = np.append(y_var,decoded_bytes)
                    y_var = y_var[1:plot_window+1]
                    line.set_ydata(y_var)
                    ax.relim()
                    ax.autoscale_view()
                    fig.canvas.draw()
                    fig.canvas.flush_events()

                    plt.savefig("Aceleración.jpg") #Me lo guardo en un fichero para poder verlo
            except serial.SerialException: #En caso de no poder acceder a leer los datos del puerto seria saltará una excepción
                print("No se pueden leer los datos")     
                ser.close()#Cerraremos el puerto serie
    except serial.SerialException:#Si no se puede acceder al puerto seria saltará una excepción
        print("No está conectado el puerto")