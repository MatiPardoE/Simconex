import csv
import random

def generate_csv(filename, rows):
    with open(filename, mode='w', newline='') as file:
        writer = csv.writer(file)

        for i in range(rows):
            row = [
                f"{i:08d}",  # Secuencial con ancho fijo de 8 dígitos
                f"{random.uniform(7.0, 10.0):05.2f}",   # Número aleatorio entre 7.0 y 10.0, ancho de 5 caracteres
                f"{random.uniform(80.0, 90.0):06.2f}",   # Número aleatorio entre 80.0 y 90.0, ancho de 6 caracteres
                f"{random.uniform(20.0, 30.0):05.2f}",   # Número aleatorio entre 20.0 y 30.0, ancho de 5 caracteres
                f"{random.randint(20, 30):02d}"          # Entero aleatorio entre 20 y 30, ancho de 2 caracteres
            ]
            writer.writerow(row)

# Generar un archivo CSV con 10 filas de ejemplo
generate_csv("data.csv", 5000)
