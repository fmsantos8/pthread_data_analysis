#!/bin/sh

if [ ! -f /app/devices.csv ]; then
    echo "Extraindo devices.zip..."
    unzip -o /app/devices.zip -d /app
    echo "devices.csv extraído."
else
    echo "devices.csv já existe, não será extraído."
fi

echo "Executando prepare_data.py para preparar os dados..."
/app/venv/bin/python /app/prepare_data.py

echo "Analisando dados..."
make
