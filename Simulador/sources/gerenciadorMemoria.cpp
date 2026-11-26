#include "gerenciadorMemoria.h"
#include <climits>

GerenciadorMemoria::GerenciadorMemoria(
    int memoriaFisicaMB, int memoriaVirtualMB, int tamanhoPaginaMB, PoliticaSubstituicao politica)
    : m_totalPageFaults(0), m_politica(politica)
{
    Q_UNUSED(memoriaVirtualMB);
    m_totalFrames = memoriaFisicaMB / tamanhoPaginaMB;
    if (m_totalFrames < 1) m_totalFrames = 1;
}

void GerenciadorMemoria::reiniciar() {
    m_totalPageFaults = 0;
    m_frames.clear();
    while (!m_filaFIFO.isEmpty()) m_filaFIFO.dequeue();
}

int GerenciadorMemoria::framesUsados() const {
    return m_frames.size();
}

bool GerenciadorMemoria::carregarPagina(
    int idProcesso, int pagina, int tempoAtual, const QList<AcessoPagina> &acessosFuturos)
{
    // Verifica se a página já está em memória
    for (int i = 0; i < m_frames.size(); i++) {
        if (m_frames[i].idProcesso == idProcesso && m_frames[i].pagina == pagina) {
            m_frames[i].ultimoAcesso = tempoAtual;
            return false; // Sem page fault
        }
    }

    // Page fault
    m_totalPageFaults++;

    EntradaFrame novoFrame;
    novoFrame.idProcesso = idProcesso;
    novoFrame.pagina = pagina;
    novoFrame.tempoCarregamento = tempoAtual;
    novoFrame.ultimoAcesso = tempoAtual;

    int frameIdx = encontrarFrameLivre();
    if (frameIdx >= 0) {
        if (frameIdx >= m_frames.size()) {
            m_frames.append(novoFrame);
            frameIdx = m_frames.size() - 1;
        } else {
            m_frames[frameIdx] = novoFrame;
        }
        m_filaFIFO.enqueue(frameIdx);
    } else {
        int alvo = selecionarFrameParaSubstituicao(tempoAtual, acessosFuturos);
        m_filaFIFO.removeAll(alvo);
        m_filaFIFO.enqueue(alvo);
        m_frames[alvo] = novoFrame;
    }

    return true; // Page fault ocorreu
}

int GerenciadorMemoria::encontrarFrameLivre() {
    if (m_frames.size() < m_totalFrames) {
        return m_frames.size(); // Próximo índice disponível
    }
    return -1;
}

int GerenciadorMemoria::selecionarFrameParaSubstituicao(
    int tempoAtual, const QList<AcessoPagina> &acessosFuturos)
{
    switch (m_politica) {
    case PoliticaSubstituicao::FIFO:
        return substituirFIFO();
    case PoliticaSubstituicao::LRU:
        return substituirLRU();
    case PoliticaSubstituicao::Otimo:
        return substituirOtimo(acessosFuturos);
    }
    return 0;
}

int GerenciadorMemoria::substituirFIFO() {
    if (!m_filaFIFO.isEmpty()) {
        return m_filaFIFO.head(); // O mais antigo será removido
    }
    return 0;
}

int GerenciadorMemoria::substituirLRU() {
    int maisAntigoIdx = 0;
    int menorTempo = INT_MAX;
    for (int i = 0; i < m_frames.size(); i++) {
        if (m_frames[i].ultimoAcesso < menorTempo) {
            menorTempo = m_frames[i].ultimoAcesso;
            maisAntigoIdx = i;
        }
    }
    return maisAntigoIdx;
}

int GerenciadorMemoria::substituirOtimo(const QList<AcessoPagina> &acessosFuturos) {
    int melhorIdx = 0;
    int maiorDistancia = -1;

    for (int i = 0; i < m_frames.size(); i++) {
        int idProc = m_frames[i].idProcesso;
        int pag = m_frames[i].pagina;

        // Procura o próximo acesso futuro a esta página
        int proximoAcesso = INT_MAX;
        for (int j = 0; j < acessosFuturos.size(); j++) {
            if (acessosFuturos[j].idProcesso == idProc && acessosFuturos[j].pagina == pag) {
                proximoAcesso = j;
                break;
            }
        }

        if (proximoAcesso > maiorDistancia) {
            maiorDistancia = proximoAcesso;
            melhorIdx = i;
        }
    }

    return melhorIdx;
}
