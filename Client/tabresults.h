#ifndef TABRESULTS_H
#define TABRESULTS_H

#include <QTabWidget>
#include <QtGui>
#include "widgetaffichage.h"
#include <QWebView>


namespace Ui {
class TabResults;
}

class FenGenerale;

class TabResults : public QTabWidget
{
    Q_OBJECT
    
public:
    explicit TabResults(QString type, QStandardItemModel *model, QString rappelReq, FenGenerale *gestionFen, std::queue<QString> &queue, FenGenerale *parent = 0);
    ~TabResults();

    void buildNewOngletReq(QStandardItemModel* model, std::queue<QString> &queue, QString rappelReq);
    void actualiserOngletReq(QStandardItemModel* model, QString rappelReq);
    void buildNewOngletHisto(QStandardItemModel* model, std::queue<QString> &queue, QString rappelReq);
    void actualiserOngletHisto(QStandardItemModel *model,QString rappelReq);

public slots:

    void on_actionQuitter_la_fen_tre_triggered();
    void on_actionFermer_l_onget_courant_triggered();
    void on_actionOnglet_suivant_triggered();
    void on_actionOnglet_pr_c_dent_triggered();
    void on_actionImprimer_triggered();
    void on_actionA_propos_de_Qt_triggered();
    void on_actionActualiser_l_onglet_triggered();
    void on_actionD_placer_dans_une_nouvelle_fen_tre_triggered();
    void on_actionEnregistrer_les_r_sultats_triggered();
    
private:
    Ui::TabResults *ui;
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

#endif // TABRESULTS_H
