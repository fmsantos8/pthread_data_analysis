FROM ubuntu:latest

ENV DEBIAN_FRONTEND=noninteractive
ENV TERM xterm

# Install required packages including python3-venv
RUN apt-get update && \
    apt-get install -y \
    build-essential \
    make \
    python3 \
    python3-pip \
    python3-venv && \
    gdb \
    apt-get clean

WORKDIR /app

# Create virtual environment and install pandas inside it
RUN python3 -m venv /app/venv && \
    /app/venv/bin/pip install --upgrade pip && \
    /app/venv/bin/pip install pandas 

# Default command
CMD make