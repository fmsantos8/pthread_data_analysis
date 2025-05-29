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
    gdb \
    unzip && \
    apt-get clean

WORKDIR /app

COPY . /app

RUN python3 -m venv /app/venv && \
    /app/venv/bin/pip install --upgrade pip && \
    /app/venv/bin/pip install pandas

COPY entrypoint.sh /app/entrypoint.sh
RUN chmod +x /app/entrypoint.sh

CMD ["/app/entrypoint.sh"]