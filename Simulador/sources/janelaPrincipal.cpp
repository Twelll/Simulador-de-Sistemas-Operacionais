#include "janelaPrincipal.h"
#include "ui_janelaPrincipal.h"
#include "leitorcsv.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QHeaderView>

JanelaPrincipal::JanelaPrincipal(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::JanelaPrincipal)
{
    ui->setupUi(this);
    configurarInterfaceInicial();

    connect(ui->botaoSelecionarCSV,    &QPushButton::clicked, this, &JanelaPrincipal::selecionarArquivoCSV);
    connect(ui->botaoExecutar,         &QPushButton::clicked, this, &JanelaPrincipal::executarSimulacao);
    connect(ui->botaoLimpar,           &QPushButton::clicked, this, &JanelaPrincipal::limparResultados);
    connect(ui->comboEscalonamento,    QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &JanelaPrincipal::politicaEscalonamentoAlterada);
}

JanelaPrincipal::~JanelaPrincipal() {
    delete ui;
}

void JanelaPrincipal::configurarInterfaceInicial() {
    ui->spinQuantum->setEnabled(true);
    ui->botaoExecutar->setEnabled(false);
    ui->labelStatusCSV->setText("Nenhum arquivo carregado.");
    ui->labelStatusCSV->setStyleSheet("color: #888;");


    ui->grupoResultados->setWindowFlags(Qt::Window);
    ui->grupoResultados->setWindowTitle("Resultados da Simulação"); // Título do pop-up
    ui->grupoResultados->resize(950, 700); // Tamanho confortável 

    // Como os resultados saíram da tela principal, redimensionamos ela para ficar menor
    this->resize(750, 500);

    // Distribui as colunas igualmente
    ui->tabelaProcessos->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tabelaProcessos->verticalHeader()->setVisible(false);
}

void JanelaPrincipal::selecionarArquivoCSV() {
    QString caminho = QFileDialog::getOpenFileName(
        this, "Selecionar arquivo CSV", QString(), "Arquivos CSV (*.csv);;Todos os arquivos (*)");

    if (caminho.isEmpty()) return;

    QString erro;
    QList<Processo> processos = LeitorCSV::carregar(caminho, erro);

    if (!erro.isEmpty()) {
        QMessageBox::warning(this, "Erro ao carregar CSV", erro);
        ui->labelStatusCSV->setText("Erro ao carregar arquivo.");
        ui->labelStatusCSV->setStyleSheet("color: #c0392b;");
        ui->botaoExecutar->setEnabled(false);
        return;
    }

    m_processos = processos;
    ui->labelStatusCSV->setText(QString("Arquivo carregado: %1 processos encontrados.")
                                    .arg(m_processos.size()));
    ui->labelStatusCSV->setStyleSheet("color: #27ae60;");
    ui->botaoExecutar->setEnabled(true);
    atualizarTabelaProcessos();
}

void JanelaPrincipal::atualizarTabelaProcessos() {
    QTableWidget *tabela = ui->tabelaProcessos;
    tabela->clearContents();
    tabela->setRowCount(m_processos.size());

    for (int i = 0; i < m_processos.size(); i++) {
        const Processo &p = m_processos[i];
        tabela->setItem(i, 0, new QTableWidgetItem(QString("P%1").arg(p.id)));
        tabela->setItem(i, 1, new QTableWidgetItem(QString::number(p.tempoChegada)));
        tabela->setItem(i, 2, new QTableWidgetItem(QString::number(p.tempoExecucao)));
        tabela->setItem(i, 3, new QTableWidgetItem(QString::number(p.prioridade)));
        tabela->setItem(i, 4, new QTableWidgetItem(QString::number(p.memoriaNecessaria) + " MB"));

        for (int j = 0; j < 5; j++) {
            if (tabela->item(i, j))
                tabela->item(i, j)->setTextAlignment(Qt::AlignCenter);
        }
    }
}

void JanelaPrincipal::politicaEscalonamentoAlterada(int indice) {
    ui->spinQuantum->setEnabled(indice == 0);
    ui->labelQuantum->setEnabled(indice == 0);
}

bool JanelaPrincipal::validarEntradas() {
    if (m_processos.isEmpty()) {
        QMessageBox::warning(this, "Atenção", "Carregue um arquivo CSV com processos antes de simular.");
        return false;
    }
    if (ui->spinMemoriaFisica->value() <= 0) {
        QMessageBox::warning(this, "Atenção", "Informe um valor válido para a memória física.");
        return false;
    }
    if (ui->spinMemoriaVirtual->value() < ui->spinMemoriaFisica->value()) {
        QMessageBox::warning(this, "Atenção", "A memória virtual deve ser maior ou igual à memória física.");
        return false;
    }
    return true;
}

