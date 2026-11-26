#ifndef LEITORCSV_H
#define LEITORCSV_H

#include <QString>
#include <QList>
#include "processo.h"

class LeitorCSV {
public:
    static QList<Processo> carregar(const QString &caminhoArquivo, QString &erro);
};

#endif // LEITORCSV_H
