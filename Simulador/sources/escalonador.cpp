#include "escalonador.h"
#include <algorithm>
#include <climits>

QList<EntradaLinhaDoTempo> Escalonador::executar(
    QList<Processo> processos,
    PoliticaEscalonamento politica,
    int quantum,
    double &tempoMedioResposta,
    double &tempoMedioEspera)
{
    switch (politica) {
    case PoliticaEscalonamento::RoundRobin:
        return executarRoundRobin(processos, quantum, tempoMedioResposta, tempoMedioEspera);
    case PoliticaEscalonamento::SJFPreemptivo:
        return executarSJF(processos, tempoMedioResposta, tempoMedioEspera);
    case PoliticaEscalonamento::PrioridadePreemptiva:
        return executarPrioridade(processos, tempoMedioResposta, tempoMedioEspera);
    }
    return {};
}

QList<EntradaLinhaDoTempo> Escalonador::executarRoundRobin(
    QList<Processo> &processos, int quantum, double &tmr, double &tme)
{
    QList<EntradaLinhaDoTempo> linha;
    QList<int> fila;
    int tempo = 0;
    int concluidos = 0;
    int n = processos.size();

    std::sort(processos.begin(), processos.end(),
              [](const Processo &a, const Processo &b){ return a.tempoChegada < b.tempoChegada; });

    QList<bool> naFila;
    for (int i = 0; i < n; ++i) {
        naFila.append(false); // Adiciona 'false' n vezes
    }

    int proximoAChegar = 0;

    while (concluidos < n) {
        // Adiciona processos que chegaram até o tempo atual
        while (proximoAChegar < n && processos[proximoAChegar].tempoChegada <= tempo) {
            fila.append(proximoAChegar);
            naFila[proximoAChegar] = true;
            proximoAChegar++;
        }

        if (fila.isEmpty()) {
            tempo = processos[proximoAChegar].tempoChegada;
            continue;
        }

        int idx = fila.takeFirst();
        naFila[idx] = false;
        Processo &p = processos[idx];

        if (p.tempoResposta == -1) {
            p.tempoResposta = tempo - p.tempoChegada;
            p.tempoInicio = tempo;
        }

        int fatia = qMin(quantum, p.tempoCpuRestante);

        EntradaLinhaDoTempo entrada;
        entrada.idProcesso = p.id;
        entrada.tempoInicio = tempo;
        entrada.tempoFim = tempo + fatia;
        linha.append(entrada);

        tempo += fatia;
        p.tempoCpuRestante -= fatia;

        // Adiciona processos que chegaram durante a execução desta fatia
        while (proximoAChegar < n && processos[proximoAChegar].tempoChegada <= tempo) {
            fila.append(proximoAChegar);
            naFila[proximoAChegar] = true;
            proximoAChegar++;
        }

        if (p.tempoCpuRestante > 0) {
            fila.append(idx);
            naFila[idx] = true;
        } else {
            p.tempoFinalizacao = tempo;
            p.tempoEspera = p.tempoFinalizacao - p.tempoChegada - p.tempoExecucao;
            concluidos++;
        }
    }

    calcularMetricas(processos, tmr, tme);
    return linha;
}

