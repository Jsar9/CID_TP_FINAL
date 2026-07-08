# -*- coding: utf-8 -*-
"""
Created on Fri Jun 14 17:11:44 2024

@author: Control Industrial Distribuido
"""

import socket
import struct
import time
import csv
import numpy as np
import matplotlib.pyplot as plt

# Configurar puerto y dirección IP de la PC (Servidor) -> PLC (Cliente)
HOST = "192.168.2.115"  #
PORT = 2000  #

# Inicializar listas para guardar los datos recibidos y graficar
y = np.zeros((4,1000)) # 4 variables: temp, setpoint, ctrl, ctrlpwm
x = np.arange(0,1000)  # vector de indices para graficar: [0,...,1000]

# 
plt.ion()
 
# Crear la figura que vamos a ir actualizando con los datos
figure, (ax1, ax2, ax3) = plt.subplots(3,1,figsize=(10, 10))

# subfigura 1: posicion
line11, = ax1.plot(x, y[0,:], color='b')
ax1.legend(["posicion"])
ax1.set_ylim([0,120])
ax1.grid(True)
ax1.set_ylabel("Posicion")

# subfigura 2: velocidad
line13, = ax2.plot(x, y[1,:], color='r')
ax2.legend(["velocidad"])
ax2.set_ylim([-200,200])
ax2.grid(True)
ax2.set_ylabel("Velocidad")

figure.suptitle("Planta Térmica", fontsize=20)

# nombre del archivo csv: output-<timestamp>.csv
timestr = time.strftime("%Y%m%d-%H%M%S")
with open("output"+timestr+".csv",'w') as file:
    writer = csv.writer(file, delimiter=',',lineterminator='\n')
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock: # Crea el socket
        sock.bind((HOST, PORT)) # Conecta con el socket
        sock.listen() # Escucha las conexiones entrantes
        # Espera por una conexión
        print("Waiting for connection ...")
        conn, addr = sock.accept() # addr: dirección IP del cliente (PLC)
        with conn:
            print(f"Conexión con {addr}")
            try: 
                while True:
                    data = conn.recv(8) # tipos de datos en S7-1200: REAL (4 bytes), DInt (4 bytes)
                    if not data:
                        break
                    datareal_pos  = struct.unpack('>f',data[0:4])[0]
                    datareal_vel  = struct.unpack('>f',data[4:8])[0]

                    # verificamos los datos recibidos imprimiendo por linea de comandos
                    print(f"recibido: posicion={datareal_pos}, velocidad={datareal_vel}")
                    
                    # desplaza los elementos de la lista y en la que guardamos las mediciones
                    y = np.roll(y,-1,axis=1)
                    # agregamos las nuevas lecturas recibidas
                    y[:,-1] = [datareal_pos,datareal_vel]
                    # actualizamos los plots
                    line11.set_xdata(x)
                    line11.set_ydata(y[0,:])
                    line13.set_xdata(x)
                    line13.set_ydata(y[1,:]) # antes había un 2, chequear si anda
                    # dibuja los valores actualizados
                    figure.canvas.draw()
                    figure.canvas.flush_events()
                    
                    # escribe una linea en el archivo csv
                    writer.writerow(y[:,-1])

            except KeyboardInterrupt:
                print("Chaucha!")
            finally:
                # Cierra la conexión
                conn.close()