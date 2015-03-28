#include "tcpsocketclient.h"
#include "fenserveur.h"

TcpSocketClient::TcpSocketClient(QTcpSocket *socket, FenServeur *serv, QString loginClient)
{
    m_socket = socket;
    m_serv = serv;
    m_infosPersos["login"] = loginClient;
    m_tailleMessage = 0;
}

// slot appelé lorsque le client se connecte à son serveur tcp "privé"
void TcpSocketClient::nouvelleConnexion() {

    m_socketPrivate = m_serveur->nextPendingConnection();

    connect(m_socketPrivate, SIGNAL(readyRead()), this, SLOT(donneesRecues()));
    connect(m_socketPrivate, SIGNAL(disconnected()), this, SLOT(deconnexionClient()));

    envoyerInfosPersos();
}

// slot appelé à la déconnexion client
void TcpSocketClient::deconnexionClient()
{
    m_socketPrivate->deleteLater();
}

// accesseurs divers
QTcpSocket* TcpSocketClient::mySocket() {
    return this->m_socket;
}
QTcpServer* TcpSocketClient::myServer() {
    return this->m_serveur;
}
int TcpSocketClient::myServerPort() {
    return m_serverPort;
}
QString TcpSocketClient::myLogin() {
    return m_infosPersos["login"].toString();
}
void TcpSocketClient::setPortFwd(int port) {
    m_serverPort = port;
}

// accesseur particulier, normalement appelé qu'une seule fois, une fois le client authentifié on vient renseigner son login,
// le port auquel il va se connecter et on va chercher dans la bdd toutes les infos qui concernent son profil
void TcpSocketClient::setMyLogin(QString login, int portFwd) {

    m_serverPort = portFwd; // port sur lequel il est autorisé à se connecter
    m_infosPersos["login"] = login;
    m_serv -> renvoieValeursUtilisateur(login, m_infosPersos);  // requete bdd pour ses inofs persos

    // démarrage du serveur TCP
    m_serveur = new QTcpServer(this);
    if (m_serveur->listen(QHostAddress::Any, portFwd))
        connect(m_serveur, SIGNAL(newConnection()), this, SLOT(nouvelleConnexion()));
}

//                          ####################################################################################
//                          ############################### émission ###########################################

void TcpSocketClient::envoyerInfosPersos() {
    // on envoie la map d'infos persos
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);

    int nbIter = m_infosPersos.size();
    int id = 0; // permet au client d'identifier le type de traitement à appliquer aux données reçues

    stream << (quint32)0;
    stream << id;
    stream << nbIter;

    // on passe la map par itération, seule moyen trouvé pour l'instant
    for (std::map<QString, QVariant>::iterator it=m_infosPersos.begin(); it!=m_infosPersos.end(); ++it) {
        QString str = it->first;
        QVariant variantItem = it->second;
        stream << str;
        stream << variantItem;
    }
    stream.device()->seek(0);
    stream << (quint32) (data.size() - sizeof(quint32));

    m_socketPrivate->write(data);
}

