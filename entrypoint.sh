#!/bin/sh

if [ ! -f /app/devices.csv ]; then
    echo "Extraindo devices.zip..."
    unzip -o /app/devices.zip -d /app
    echo "devices.csv extraído."
else
    echo "devices.csv já existe, não será extraído."
fi

echo "Executando prepare_data.py para preparar os dados..."
python3 -m venv /app/venv > /dev/null 2>&1 && \
/app/venv/bin/pip install --upgrade pip --no-cache-dir > /dev/null 2>&1 && \
/app/venv/bin/pip install pandas --no-cache-dir > /dev/null 2>&1
/app/venv/bin/python /app/prepare_data.py

echo "Analisando dados..."
make
