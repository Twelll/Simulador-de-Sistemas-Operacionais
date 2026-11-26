#include "widgetLinhaDoTempo.h"
#include <QPainter>
#include <QPen>
#include <QFont>
#include <QSet>

WidgetLinhaDoTempo::WidgetLinhaDoTempo(QWidget *parent)
    : QWidget(parent), m_numeroProcessos(0), m_tempoTotal(0)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setMinimumHeight(100);
}

void WidgetLinhaDoTempo::definirDados(const QList<EntradaLinhaDoTempo> &dados, int numeroProcessos) {
    m_dados = dados;
    m_numeroProcessos = numeroProcessos;

    m_tempoTotal = 0;
    for (const EntradaLinhaDoTempo &e : dados) {
        if (e.tempoFim > m_tempoTotal) m_tempoTotal = e.tempoFim;
    }

    int alturaTotal = MARGEM_SUPERIOR + (m_numeroProcessos * (ALTURA_LINHA + 4)) + MARGEM_INFERIOR + 20;
    setFixedHeight(qMax(alturaTotal, 100));
    update();
}

void WidgetLinhaDoTempo::limpar() {
    m_dados.clear();
    m_numeroProcessos = 0;
    m_tempoTotal = 0;
    setFixedHeight(100);
    update();
}

QColor WidgetLinhaDoTempo::corDoProcesso(int idProcesso) const {
    static const QColor cores[] = {
        QColor(70,  130, 200),
        QColor(60,  170,  90),
        QColor(210,  80,  70),
        QColor(200, 140,  40),
        QColor(140,  70, 180),
        QColor(50,  170, 160),
        QColor(220, 100, 160),
        QColor(100, 160,  60),
    };
    return cores[(idProcesso - 1) % 8];
}

void WidgetLinhaDoTempo::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    p.fillRect(rect(), QColor(245, 246, 248));

    if (m_dados.isEmpty()) {
        p.setPen(QColor(150, 150, 150));
        p.drawText(rect(), Qt::AlignCenter, "Nenhuma simulação executada.");
        return;
    }

    QFont fonteLabel("Monospace", 8);
    QFont fonteTick("Monospace", 7);
    p.setFont(fonteLabel);

    // Rótulos dos processos à esquerda
    QSet<int> processosMostrados;
    int alturaAtual = MARGEM_SUPERIOR;

    for (int pid = 1; pid <= m_numeroProcessos; pid++) {
        p.setPen(QColor(50, 50, 50));
        QString label = QString("P%1").arg(pid);
        p.drawText(QRect(0, alturaAtual, MARGEM_ESQUERDA - 5, ALTURA_LINHA),
                   Qt::AlignVCenter | Qt::AlignRight, label);
        alturaAtual += ALTURA_LINHA + 4;
    }

    // Barra de cada processo
    for (const EntradaLinhaDoTempo &entrada : m_dados) {
        int linhaPid = entrada.idProcesso - 1;
        int y = MARGEM_SUPERIOR + linhaPid * (ALTURA_LINHA + 4);
        int x = MARGEM_ESQUERDA + entrada.tempoInicio * PIXELS_POR_UNIDADE;
        int largura = (entrada.tempoFim - entrada.tempoInicio) * PIXELS_POR_UNIDADE;

        QColor cor = corDoProcesso(entrada.idProcesso);
        p.fillRect(x, y, largura, ALTURA_LINHA, cor);

        // Borda
        p.setPen(QPen(cor.darker(130), 1));
        p.drawRect(x, y, largura, ALTURA_LINHA);

        // Texto dentro da barra se couber
        if (largura > 18) {
            p.setPen(Qt::white);
            p.setFont(QFont("Monospace", 7, QFont::Bold));
            QString texto = QString("%1-%2").arg(entrada.tempoInicio).arg(entrada.tempoFim);
            p.drawText(QRect(x + 2, y, largura - 4, ALTURA_LINHA),
                       Qt::AlignVCenter | Qt::AlignHCenter, texto);
            p.setFont(fonteLabel);
        }
    }

    // Eixo do tempo
    int yEixo = MARGEM_SUPERIOR + m_numeroProcessos * (ALTURA_LINHA + 4) + 6;
    p.setPen(QPen(QColor(100, 100, 100), 1));
    p.drawLine(MARGEM_ESQUERDA, yEixo, MARGEM_ESQUERDA + m_tempoTotal * PIXELS_POR_UNIDADE, yEixo);

    // Marcações de tempo
    p.setFont(fonteTick);
    int passo = qMax(1, m_tempoTotal / 20);
    for (int t = 0; t <= m_tempoTotal; t += passo) {
        int x = MARGEM_ESQUERDA + t * PIXELS_POR_UNIDADE;
        p.setPen(QColor(100, 100, 100));
        p.drawLine(x, yEixo, x, yEixo + 4);
        p.drawText(QRect(x - 10, yEixo + 5, 20, 15), Qt::AlignHCenter, QString::number(t));
    }
}

QSize WidgetLinhaDoTempo::sizeHint() const {
    int largura = MARGEM_ESQUERDA + m_tempoTotal * PIXELS_POR_UNIDADE + 40;
    int altura = MARGEM_SUPERIOR + m_numeroProcessos * (ALTURA_LINHA + 4) + MARGEM_INFERIOR + 20;
    return QSize(qMax(largura, 400), qMax(altura, 100));
}
