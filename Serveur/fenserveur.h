#ifndef FENSERVEUR_H
#define FENSERVEUR_H

#include <QWidget>
#include <QtNetwork>

#include "basededonnees.h"
#include "tcpsocketclient.h"

namespace Ui {
class FenServeur;
}

class FenServeur : public QWidget
{
    Q_OBJECT
    
public:
    explicit FenServeur(QString dblogin, QString dbpassword, QString hostname, QWidget *parent = 0);
    ~FenServeur();

    int authCheck(QString login, QString mdp);
    //void createClient(MainWindow *sonIG, QString sonLogin);

    void renvoieValeursUtilisateur(QString login, std::map<QString, QVariant> &valeurs);
    void retourneValeursColonne(QString nomColonne, QString nomtable, QString where, std::queue<QString> &queue);

    bool modifierProduit(QString action, QString nomProduit, QString user, QString typeProduit="null", int nombreProduit=0);
    void reqAffichage(QString type, QString produit, QString classement, bool ordreDESC, int nbTotal, bool dateAjout, bool lastUpdate, QStandardItemModel *model);

    void reqUtilisateursHistorique(std::map< QString, std::list<QString> > &map);
    void reqHisto(QString elem, QString type, QString date1, QString date2, int nbTotal, QStandardItemModel *model);

public slots:
    void nouvelleConnexion();
    void authRecu();
    void deconnexionClient();
    
private slots:
    void on_pushButton_clicked();

signals:
    void nbClientshasChanged(int nb);

private:
    Ui::FenServeur *ui;

    QTcpServer *serveur;
    BaseDeDonnees *m_bdd;
    int m_nbClients;
    int m_portClient;

    QStandardItemModel *m_modele;
    void refonteModele();

    //std::vector<Client*> m_vecteurClient;

    QList<TcpSocketClient*> m_clients;
    quint32 m_tailleMessage;
};

#endif // FENSERVEUR_H
