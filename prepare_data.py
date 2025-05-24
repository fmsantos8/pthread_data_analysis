import pandas as pd
import os

os.system('cls' if os.name == 'nt' else 'clear')

FILE_TO_PROCESS = 'sample_data.csv'
OUTPUT_FILE = FILE_TO_PROCESS.replace('.csv', '_clean.csv')

file_path = FILE_TO_PROCESS
data = pd.read_csv(file_path, sep='|')

if data.empty:
    print("O arquivo está vazio.")
    exit(1)

# remove OUTPUT_FILE if it exists
if os.path.exists(OUTPUT_FILE):
    os.remove(OUTPUT_FILE)

print(f"Total number of rows: {data.shape[0]}") # Total number of rows: 6167875

# Remove colunas desnecessárias
data = data.drop(columns=['id', 'contagem', 'latitude', 'longitude'])

# Remove linhas onde 'device' ou 'data' são nulos
data = data.dropna(subset=['device', 'data'])
print(f"Rows after removing 'device' and 'data' null rows: {data.shape[0]}") # Rows after removing 'device' and 'data' null rows: 6166977

# Converte a coluna 'data' para datetime
data['data'] = pd.to_datetime(data['data'], format='mixed')

# Lista de colunas de sensores
sensor_cols = ['temperatura', 'umidade', 'luminosidade', 'ruido', 'eco2', 'etvoc']

# Remove linhas onde todos os sensores estão nulos
data = data.dropna(subset=sensor_cols, how='all') 
print(f"Rows after removing all sensor null rows: {data.shape[0]}") # Rows after removing all sensor null rows: 5949052

# Filtra registros a partir de março de 2024
data = data[data['data'] >= '2024-03-01']
print(f"Total number of rows with date >= '2024-03-01': {data.shape[0]}") # Total number of rows with data >= '2024-03-01': 4175008

# Ordena as linhas por 'device' e 'data'
data = data.sort_values(by=['device', 'data'])

# Salva resultados em CSV
output_file = OUTPUT_FILE
data.to_csv(output_file, sep='|', index=False)
print(f"Arquivo de saída gerado: {output_file}")