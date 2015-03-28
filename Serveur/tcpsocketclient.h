#ifndef TCPSOCKETCLIENT_H
#define TCPSOCKETCLIENT_H

#include <QtNetwork>
#include <QtCore>
#include <map>
#include <queue>
#include <QStandardItemModel>

class FenServeur;

class TcpSocketClient : public QObject
{
    Q_OBJECT
public:
    explicit TcpSocketClient(QTcpSocket *socket, FenServeur *serv, QString loginClient);

    // accesseurs
    void setServeur(FenServeur *serv);
    QTcpSocket* mySocket();
    QTcpServer* myServer();
    int myServerPort();
    QString myLogin();
    void setPortFwd(int port);
    void setMyLogin(QString login, int portFwd);

    // utiles pour le fonctionnement logiciel, requetes bdd
    void envoyerInfosPersos();
    void envoiDonnees(int id, int idRetour, QStandardItemModel *model, int idAction=0, bool retour=true);


    void formAffiche(int action, QString typeChoisi);
    void reqAffichage(QString type, QString produit, QString classement, bool ordreDESC, int nbTotal, bool dateAjout, bool lastUpdate, QString rappelReq);
    void formHisto();
    
signals:
    
public slots:
    void nouvelleConnexion();
    void deconnexionClient();
    void donneesRecues();

private:
    FenServeur *m_serv;
    QTcpSocket *m_socket;
    QTcpServer *m_serveur;
    QTcpSocket *m_socketPrivate;
    int m_serverPort;
    int m_tailleMessage;

    //MainWindow *m_IG;
    std::map<QString, QVariant> m_infosPersos;
    std::queue<QString> m_queue;
    std::map< QString, std::list<QString> > m_map;
};

#endif // TCPSOCKETCLIENT_H
