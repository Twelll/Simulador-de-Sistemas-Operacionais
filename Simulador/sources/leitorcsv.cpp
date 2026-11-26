#include "leitorcsv.h"
#include <QFile>
#include <QTextStream>
#include <QStringList>

QList<Processo> LeitorCSV::carregar(const QString &caminhoArquivo, QString &erro) {
    QList<Processo> processos;
    erro.clear();

    QFile arquivo(caminhoArquivo);
    if (!arquivo.open(QIODevice::ReadOnly | QIODevice::Text)) {
        erro = "Não foi possível abrir o arquivo: " + caminhoArquivo;
        return processos;
    }

    QTextStream stream(&arquivo);
    int numeroLinha = 0;
    int idProcesso = 1;

    while (!stream.atEnd()) {
        QString linha = stream.readLine().trimmed();
        numeroLinha++;

        if (linha.isEmpty()) continue;
        if (numeroLinha == 1 && linha.contains("chegada", Qt::CaseInsensitive)) continue;

        QStringList campos = linha.split(',');
        if (campos.size() < 4) {
            erro = QString("Linha %1 inválida: esperados 4 campos, encontrados %2.")
                       .arg(numeroLinha).arg(campos.size());
            return QList<Processo>();
        }

        bool ok1, ok2, ok3, ok4;
        int chegada  = campos[0].trimmed().toInt(&ok1);
        int execucao = campos[1].trimmed().toInt(&ok2);
        int prior    = campos[2].trimmed().toInt(&ok3);
        int memoria  = campos[3].trimmed().toInt(&ok4);

        if (!ok1 || !ok2 || !ok3 || !ok4 || execucao <= 0 || memoria <= 0) {
            erro = QString("Linha %1 contém valores inválidos.").arg(numeroLinha);
            return QList<Processo>();
        }

        processos.append(Processo(idProcesso++, chegada, execucao, prior, memoria));
    }

    arquivo.close();

    if (processos.isEmpty()) {
        erro = "O arquivo não contém processos válidos.";
    }

    return processos;
}