void TcpSocketClient::envoiDonnees(int id, int idRetour, QStandardItemModel *model, int idAction, bool retour) {
    // renvoi de données demandées par le client
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);

    stream << (quint32)0;
    stream << id;
    stream << idRetour;

    //QMessageBox::information(0, "debug", " serveur envoie "+QString::number(id)+" "+QString::number(idRetour));

    int nbIter = m_queue.size();
    int nbRows = model->rowCount();
    int nbColums = model->columnCount();

    switch (id) {       // id permet de différencier le traitement pour le client (à la réception de cet envoi)
    case 1:
        stream << nbIter;

        // on passe la queue par itération, seul moyen trouvé pour l'instant
        for (int i=0; i<nbIter; ++i) {
            QString str = m_queue.front();
            stream << str;
            m_queue.pop();
        }
        break;

    case 2:
        stream << retour;
        break;

    case 3:
        stream << idAction;
        stream << nbRows;
        stream << nbColums;
        for (int i = 0; i<nbRows; ++i) {
            for (int j=0; j<nbColums; ++j) {
                QString str = model->item(i, j)->text();
                stream << str;
            }
        }
        break;

    case 4:
    {
        std::map< QString, std::list<QString> >::iterator it = m_map.begin();
        it = m_map.find("pays");
        std::list<QString> payslist = (it->second);
        int nbPays = payslist.size();
        payslist.sort();
        stream << nbPays;

        for(int i=0; i<nbPays; ++i) {
            QString paysStr = payslist.front();
            payslist.pop_front();
            stream << paysStr;

            it = m_map.find(paysStr);
            std::list<QString> villeslist = (it->second);
            int nbVilles = villeslist.size();
            villeslist.sort();
            stream << nbVilles;

            for (int j=0; j<nbVilles; ++j) {
                QString villeStr = villeslist.front();
                villeslist.pop_front();
                stream << villeStr;

                it = m_map.find(villeStr);
                std::list<QString> personneslist = (it->second);
                int nbPersonnes = personneslist.size();
                personneslist.sort();
                stream << nbPersonnes;

                for (int k=0; k<nbPersonnes; ++k) {
                    QString nomStr = personneslist.front();
                    personneslist.pop_front();
                    stream << nomStr;
                }
            }
        }
    }
        break;
    }
    stream.device()->seek(0);
    stream << (quint32) (data.size() - sizeof(quint32));

    m_socketPrivate->write(data);
}

//                          ####################################################################################
//                          ############################### Réception ###########################################

void TcpSocketClient::donneesRecues() {

    QDataStream in(m_socketPrivate);
    if (m_tailleMessage == 0)
    {
        if (m_socketPrivate->bytesAvailable() < (int)sizeof(quint32))
             return;
        in >> m_tailleMessage;
    }

    if (m_socketPrivate->bytesAvailable() < m_tailleMessage)
        return;


    // Si on arrive jusqu'à cette ligne, on peut récupérer le message entier
    int id, idRetour, unInt, idAction;
    QString unStr, deuxStr, troisStr, quatreStr;
    bool unBool, deuxBool, troisBool;
    in >> id;
    in >> idRetour;
    in >> idAction;
    in >> unStr;
    in >> deuxStr;
    in >> troisStr;
    in >> quatreStr;
    in >> unInt;
    in >> unBool;
    in >> deuxBool;
    in >> troisBool;

    int nbIter;
    in >> nbIter;
    // on remplit m_queue
    for (int i=0; i<nbIter; ++i) {
        QString str;
        in >> str;
        m_queue.push(str);
    }

    // On remet la taille du message à 0 pour pouvoir recevoir de futurs messages
    m_tailleMessage = 0;
    QStandardItemModel *model = new QStandardItemModel;
    //QMessageBox::information(0, "debug", " serveur recoit "+QString::number(id)+" "+QString::number(idRetour));

    switch (id) {
    case 0:
        m_serv -> retourneValeursColonne(unStr, deuxStr, troisStr, m_queue);
        envoiDonnees(1, idRetour, model);
        break;

    case 1:
    {
        bool retour = m_serv -> modifierProduit(unStr, deuxStr, quatreStr, troisStr, unInt);
        envoiDonnees(2, idRetour, model, 0, retour);
    }
        break;

    case 2:
        m_serv ->reqAffichage(unStr, deuxStr, troisStr, unBool, unInt, deuxBool, troisBool, model);
        envoiDonnees(3, idRetour, model, idAction);
        break;

    case 3:
        m_serv->reqUtilisateursHistorique(m_map);
        envoiDonnees(4, idRetour, model);
        break;

    case 4:
        m_serv->reqHisto(unStr, deuxStr, troisStr, quatreStr, unInt, model);
        envoiDonnees(3, idRetour, model, idAction);
        break;
    }

}
