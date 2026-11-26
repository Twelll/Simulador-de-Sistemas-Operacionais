#include "simulador.h"
#include <QMap>
#include <cmath>

Simulador::Simulador() {}

ResultadoSimulacao Simulador::executar(
    const QList<Processo> &processos,
    PoliticaEscalonamento politicaEscalonamento,
    int quantum,
    int memoriaFisicaMB,
    int memoriaVirtualMB,
    PoliticaSubstituicao politicaSubstituicao)
{
    ResultadoSimulacao resultado;
    resultado.sucesso = false;

    if (processos.isEmpty()) {
        resultado.mensagemErro = "Nenhum processo para simular.";
        return resultado;
    }

    // Valida memória
    int memoriaMaxProcesso = 0;
    for (const Processo &p : processos) {
        memoriaMaxProcesso = qMax(memoriaMaxProcesso, p.memoriaNecessaria);
    }
    if (memoriaFisicaMB < TAMANHO_PAGINA_MB) {
        resultado.mensagemErro = "Memória física insuficiente (mínimo 4 MB).";
        return resultado;
    }

    // Escalonamento de processos
    resultado.linhaDoTempo = Escalonador::executar(
        processos,
        politicaEscalonamento,
        quantum,
        resultado.tempoMedioResposta,
        resultado.tempoMedioEspera
    );

    // Simulação de memória virtual
    simularMemoria(processos, resultado.linhaDoTempo, memoriaFisicaMB,
                   politicaSubstituicao, resultado.totalPageFaults);

    resultado.sucesso = true;
    return resultado;
}

QList<AcessoPagina> Simulador::gerarSequenciaAcessos(
    const QList<Processo> &processos,
    const QList<EntradaLinhaDoTempo> &linhaDoTempo)
{
    QList<AcessoPagina> acessos;

    // Mapa id -> processo para lookup rápido
    QMap<int, Processo> mapa;
    for (const Processo &p : processos) {
        mapa[p.id] = p;
    }

    for (const EntradaLinhaDoTempo &entrada : linhaDoTempo) {
        if (!mapa.contains(entrada.idProcesso)) continue;
        const Processo &p = mapa[entrada.idProcesso];

        int numeroPaginas = qMax(1, (int)std::ceil((double)p.memoriaNecessaria / TAMANHO_PAGINA_MB));

        // Simula acesso às páginas do processo durante cada fatia
        for (int t = entrada.tempoInicio; t < entrada.tempoFim; t++) {
            // Acessa páginas ciclicamente durante a execução
            int paginaAcessada = (t - entrada.tempoInicio) % numeroPaginas;
            AcessoPagina acesso;
            acesso.idProcesso = entrada.idProcesso;
            acesso.pagina = paginaAcessada;
            acessos.append(acesso);
        }
    }

    return acessos;
}

void Simulador::simularMemoria(
    const QList<Processo> &processos,
    const QList<EntradaLinhaDoTempo> &linhaDoTempo,
    int memoriaFisicaMB,
    PoliticaSubstituicao politica,
    int &totalPageFaults)
{
    QList<AcessoPagina> sequenciaAcessos = gerarSequenciaAcessos(processos, linhaDoTempo);

    GerenciadorMemoria gerenciador(memoriaFisicaMB, memoriaFisicaMB * 4, TAMANHO_PAGINA_MB, politica);
    gerenciador.reiniciar();

    for (int i = 0; i < sequenciaAcessos.size(); i++) {
        const AcessoPagina &acesso = sequenciaAcessos[i];
        QList<AcessoPagina> futuros = sequenciaAcessos.mid(i + 1);
        gerenciador.carregarPagina(acesso.idProcesso, acesso.pagina, i, futuros);
    }

    totalPageFaults = gerenciador.totalPageFaults();
}
