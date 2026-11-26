#ifndef SIMULADOR_H
#define SIMULADOR_H

#include <QList>
#include "processo.h"
#include "escalonador.h"
#include "gerenciadorMemoria.h"

struct ResultadoSimulacao {
    QList<EntradaLinhaDoTempo> linhaDoTempo;
    double tempoMedioResposta;
    double tempoMedioEspera;
    int totalPageFaults;
    bool sucesso;
    QString mensagemErro;
};

class Simulador {
public:
    Simulador();

    ResultadoSimulacao executar(
        const QList<Processo> &processos,
        PoliticaEscalonamento politicaEscalonamento,
        int quantum,
        int memoriaFisicaMB,
        int memoriaVirtualMB,
        PoliticaSubstituicao politicaSubstituicao
    );

private:
    static const int TAMANHO_PAGINA_MB = 4;

    QList<AcessoPagina> gerarSequenciaAcessos(
        const QList<Processo> &processos,
        const QList<EntradaLinhaDoTempo> &linhaDoTempo
    );

    void simularMemoria(
        const QList<Processo> &processos,
        const QList<EntradaLinhaDoTempo> &linhaDoTempo,
        int memoriaFisicaMB,
        PoliticaSubstituicao politica,
        int &totalPageFaults
    );
};

#endif // SIMULADOR_H
