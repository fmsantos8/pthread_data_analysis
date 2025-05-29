# Análise de Dados de Sensoriamento com Pthreads

Este projeto demonstra o uso de threads POSIX (`pthreads`) para análise de dados de sensoriamento.
## Pré-requisitos

O projeto inteiro é executado dentro de um container Docker, portanto, você precisa ter:

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
docker run --rm -v ${PWD}:/app pthread_data_analysisí
```

### 3. Execução iterativa (opcional)

Se desejar acessar o container de forma interativa, use o comando abaixo:

```bash
docker run --rm -it -v ${PWD}:/app pthread_data_analysis /bin/bash
```

---

## Estrutura do Projeto

Dentro do container, você pode executar o `makefile` manualmente para compilar e rodar o projeto.

```plaintext
Dockerfile            # Configuração do ambiente Docker
Makefile              # Script de automação para compilação
src/                  # Código-fonte do projeto
    classes/          # Classes de dados e estruturas
        device/       # Guarda todas as medições e status mensais
        linked_list/  # Classe de linked list
        measure/      # Classe responsável por mapear as medições
        stats/        # Classe responsável pelos cálculos estatísticos
    libs/             # Bibliotecas auxiliares
        csv/          # Biblioteca para manipulação de arquivos CSV
        device_mapper # Biblioteca que converte dados CSV em objetos Device e vice versa
    main.c            # Arquivo principal com lógica de threads
.clang-format         # Configuração de formatação de código
devices.csv           # Arquivo CSV com dados de dispositivos
devices_clean.csv     # Arquivo CSV com dados de dispositivos limpos
devices_expected.csv  # Arquivo CSV com dados esperados para comparação gerados em Python
devices_output.csv    # Arquivo CSV com dados de saída após processamento gerados em C
prepare_data.py       # Script Python para preparar os dados de entrada
```

## Funcionamento do Projeto

### TODO: DESCREVER UM POUCO MELHOR O QUE É A THREAD PRINCIPAL, THREAD DE PROCESSAMENTO (COLOCAR NOME DA TAREFA), EXPLICAR SOBRE O PREPARE_DATA.PY.
### TODO: COLOCAR UM FLUXOGRAMA DO FUNCIONAMENTO DO PROJETO.

### Como o CSV é carregado para o programa?

O arquivo CSV é carregado através da biblioteca [lib_csv](src/libs/csv/lib_csv.h), que lê o arquivo `.csv` e converte para uma estrutura de strings. Em seguida, a biblioteca [device_mapper](src/libs/device_mapper/device_mapper.h) converte essas strings em objetos da classe `device_t`.

### Como é feita a distribuição de tarefas entre as threads?

A conversão das strings lidas do CSV em objetos `device_t` é feita na thread principal, antes do início das threads de processamento. Após a conversão, a thread principal inicia as threads de processamento, que consomem os objetos `device_t` da lista encadeada (`linked_list_t`) até que todos os objetos sejam processados. O número de threads é determinado pelo número de núcleos disponíveis no sistema.

### Como é feita a análise estatística?

A análise estatística é realizada somente nas threads de processamento, chamando a função `cls_device_process_readings` da classe `device_t`. Essa função percorre todas as medições (`measure_t`) existentes no objeto `device_t` e agrupa as medições por mês em uma lista encadeada de `stats_t`, que é o objeto responsável por calcular e armazenar as estatísticas. 

Os cálculos de mínimo, máximo e média são realizados ao adicionar uma novos valores à um `stats_t`, dentro da função `cls_stats_add_readings`.

Abaixo está um exemplo de como é o funcionamento do código descrito acima:

```c

    // ...

    node_t *current = cls_linked_list_get(device->measures, 0);
    while (current != NULL) {
        measure_t *measure = (measure_t *)cls_linked_list_get_data(current);
        assert(measure != NULL);

        date_t *date = cls_measure_get_date(measure); // Obtém a data da medição (ano e mês)
        stats_t *stats = get_stats_by_date(device->stats, date); // Verifica se já existe um stats na lista para essa data
        if (stats == NULL) {
            stats = cls_stats_init(date);
            assert(stats != NULL);
            cls_linked_list_add(device->stats, stats); // Caso não exista, cria um novo stats e adiciona à lista
        }

        sensor_readings_t *readings = cls_measure_get_readings(measure); // Obtém as leituras do sensor da medição
        cls_stats_add_readings(stats, readings); // Adiciona as leituras ao stats, que irá calcular os valores de mínimo, máximo e média
        current = cls_linked_list_get_next(current);
    }

    // cls_stats_add_readings

    for (int i = 0; i < SENSOR_MAX; i++) { // Percorre todos os tipos de sensores
        float reading = readings->values[i]; 
        if (reading == FLT_MAX) {
            continue; // Ignora leituras inválidas
        }

        stats->average_sum[i] += reading; // Soma as leituras para calcular a média
        stats->average_count[i]++; 

        if (reading < stats->min_value[i]) { 
            stats->min_value[i] = reading; // Atualiza o valor mínimo se a leitura for menor
        }
        if (reading > stats->max_value[i]) {
            stats->max_value[i] = reading; // Atualiza o valor máximo se a leitura for maior
        }
    }
``` 

### Como as threads geram o arquivo de saída?

As threads não geram o arquivo de saída diretamente, elas fazem os cálculos estatísticos e após o processamento, a thread principal coleta os resultados das threads de processamento, converte os objetos `device_t` em uma estrutura `csv_data_t` utilizando a função `lib_device_mapper_to_csv` da biblioteca [device_mapper](src/libs/device_mapper/lib_device_mapper.h) e escreve os dados no arquivo `devices_output.csv` usando a biblioteca [lib_csv](src/libs/csv/lib_csv.h).

### As threads criadas são executadas em modo usuário ou núcleo?

As threads criadas usam a biblioteca POSIX `pthreads`, então elas rodam em modo usuário.

### Possíveis problemas/melhorias

Atualmente as threads de processamento não escrevem o arquivo de saída diretamente, o que poderia ser uma melhoria para evitar que a thread principal precise coletar todos os resultados antes de escrever no arquivo. Isso poderia ser implementado utilizando uma fila compartilhada entre as threads de processamento e a thread principal, onde cada thread escreveria seu resultado assim que terminasse o processamento de um objeto `device_t`.