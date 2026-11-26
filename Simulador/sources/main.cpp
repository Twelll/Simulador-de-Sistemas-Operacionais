/* =================================================================================================
   Projeto (Escalonador de Processos)
   ================================================================================================= 
   Lucas Gabriel Rodrigues de Moraes       RA:2669595 
   Thiago Wesley Pasquim de Melo           RA:2669757
   Vinicius Gonçalves de Andrade           RA:2669765 
   =================================================================================================
*/

#include <QApplication>
#include "janelaPrincipal.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("Simulador de Sistemas Operacionais");
    app.setOrganizationName("Engenharia de Computação");

    JanelaPrincipal janela;

    janela.showMaximized(); 

    return app.exec();
}
