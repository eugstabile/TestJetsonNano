import numpy as np
import matplotlib.pyplot as plt
 
input_file_path = 'build/clientApp/Release/frame.raw'

# Función para leer un archivo binario y convertirlo en un array de NumPy
def read_binary_file(file_path):
    with open(file_path, 'rb') as f:
        raw_array = np.fromfile(f, dtype=np.uint16)
    
    return raw_array

try:
    raw_array = read_binary_file(input_file_path)
    igualarlo = raw_array.reshape((3008,4224)) 
    plt.imshow(igualarlo)
    plt.show()
        
except FileNotFoundError:
    print(f"El archivo {input_file_path} no se encontró. Asegúrate de que la ruta y el nombre del archivo sean correctos.")
