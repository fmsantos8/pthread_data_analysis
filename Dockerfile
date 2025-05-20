FROM ubuntu:latest

ENV DEBIAN_FRONTEND=noninteractive
ENV TERM xterm

RUN apt-get update && \
    apt-get install -y build-essential make && \
    apt-get clean

WORKDIR /app

CMD make