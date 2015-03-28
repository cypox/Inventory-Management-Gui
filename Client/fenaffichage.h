#ifndef FENAFFICHAGE_H
#define FENAFFICHAGE_H

#include <QMainWindow>
#include <QtGui>
#include "widgetaffichage.h"

namespace Ui {
class fenAffichage;
}

class FenGenerale;

class fenAffichage : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit fenAffichage(QString type, QStandardItemModel *model, QString rappelReq, FenGenerale *gestionFen, std::queue<QString> &queue, FenGenerale *parent = 0);
    ~fenAffichage();

    void buildNewOngletReq(QStandardItemModel* model, std::queue<QString> &queue, QString rappelReq);
    void actualiserOngletReq(QStandardItemModel* model, QString rappelReq);
    void buildNewOngletHisto(QStandardItemModel* model, std::queue<QString> &queue, QString rappelReq);
    
private slots:

    void on_actionQuitter_la_fen_tre_triggered();
    void on_actionFermer_l_onget_courant_triggered();
    void on_actionOnglet_suivant_triggered();
    void on_actionOnglet_pr_c_dent_triggered();

    void on_actionImprimer_triggered();

    void on_actionA_propos_de_Qt_triggered();

    void on_actionActualiser_l_onglet_triggered();

    void on_actionD_placer_dans_une_nouvelle_fen_tre_triggered();

private:
    Ui::fenAffichage *ui;
    int m_compteurReq;
    int m_compteurHisto;
    int m_compteur;
    FenGenerale *m_parent;

    int m_ongletEnReactualisation;
    bool m_dateAjoutEnReactualisation;
    bool m_lastUpdateEnReactualisation;

protected:
    void closeEvent(QCloseEvent *event);

};

#endif // FENAFFICHAGE_H
