import pandas as pd
import os

os.system('cls' if os.name == 'nt' else 'clear')

FILE_TO_PROCESS = 'devices.csv'
OUTPUT_FILE = 'relatorio_dispositivos.csv'

file_path = FILE_TO_PROCESS
data = pd.read_csv(file_path, sep='|')

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

# Cria coluna "ano-mes"
data['ano-mes'] = data['data'].dt.to_period('M').astype(str)

# Sensores a serem analisados
sensors = ['temperatura', 'umidade', 'luminosidade', 'ruido', 'eco2', 'etvoc']

# Lista para armazenar os resultados
result_rows = []

# Agrupa por dispositivo e ano-mês
grouped = data.groupby(['device', 'ano-mes'])

for (device, year_month), group in grouped:
    for sensor in sensors:
        max_val = group[sensor].max()
        min_val = group[sensor].min()
        mean_val = group[sensor].mean()
        
        result_rows.append({
            'device': device,
            'ano-mes': year_month,
            'sensor': sensor,
            'valor_maximo': round(max_val, 2),
            'valor_medio': round(mean_val, 2),
            'valor_minimo': round(min_val, 2),
        })

# Cria DataFrame de resultados
result_df = pd.DataFrame(result_rows)

# Salva resultados em CSV
output_file = OUTPUT_FILE
result_df.to_csv(output_file, sep=';', index=False)
print(f"Arquivo de saída gerado: {output_file}")