QList<EntradaLinhaDoTempo> Escalonador::executarSJF(
    QList<Processo> &processos, double &tmr, double &tme)
{
    QList<EntradaLinhaDoTempo> linha;
    int tempo = 0;
    int concluidos = 0;
    int n = processos.size();
    int processoAtual = -1;

    while (concluidos < n) {
        // Escolhe o processo com menor tempo restante entre os prontos
        int melhor = -1;
        for (int i = 0; i < n; i++) {
            if (processos[i].estado == EstadoProcesso::Finalizado) continue;
            if (processos[i].tempoChegada > tempo) continue;
            if (melhor == -1 || processos[i].tempoCpuRestante < processos[melhor].tempoCpuRestante)
                melhor = i;
        }

        if (melhor == -1) {
            // Avança para o próximo processo que chegará
            int proximoChegada = INT_MAX;
            for (int i = 0; i < n; i++) {
                if (processos[i].estado != EstadoProcesso::Finalizado)
                    proximoChegada = qMin(proximoChegada, processos[i].tempoChegada);
            }
            tempo = proximoChegada;
            continue;
        }

        Processo &p = processos[melhor];

        if (p.tempoResposta == -1) {
            p.tempoResposta = tempo - p.tempoChegada;
            p.tempoInicio = tempo;
        }

        // Executa por 1 unidade (preemptivo: verifica a cada unidade)
        EntradaLinhaDoTempo entrada;
        entrada.idProcesso = p.id;
        entrada.tempoInicio = tempo;
        entrada.tempoFim = tempo + 1;

        // Consolida com entrada anterior se mesmo processo
        if (!linha.isEmpty() && linha.last().idProcesso == p.id && linha.last().tempoFim == tempo) {
            linha.last().tempoFim = tempo + 1;
        } else {
            linha.append(entrada);
        }

        tempo++;
        p.tempoCpuRestante--;

        if (p.tempoCpuRestante == 0) {
            p.tempoFinalizacao = tempo;
            p.tempoEspera = p.tempoFinalizacao - p.tempoChegada - p.tempoExecucao;
            p.estado = EstadoProcesso::Finalizado;
            concluidos++;
        }
    }

    calcularMetricas(processos, tmr, tme);
    return linha;
}

QList<EntradaLinhaDoTempo> Escalonador::executarPrioridade(
    QList<Processo> &processos, double &tmr, double &tme)
{
    QList<EntradaLinhaDoTempo> linha;
    int tempo = 0;
    int concluidos = 0;
    int n = processos.size();

    while (concluidos < n) {
        // Escolhe o processo com maior prioridade (menor número = maior prioridade)
        int melhor = -1;
        for (int i = 0; i < n; i++) {
            if (processos[i].estado == EstadoProcesso::Finalizado) continue;
            if (processos[i].tempoChegada > tempo) continue;
            if (melhor == -1 || processos[i].prioridade < processos[melhor].prioridade)
                melhor = i;
        }

        if (melhor == -1) {
            int proximoChegada = INT_MAX;
            for (int i = 0; i < n; i++) {
                if (processos[i].estado != EstadoProcesso::Finalizado)
                    proximoChegada = qMin(proximoChegada, processos[i].tempoChegada);
            }
            tempo = proximoChegada;
            continue;
        }

        Processo &p = processos[melhor];

        if (p.tempoResposta == -1) {
            p.tempoResposta = tempo - p.tempoChegada;
            p.tempoInicio = tempo;
        }

        EntradaLinhaDoTempo entrada;
        entrada.idProcesso = p.id;
        entrada.tempoInicio = tempo;
        entrada.tempoFim = tempo + 1;

        if (!linha.isEmpty() && linha.last().idProcesso == p.id && linha.last().tempoFim == tempo) {
            linha.last().tempoFim = tempo + 1;
        } else {
            linha.append(entrada);
        }

        tempo++;
        p.tempoCpuRestante--;

        if (p.tempoCpuRestante == 0) {
            p.tempoFinalizacao = tempo;
            p.tempoEspera = p.tempoFinalizacao - p.tempoChegada - p.tempoExecucao;
            p.estado = EstadoProcesso::Finalizado;
            concluidos++;
        }
    }

    calcularMetricas(processos, tmr, tme);
    return linha;
}

void Escalonador::calcularMetricas(const QList<Processo> &processos, double &tmr, double &tme) {
    double somaResposta = 0, somaEspera = 0;
    int n = processos.size();
    for (const Processo &p : processos) {
        somaResposta += p.tempoResposta;
        somaEspera   += p.tempoEspera;
    }
    tmr = (n > 0) ? somaResposta / n : 0.0;
    tme = (n > 0) ? somaEspera   / n : 0.0;
}
