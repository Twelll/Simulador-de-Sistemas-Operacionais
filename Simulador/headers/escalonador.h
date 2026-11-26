#ifndef ESCALONADOR_H
#define ESCALONADOR_H

#include <QList>
#include "processo.h"

enum class PoliticaEscalonamento {
    RoundRobin,
    SJFPreemptivo,
    PrioridadePreemptiva
};

class Escalonador {
public:
    static QList<EntradaLinhaDoTempo> executar(
        QList<Processo> processos,
        PoliticaEscalonamento politica,
        int quantum,
        double &tempoMedioResposta,
        double &tempoMedioEspera
    );

private:
    static QList<EntradaLinhaDoTempo> executarRoundRobin(QList<Processo> &processos, int quantum, double &tmr, double &tme);
    static QList<EntradaLinhaDoTempo> executarSJF(QList<Processo> &processos, double &tmr, double &tme);
    static QList<EntradaLinhaDoTempo> executarPrioridade(QList<Processo> &processos, double &tmr, double &tme);
    static void calcularMetricas(const QList<Processo> &processos, double &tmr, double &tme);
};

#endif // ESCALONADOR_H
