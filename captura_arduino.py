import serial
import time
import csv



serial_port = 'COM3' 
baudrate= 115200
filename = 'datos_entrada_id.csv'

try: 
    print(f"Intentando conexión al puerto serie {serial_port}")
    arduino = serial.Serial(serial_port, baudrate=baudrate)

    #Se esperan 2 segundos para darle tiempo al reinicio del arduino y comenzar a leer datos
    time.sleep(2)

    #Se limpia el buffer de entrada para no leer basura
    arduino.reset_input_buffer() 
    
    print("Enviando secuencia de inicio (abcd)...")

    #Se envían los 4 bytes de la secuencia de inicio
    arduino.write(b'abcd')


    print("Conexión exitosa - el arduino se reinició y comienza la ejecución")
    print(f"Se guardan los datos en: {filename}")


    #Se abre el archivo creado en modo escritura
    with open(filename, mode='w', newline='') as csv_file:



        #Se inicializa el escritor del csv
        writer = csv.writer (csv_file)
        
        #Se escribe en la primer fila los nombres de las columnas
        writer.writerow(['time (ms)', 'signal'])

        while True:

            #Si ya hay datos esperando en el puerto serial
            if arduino.in_waiting > 0:
                
                #---------------- INICIO - PARSEO

                #Se lee la línea, se decodifican los bytes a texto usando utf-8 y se aplica strip para borrar
                # espacios en blanco, saltos de linea y tabulaciones al principio y al final de la linea
                line = arduino.readline().decode('utf-8').strip()

                #Se separan los datos utilizando la coma como separador 
                data = line.split(',')

                #---------------- FIN - PARSEO

                #Se usa el escritor para escribir una nueva línea con los datos ya parseados.
                if len(data) == 4:
                    writer.writerow(data)
                    print(data)

##---------------- INICIO - TRATAMIENTO DE ERRORES

#Error con el acceso al puerto serial
except serial.SerialException:
    print(f"\n[ERROR]No se pudo abrir el puerto {serial_port}.")
    print("Verificar que el nombre del puerto sea correcto y que el Serial Monitor esta cerrado")

#Para frenar el codigo
except KeyboardInterrupt:
    print("\n\n[INFO] Tuki.")

#Siempre se busca cerrar el puerto al finalizar la toma o al haber errores
finally:
    if 'arduino' in locals() and arduino.is_open:
        arduino.close()
        print(f"[INFO] Se cerró el puerto {serial_port} correctamente.")
        print(f"[INFO] Se guardaron los datos en {filename}")