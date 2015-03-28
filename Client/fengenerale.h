#ifndef FENGENERALE_H
#define FENGENERALE_H

#include <QtGui>
#include <QMainWindow>
#include <map>
#include <queue>
#include <QStandardItemModel>
#include <QtNetwork>
#include <QLayout>

#include "tabresults.h"
#include "intersocketclient.h"
#include "formtype.h"

namespace Ui {
class FenGenerale;
}

class FenGenerale : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit FenGenerale(QString address, int port, QWidget *parent = 0);
    ~FenGenerale();

    // accesseur au socket tcp
    interSocketClient* mySocket();
    TabResults* myTab();

    void appendMessage(QString mess);
    void ajoutNewType(QString type);

    //void setClient(Client *client);
    void buildInfosPersos(std::map<QString, QVariant> &infosPersos);
    void buildDetails(std::queue<QString> &queue);
    void buildAjoutProduit(std::queue<QString> &queue);
    void buildDeleteProduit(std::queue<QString> &queue);
    void buildUpdateProduit(std::queue<QString> &queue);

    void buildComboBoxAffichageProduit(std::queue<QString> &queue);
    void buildComboBoxAffichageType(std::queue<QString> &queue);

    void buildResultsFen(QStandardItemModel *model, QString rappelReq, int id);
    void setBoolFenAffichage (bool val);

    void buildHistoUtilisateurs(QStandardItemModel* model);
    void buildResultsFenHisto(QStandardItemModel* model, QString rappelReq, int id);

    // accessible depuis intersocketclient
    void on_actionQuitter_la_fen_tre_triggered();

private slots:

    void setBddInfo1(QString info);
    void setBddInfo2(QString info);
    void setBddInfoNb1(int info);

    void on_comboBox_activated(QString action);
    void on_comboBox_activated(int index);

    void on_ButtonOkAction_clicked();
    void on_comboBoxtype_activated(QString typeChoisi);
    void on_comboBox_currentIndexChanged(int index);
    bool checkExistence(QString info);

    void on_comboBoxAffichageType_activated(const QString &arg1);
    void on_tabWidget_selected(const QString &arg1);
    void on_ButtonOkAffichage_clicked();

    void on_comboBoxAffichageProduit_currentIndexChanged(int index);
    void on_buttonOkHistorique_clicked();
    void on_ajoutTypePushButton_clicked();
    void on_actionOnglet_suivant_triggered();
    void on_actionOnglet_pr_c_dent_triggered();
    void on_actionActualiser_l_onglet_triggered();
    void on_actionFermer_l_onget_courant_triggered();
    void on_actionD_placer_dans_une_nouvelle_fen_tre_triggered();

    void on_actionA_propos_de_Qt_triggered();
    void m_fenAffichBoolConsequence(bool val);

    void on_actionImprimer_triggered();

    void on_actionEnregistrer_les_r_sultats_triggered();

signals:
    void m_fenAffichBoolChanged(bool);

private:
    void deleteWidgetsLayout();
    void deleteItemsComboBox(QComboBox *box);

    Ui::FenGenerale *ui;
    interSocketClient *m_socket;
    TabResults *m_fenAffichage;

    int m_index;
    int m_sousIndex;
    QString m_action;
    QString m_infobdd1;
    QString m_infobdd2;
    int m_infobddnb1;
    bool m_fenaffichbool;

    std::queue<QString> m_file;
    std::queue<QString> m_queueAffichage;
    std::queue<QString> m_queueHisto;
    QStandardItemModel *m_modelTree;
};

#endif // FENGENERALE_H
