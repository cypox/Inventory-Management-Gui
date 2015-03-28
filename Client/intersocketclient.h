#ifndef INTERSOCKETCLIENT_H
#define INTERSOCKETCLIENT_H

#include <QMainWindow>
#include <QtNetwork>
#include <map>
#include <queue>
#include <QStandardItemModel>

class FenGenerale;

class interSocketClient : public QMainWindow
{
    Q_OBJECT
public:
    explicit interSocketClient(QString address, int port, FenGenerale *parent = 0);

    void envoyerRequeteServeur(int id, int idRetour, QString unStr, QString deuxStr, QString troisStr, QString quatreStr=0, int unInt=0, bool unBool=true, bool deuxBool=true, bool troisBool=true, int idAction=0);

    // onglet action
    void retourneValeursColonne(QString nomColonne, QString nomtable);
    void formAction(int index);
    void formActionFurther(int index, QString typeChoisi);

    void modifierProduit(QString action, QString nomProduit, QString typeProduit="null", int nombreProduit=0);

    // onglet affichage
    void formAffiche(int action, QString typeChoisi);
    void reqAffichage(QString type, QString produit, QString classement, bool ordreDESC, int nbTotal, bool dateAjout, bool lastUpdate, QString rappelReq, int idAction);

    // onglet historique
    void formHisto();
    void reqHisto(QString elem, QString type, QString date1, QString date2, int nbTotal, QString rappelReq, int idAction);
    
signals:
    
public slots:
    void connecte();
    void donneesRecues();
    void deconnecte();
    void erreurSocket(QAbstractSocket::SocketError);
    void traitementRetour(int id, QStandardItemModel *model, int idAction=0, bool retour = true);

private:
    FenGenerale *m_IG;
    QTcpSocket *m_socket;
    quint32 m_tailleMessage;
    QSemaphore m_sem;

    std::queue<QString> m_queue;
    std::map<QString, QVariant> m_infosPersos;
    QString m_stockage;
};

#endif // INTERSOCKETCLIENT_H
