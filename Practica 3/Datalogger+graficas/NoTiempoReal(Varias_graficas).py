import serial
import matplotlib
matplotlib.use("tkAgg")
import matplotlib.pyplot as plt
import numpy as geek
from scipy.signal import find_peaks

def main():
    ax = []
    ay = []
    az = []
    asqrt = []
    amean = []
    a = []

    while True:
        try:
                ser = serial.Serial("COM4", 115200)
                while True: 
                    ser.flushInput() #Limpio el buffer, sobre todo para cuando escribamos en fichero
                    try:
                        
                        for i in range(500):
                            acel = ser.readline().decode("utf-8") #Leo una linea completa y decodifico para poder quitar los saltos de linea
                            acel = acel.replace("\n","") #quito salto de linea
                            acel = acel.replace("\r","") #reemplazo retorno de carro

                            a=acel.split(",")
                            print(a)

                            try:
                                for i in range(5):
                                    c = float(a[i])
                                    print(c)

                                    with open ("datos.txt","a") as f:
                                        f.write(acel)
                                        f.write('\n')
                                    
                                    ax.append(float(a[0])) 
                                    ay.append(float(a[1]))
                                    az.append(float(a[2]))
                                    asqrt.append(float(a[3]))
                                    amean.append(float(a[4]))
                            except:
                                continue

                        tamaño  = 30*geek.size(ax)/1500;
                        tiempo = geek.linspace(0,tamaño,geek.size(ax)) # Creo eje de tiempos para plotear frente a los 
                                                                       #valores de aceleración

                        psqrt = geek.linspace(0,tamaño,geek.size(asqrt))
                        psqrt = geek.append(asqrt, geek.size(asqrt))

                        #Find peaks
                        peaks = find_peaks(psqrt, height= 13.0, distance= 5.0, prominence= 1.5)
                        height = peaks[1]['peak_heights'] #list containing the height of the peaks
                        peak_pos = tiempo[peaks[0]] #list of the peaks positions

                        #Finding the minima
                        sqrt2 = psqrt*-1
                        minima = find_peaks(sqrt2, distance= 5.0, prominence= 1.5)
                        min_pos = tiempo[minima[0]] #list of the minima positions
                        min_height = sqrt2[minima[0]] #list of the mirrored minima heights

                        #Creo las graficas y las ploteo
                        fig, axs = plt.subplots(2,3)
                        axs[0,0].plot(tiempo,ax)
                        axs[0,0].set_title('Aceleracion X')
                        axs[0,1].plot(tiempo,ay,'tab:orange')
                        axs[0,1].set_title('Aceleracion Y')
                        axs[0,2].plot(tiempo,az,'tab:green')
                        axs[0,2].set_title('Aceleracion Z')
                        axs[1,0].plot(tiempo,asqrt, 'tab:blue')
                        axs[1,0].set_title('Aceleracion SQRT')
                        axs[1,1].plot(tiempo, amean, 'tab:red')
                        axs[1,1].set_title('Aceleracion PROMEDIO')
                        axs[1,2].plot(tiempo, asqrt, 'tab:blue')
                        axs[1,2].set_title('Maximos y minimos')
                        axs[1,2].scatter(peak_pos, height, color = 'r', s = 15, marker = 'D', label = 'Maxima')
                        axs[1,2].scatter(min_pos, min_height*-1, color = 'g', s = 15, marker = 'X', label = 'Minima')

                        for ax in axs.flat:
                            ax.set(xlabel='Tiempo', ylabel='Aceleración')

                        # Hide x labels and tick labels for top plots and y ticks for right plots.
                        for ax in axs.flat:
                            ax.label_outer()

                        plt.savefig("Aceleración.jpg") #Me lo guardo en un fichero para poder verlo
                        plt.show() #La muestro
                        a = [] 

                    except serial.SerialException: #En caso de no poder acceder a leer los datos del puerto seria saltara una excepcion
                        print("No se pueden leer los datos")     
                        ser.close()#Cerraremos el puerto serie
        except serial.SerialException:#Si no se puede acceder al puerto seria saltara una excepcion
            print("No esta conectado el puerto")

if __name__ == '__main__':
	main()
    