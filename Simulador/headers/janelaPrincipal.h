#ifndef JANELAPRINCIPAL_H
#define JANELAPRINCIPAL_H

#include <QMainWindow>
#include <QList>
#include "processo.h"
#include "simulador.h"

QT_BEGIN_NAMESPACE
namespace Ui { class JanelaPrincipal; }
QT_END_NAMESPACE

class JanelaPrincipal : public QMainWindow {
    Q_OBJECT

public:
    explicit JanelaPrincipal(QWidget *parent = nullptr);
    ~JanelaPrincipal();

private slots:
    void selecionarArquivoCSV();
    void executarSimulacao();
    void politicaEscalonamentoAlterada(int indice);
    void limparResultados();

private:
    Ui::JanelaPrincipal *ui;
    QList<Processo> m_processos;

    void atualizarTabelaProcessos();
    void exibirResultados(const ResultadoSimulacao &resultado);
    void configurarInterfaceInicial();
    bool validarEntradas();
};

#endif // JANELAPRINCIPAL_H