void JanelaPrincipal::executarSimulacao() {
    if (!validarEntradas()) return;

    PoliticaEscalonamento politicaEsc;
    switch (ui->comboEscalonamento->currentIndex()) {
    case 0: politicaEsc = PoliticaEscalonamento::RoundRobin; break;
    case 1: politicaEsc = PoliticaEscalonamento::SJFPreemptivo; break;
    case 2: politicaEsc = PoliticaEscalonamento::PrioridadePreemptiva; break;
    default: politicaEsc = PoliticaEscalonamento::RoundRobin;
    }

    PoliticaSubstituicao politicaSub;
    switch (ui->comboSubstituicao->currentIndex()) {
    case 0: politicaSub = PoliticaSubstituicao::FIFO; break;
    case 1: politicaSub = PoliticaSubstituicao::LRU; break;
    case 2: politicaSub = PoliticaSubstituicao::Otimo; break;
    default: politicaSub = PoliticaSubstituicao::FIFO;
    }

    Simulador simulador;
    ResultadoSimulacao resultado = simulador.executar(
        m_processos,
        politicaEsc,
        ui->spinQuantum->value(),
        ui->spinMemoriaFisica->value(),
        ui->spinMemoriaVirtual->value(),
        politicaSub
        );

    if (!resultado.sucesso) {
        QMessageBox::critical(this, "Erro na simulação", resultado.mensagemErro);
        return;
    }

    exibirResultados(resultado);
}

void JanelaPrincipal::exibirResultados(const ResultadoSimulacao &resultado) {
    // Linha do tempo
    ui->widgetLinhaDoTempo->definirDados(resultado.linhaDoTempo, m_processos.size());

    // Métricas
    ui->labelTMR->setText(QString("Tempo Médio de Resposta: %1 unidades")
                              .arg(resultado.tempoMedioResposta, 0, 'f', 2));
    ui->labelTME->setText(QString("Tempo Médio de Espera: %1 unidades")
                              .arg(resultado.tempoMedioEspera, 0, 'f', 2));
    ui->labelPageFaults->setText(QString("Total de Page Faults: %1")
                                     .arg(resultado.totalPageFaults));

    // Relatório detalhado
    ui->textRelatorio->clear();
    QString relatorio;
    relatorio += "=== RELATÓRIO DE SIMULAÇÃO ===\n\n";
    relatorio += QString("Política de Escalonamento: %1\n")
                     .arg(ui->comboEscalonamento->currentText());
    relatorio += QString("Política de Substituição:  %1\n")
                     .arg(ui->comboSubstituicao->currentText());
    relatorio += QString("Memória Física:            %1 MB\n").arg(ui->spinMemoriaFisica->value());
    relatorio += QString("Memória Virtual:           %1 MB\n").arg(ui->spinMemoriaVirtual->value());

    if (ui->comboEscalonamento->currentIndex() == 0)
        relatorio += QString("Quantum:                   %1\n").arg(ui->spinQuantum->value());

    relatorio += "\n--- Métricas de Performance ---\n";
    relatorio += QString("Tempo Médio de Resposta: %1\n")
                     .arg(resultado.tempoMedioResposta, 0, 'f', 2);
    relatorio += QString("Tempo Médio de Espera:   %1\n")
                     .arg(resultado.tempoMedioEspera, 0, 'f', 2);
    relatorio += QString("Total de Page Faults:     %1\n").arg(resultado.totalPageFaults);

    relatorio += "\n--- Linha do Tempo (resumo) ---\n";
    for (const EntradaLinhaDoTempo &e : resultado.linhaDoTempo) {
        relatorio += QString("P%1: [%2 → %3]\n").arg(e.idProcesso).arg(e.tempoInicio).arg(e.tempoFim);
    }

    ui->textRelatorio->setPlainText(relatorio);
    ui->abaResultados->setCurrentIndex(0);

 
    // ABRE A JANELA (POP-UP) COM OS RESULTADOS
    ui->grupoResultados->show();           
    ui->grupoResultados->raise();          
    ui->grupoResultados->activateWindow();
}

void JanelaPrincipal::limparResultados() {
    ui->widgetLinhaDoTempo->limpar();
    ui->labelTMR->setText("Tempo Médio de Resposta: —");
    ui->labelTME->setText("Tempo Médio de Espera: —");
    ui->labelPageFaults->setText("Total de Page Faults: —");
    ui->textRelatorio->clear();
    m_processos.clear();
    ui->tabelaProcessos->clearContents();
    ui->tabelaProcessos->setRowCount(0);
    ui->labelStatusCSV->setText("Nenhum arquivo carregado.");
    ui->labelStatusCSV->setStyleSheet("color: #888;");
    ui->botaoExecutar->setEnabled(false);

    // Esconde o pop-up caso o usuário clique em Limpar
    ui->grupoResultados->hide();
}
