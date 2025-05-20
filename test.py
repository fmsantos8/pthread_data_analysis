import pandas as pd
import os

os.system('clear')

file_path = 'sample_data.csv'
data = pd.read_csv(file_path, sep='|')

device = "sirrosteste_UCS_AMV-08"
device_count = data[data['device'] == device].shape[0]
total_number_of_rows = data.shape[0]
print(f"Total number of rows: {total_number_of_rows}")

print(f"Number of rows with device '{device}': {device_count}")

if data['device'].isnull().any():
    empty_count = data['device'].isnull().sum()
    print(f"Number of empty values in 'device' column: {empty_count}")