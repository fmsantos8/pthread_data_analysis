# Análise de Dados de Sensoriamento com Pthreads

Este projeto demonstra o uso de threads POSIX (`pthreads`) para análise de dados de sensoriamento.
## Pré-requisitos

- [Docker Desktop](https://www.docker.com/products/docker-desktop) instalado e em execução na sua máquina.

## Como compilar e executar o projeto

### 1. Construir a imagem Docker

Execute o comando abaixo para construir a imagem Docker:

```bash
docker build -t pthread_data_analysis .
```

### 2. Executar o container

#### Para Linux:

```bash
docker run --rm -v $(pwd):/app pthread_data_analysis
```

#### Para Windows (PowerShell):

```powershell
docker run --rm -v ${PWD}:/app pthread_data_analysis
```

### 3. Execução iterativa (opcional)

Se desejar acessar o container de forma interativa, use o comando abaixo:

```bash
docker run --rm -it -v ${PWD}:/app pthread_data_analysis /bin/bash
```

Dentro do container, você pode executar o `make` manualmente para compilar e rodar o projeto.

## Estrutura do Projeto

```plaintext
Dockerfile          # Configuração do ambiente Docker
Makefile            # Script de automação para compilação
src/                # Código-fonte do projeto
    main.c          # Arquivo principal com lógica de threads
    example.c       # TODO: Documentação dos demais arquivos
    example.h       # TODO: Documentação dos demais arquivos
    ...
build/              # Diretório de saída para arquivos compilados
```
---