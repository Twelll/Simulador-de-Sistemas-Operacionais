#ifndef PROCESSO_H
#define PROCESSO_H

#include <QString>
#include <QList>

enum class EstadoProcesso {
    Pronto,
    Executando,
    Bloqueado,
    Finalizado
};

struct Processo {
    int id;
    int tempoChegada;
    int tempoExecucao;
    int prioridade;
    int memoriaNecessaria; // em MB

    // Campos calculados durante simulação
    int tempoCpuRestante;
    int tempoInicio;
    int tempoFinalizacao;
    int tempoEspera;
    int tempoResposta;
    EstadoProcesso estado;

    // Paginação
    int numeroPaginas;
    QList<int> tabelaPaginas; // índice = página virtual, valor = frame físico (-1 se não mapeado)

    Processo()
        : id(0), tempoChegada(0), tempoExecucao(0), prioridade(0), memoriaNecessaria(0),
          tempoCpuRestante(0), tempoInicio(-1), tempoFinalizacao(-1),
          tempoEspera(0), tempoResposta(-1), estado(EstadoProcesso::Pronto),
          numeroPaginas(0) {}

    Processo(int id, int chegada, int execucao, int prior, int memoria)
        : id(id), tempoChegada(chegada), tempoExecucao(execucao), prioridade(prior),
          memoriaNecessaria(memoria), tempoCpuRestante(execucao), tempoInicio(-1),
          tempoFinalizacao(-1), tempoEspera(0), tempoResposta(-1),
          estado(EstadoProcesso::Pronto), numeroPaginas(0) {}
};

struct EntradaLinhaDoTempo {
    int idProcesso;
    int tempoInicio;
    int tempoFim;
};

#endif // PROCESSO_H
