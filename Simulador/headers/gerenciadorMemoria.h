#ifndef GERENCIADORMEMORIA_H
#define GERENCIADORMEMORIA_H

#include <QList>
#include <QQueue>
#include "processo.h"

enum class PoliticaSubstituicao {
    FIFO,
    LRU,
    Otimo
};

struct AcessoPagina {
    int idProcesso;
    int pagina;
};

class GerenciadorMemoria {
public:
    GerenciadorMemoria(int memoriaFisicaMB, int memoriaVirtualMB, int tamanhoPaginaMB, PoliticaSubstituicao politica);

    void reiniciar();
    bool carregarPagina(int idProcesso, int pagina, int tempoAtual, const QList<AcessoPagina> &acessosFuturos);

    int totalPageFaults() const { return m_totalPageFaults; }
    int framesUsados() const;
    int totalFrames() const { return m_totalFrames; }

    struct EntradaFrame {
        int idProcesso;
        int pagina;
        int tempoCarregamento;
        int ultimoAcesso;
    };

    QList<EntradaFrame> estadoFrames() const { return m_frames; }

private:
    int m_totalFrames;
    int m_totalPageFaults;
    PoliticaSubstituicao m_politica;

    QList<EntradaFrame> m_frames;
    QQueue<int> m_filaFIFO;

    int encontrarFrameLivre();
    int selecionarFrameParaSubstituicao(int tempoAtual, const QList<AcessoPagina> &acessosFuturos);
    int substituirFIFO();
    int substituirLRU();
    int substituirOtimo(const QList<AcessoPagina> &acessosFuturos);
};

#endif // GERENCIADORMEMORIA_H
