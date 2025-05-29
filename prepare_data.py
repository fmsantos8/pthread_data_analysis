import pandas as pd
import os

FILE_TO_PROCESS = 'devices.csv'
OUTPUT_FILE_CLEAN = FILE_TO_PROCESS.replace('.csv', '_clean.csv')
OUTPUT_FILE_EXPECTED = FILE_TO_PROCESS.replace('.csv', '_expected.csv')

if os.path.exists(OUTPUT_FILE_CLEAN):
    if os.path.exists(OUTPUT_FILE_EXPECTED):
        print(f"Os arquivos {OUTPUT_FILE_CLEAN} e {OUTPUT_FILE_EXPECTED} já existem. Abortando processo de limpeza dos dados.")
        exit(1)

file_path = FILE_TO_PROCESS
data = pd.read_csv(file_path, sep='|')

if data.empty:
    print("O arquivo está vazio.")
    exit(1)

# remove OUTPUT_FILE_CLEAN if it exists
if os.path.exists(OUTPUT_FILE_CLEAN):
    os.remove(OUTPUT_FILE_CLEAN)

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
output_file_clean = OUTPUT_FILE_CLEAN
data.to_csv(output_file_clean, sep='|', index=False)
print(f"Arquivo de saída gerado: {output_file_clean}")

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
            'valor_maximo': f"{max_val:.2f}",
            'valor_medio': f"{mean_val:.2f}",
            'valor_minimo': f"{min_val:.2f}",
        })

# Cria DataFrame de resultados
result_df = pd.DataFrame(result_rows)

# Salva resultados em CSV
output_file_clean = OUTPUT_FILE_EXPECTED
result_df.to_csv(output_file_clean, sep=';', index=False)
print(f"Arquivo de saída gerado: {output_file_clean}")