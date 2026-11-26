#ifndef WIDGETLINHADOTEMPO_H
#define WIDGETLINHADOTEMPO_H

#include <QWidget>
#include <QList>
#include <QColor>
#include "processo.h"

class WidgetLinhaDoTempo : public QWidget {
    Q_OBJECT

public:
    explicit WidgetLinhaDoTempo(QWidget *parent = nullptr);

    void definirDados(const QList<EntradaLinhaDoTempo> &dados, int numeroProcessos);
    void limpar();

protected:
    void paintEvent(QPaintEvent *event) override;
    QSize sizeHint() const override;

private:
    QList<EntradaLinhaDoTempo> m_dados;
    int m_numeroProcessos;
    int m_tempoTotal;

    static const int ALTURA_LINHA = 30;
    static const int MARGEM_ESQUERDA = 60;
    static const int MARGEM_SUPERIOR = 30;
    static const int MARGEM_INFERIOR = 30;
    static const int PIXELS_POR_UNIDADE = 24;

    QColor corDoProcesso(int idProcesso) const;
};

#endif // WIDGETLINHADOTEMPO_H
