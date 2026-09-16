import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

init_time = 0 #secs
sampling_rate = 0.1 # secs (tiempo de muestreo analógico, el tiempo del OB30 en el PLC)

filename = "Mediciones_recortadas.csv"

df = pd.read_csv(filename)

#Se extraen las columnas como listas y se convierten a Arrays de numpy
time = df.iloc[:, 0].to_numpy()
acceleration = df.iloc[:, 1].to_numpy()
pwm = df.iloc[:, 2].to_numpy()