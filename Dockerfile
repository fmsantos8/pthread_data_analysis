FROM ubuntu:latest

ENV DEBIAN_FRONTEND=noninteractive
ENV TERM=xterm

RUN apt-get update && \
    apt-get install -y \
    build-essential \
    make \
    python3 \
    python3-pip \
    python3-venv \
    python3-pandas \
    gdb \
    unzip && \
    apt-get clean

WORKDIR /app

COPY . /app

CMD unzip -o /app/devices.zip -d /app && \
    python3 /app/prepare_data.py && \
    make