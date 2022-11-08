# -*- coding: utf-8 -*-
"""
Created on Fri Nov  4 11:58:17 2022

@author: raul
"""

import serial
import time
import csv
import matplotlib
matplotlib.use("tkAgg")
import matplotlib.pyplot as plt
import numpy as np

ser = serial.Serial('COM4',115200)
ser.flushInput()

plot_window = 20
y_var = np.array(np.zeros([plot_window]))

plt.ion()
fig, ax = plt.subplots()
line, = ax.plot(y_var)

while True:
    ser_bytes = ser.readline()
    getData=ser.readline()
    dataString = getData.decode('utf-8')
    data=dataString[0:][:-2]
    with open("test_data.csv","a") as f:
        writer = csv.writer(f,delimiter=",")
        writer.writerow([time.time(),data])
    y_var = np.append(y_var,data)
    y_var = y_var[1:plot_window+1]
    line.set_ydata(y_var)
    ax.relim()
    ax.autoscale_view()
    fig.canvas.draw()
    fig.canvas.flush_events()
