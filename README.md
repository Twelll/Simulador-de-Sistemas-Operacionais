# Simulador de Sistemas Operacionais
# Simulador de Sistemas Operacionais
**Disciplina:** Engenharia de Computação — Semestre 2026  
**Linguagem:** C++17 com Qt (Widgets)

---

## Funcionalidades Implementadas

### Escalonamento de Processos
- **Round-Robin (RR)** — Quantum configurável via interface
- **SJF Preemptivo** — Menor tempo restante tem prioridade a cada unidade de tempo
- **Prioridade Preemptiva** — Menor número = maior prioridade; preempção a cada unidade

### Gerenciamento de Memória Virtual
- Paginação com tamanho de página fixo em 4 MB
- Políticas de substituição: **FIFO**, **LRU**, **Ótimo**
- Contagem de **Page Faults**

### Interface Gráfica
- Seleção de arquivo CSV de entrada
- Configuração de memória física e virtual
- Seleção de política de escalonamento e substituição
- Linha do tempo visual por processo
- Relatório detalhado com métricas

---

## Formato do CSV de Entrada

```
chegada,execucao,prioridade,memoria
0,6,2,16
1,4,1,8
2,8,3,32
```

**Campos (sem cabeçalho obrigatório):**
| Campo      | Descrição                        |
|------------|----------------------------------|
| chegada    | Tempo de chegada do processo     |
| execucao   | Tempo de CPU necessário (burst)  |
| prioridade | Prioridade (menor = maior prior) |
| memoria    | Memória necessária em MB         |

---

## Como Compilar

### Opção 1 — Qt Creator (recomendado)
1. Abra o Qt Creator
2. Clique em **Abrir Projeto** e selecione `SimuladorSO.pro` (ou `CMakeLists.txt`)
3. Configure o kit Qt (Qt 5.x ou Qt 6.x)
4. Pressione **Ctrl+R** para compilar e executar

### Opção 2 — Linha de comando com qmake
```bash
mkdir build && cd build
qmake ../SimuladorSO.pro
make -j4
./SimuladorSO
```

### Opção 3 — CMake
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j4
./SimuladorSO
```

---

## Estrutura do Projeto

```
Projeto/
├── headers/
│   ├── processo.h            # Estruturas de dados (Processo, EntradaLinhaDoTempo)
│   ├── leitorcsv.h           # Leitura e validação do CSV
│   ├── escalonador.h         # Interface do escalonador
│   ├── gerenciadorMemoria.h  # Gerenciamento de frames e substituição
│   ├── simulador.h           # Orquestração da simulação completa
│   └── widgetLinhaDoTempo.h  # Widget de renderização gráfica
│
├── sources/
│   ├── main.cpp
│   ├── janelaPrincipal.cpp   # Lógica da janela principal
│   ├── leitorcsv.cpp
│   ├── escalonador.cpp       # RR, SJF-P, Prioridade-P
│   ├── gerenciadorMemoria.cpp # FIFO, LRU, Ótimo
│   ├── simulador.cpp
│   └── widgetLinhaDoTempo.cpp
│
├── janelaPrincipal.ui        # Layout da interface (Qt Designer)
├── SimuladorSO.pro           # Arquivo de projeto qmake
├── CMakeLists.txt            # Alternativa com CMake
├── exemplo_processos.csv     # CSV de exemplo para testes
└── README.md
```

---

## Dependências
- Qt 5.12+ ou Qt 6.x (módulos: Core, Gui, Widgets)
- Compilador com suporte a C++17 (GCC 7+, Clang 5+, MSVC 2017+)
- Nenhuma biblioteca externa adicional

## Desenvolvedores

| [<img loading="lazy" src="https://avatars.githubusercontent.com/u/162636181?v=4" width=115><br><sub>Thiago Wesley</sub>](https://github.com/Twelll) |  [<img loading="lazy" src="https://avatars.githubusercontent.com/u/188608481?v=4" width=115><br><sub>Lucas Gabriel</sub>](https://github.com/Acsul) |  [<img loading="lazy" src="https://avatars.githubusercontent.com/u/189047179?v=4" width=115><br><sub>Vinicius Gonçalves</sub>](https://github.com/Vinicius333-arch) |
:---: | :---: | :---: |
