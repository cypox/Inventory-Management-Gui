#include "intersocketclient.h"
#include "fengenerale.h"

interSocketClient::interSocketClient(QString address, int port, FenGenerale *parent) : m_sem(0), m_stockage(""),
    QMainWindow(parent)
{
    m_IG = parent;
    m_socket = new QTcpSocket;
    m_socket->connectToHost(address, port);

    connect(m_socket, SIGNAL(readyRead()), this, SLOT(donneesRecues()));
    connect(m_socket, SIGNAL(connected()), this, SLOT(connecte()));
    connect(m_socket, SIGNAL(disconnected()), this, SLOT(deconnecte()));
    connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(erreurSocket(QAbstractSocket::SocketError)));

    m_tailleMessage = 0;
}



//                          ####################################################################################
//                          ####################### méthodes appelés par MainWindow ############################
//                          ####################################################################################



//                  #################################    Liées à l'onglet action  ########################################

// remplissage du comboBox de type
void interSocketClient::formAction(int index) {
    switch(index) {
    case 0:
        break;

    case 1:
        envoyerRequeteServeur(0, 1, "type", "type", "none", "null", 0);
        m_IG -> buildDetails(m_queue);
        break;

    case 2:
        m_queue.push("Tous");
        envoyerRequeteServeur(0, 2, "type", "type", "none", "null", 0);
        m_IG -> buildDetails(m_queue);
        break;

    case 3:
        m_queue.push("Tous");
        envoyerRequeteServeur(0, 3, "type", "type", "none", "null", 0);
        m_IG -> buildDetails(m_queue);
        break;

    case 4:
        m_queue.push("Tous");
        envoyerRequeteServeur(0, 4, "type", "type", "none", "null", 0);
        m_IG -> buildDetails(m_queue);
        break;

    }
}

// Remplissage de la deuxième ligne du groupBox Détails du formulaire, dépend de l'action envisagée
void interSocketClient::formActionFurther(int index, QString typeChoisi) {
    switch(index) {
    case 0:
        break;

    case 1:
        envoyerRequeteServeur(0, 50, "produit", "produit", typeChoisi, "null", 0);
        break;

    case 2:
        envoyerRequeteServeur(0, 5, "produit", "produit", typeChoisi, "null", 0);
        break;

    case 3:
        envoyerRequeteServeur(0, 6, "produit", "produit", typeChoisi, "null", 0);
        break;

    case 4:
        envoyerRequeteServeur(0, 7, "produit", "produit", typeChoisi, "null", 0);
        break;
    }
}

// Lors de l'appui sur OK, envoi la requete d'action à la bdd
void interSocketClient::modifierProduit(QString action, QString nomProduit, QString typeProduit, int nombreProduit) {

    QString user;
    std::map<QString, QVariant>::iterator trouve = m_infosPersos.find("nom");
    user += (trouve->second).toString();

    if (action != "ajout_type") {
        envoyerRequeteServeur(1, 8, action, nomProduit, typeProduit, user, nombreProduit);
    } else {
        envoyerRequeteServeur(1, 81, action, "null", typeProduit, user, 0);
    }
}



//                  #################################    Liées à l'onglet affichage  ########################################

void interSocketClient::formAffiche(int action, QString typeChoisi) {

    // action 1 : construction du combo box "type" de l'onglet affichage
    // action 2 : construction du combo box "produit" de l'onglet affichage
    switch(action) {
    case 1:
        m_queue.push("Tous");
        envoyerRequeteServeur(0, 9, "type", "type", "none", "null", 0);
        break;

    case 2:
        m_queue.push("Tous");
        envoyerRequeteServeur(0, 10, "produit", "produit", typeChoisi, "null", 0);
        break;
    }
}

void interSocketClient::reqAffichage(QString type, QString produit, QString classement, bool ordreDESC, int nbTotal, bool dateAjout, bool lastUpdate, QString rappelReq, int idAction) {
    //m_serv -> reqAffichage(type, produit, classement, ordreDESC, nbTotal, dateAjout, lastUpdate, model);
    envoyerRequeteServeur(2, 11, type, produit, classement, "null", nbTotal, ordreDESC, dateAjout, lastUpdate, idAction);
    m_stockage = rappelReq;
}



//                  #################################    Liées à l'onglet historique  ########################################

void interSocketClient::formHisto() {
    envoyerRequeteServeur(3, 12, "null", "null", "null", "null");
}

void interSocketClient::reqHisto(QString elem, QString type, QString date1, QString date2, int nbTotal, QString rappelReq, int idAction) {
    envoyerRequeteServeur(4, 13, elem, type, date1, date2, nbTotal, true, true, true, idAction);
    m_stockage = rappelReq;
}




//                          ####################################################################################
//                          ############################### Emission ###########################################
//                          ####################################################################################


void interSocketClient::envoyerRequeteServeur(int id, int idRetour, QString unStr, QString deuxStr, QString troisStr, QString quatreStr, int unInt, bool unBool, bool deuxBool, bool troisBool, int idAction) {
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);

    //QMessageBox::information(0, "debug", "envoi "+QString::number(id)+" "+QString::number(idRetour));

    stream << (quint32) 0;
    stream << id;
    stream << idRetour;
    stream << idAction;
    stream << unStr;
    stream << deuxStr;
    stream << troisStr;
    stream << quatreStr;
    stream << unInt;
    stream << unBool;
    stream << deuxBool;
    stream << troisBool;

    // on passe la queue par itération
    int nbIter = m_queue.size();
    stream << nbIter;
    for (int i=0; i<nbIter; ++i) {
        QString str = m_queue.front();
        stream << str;
        m_queue.pop();
    }

    stream.device()->seek(0);
    stream << (quint32) (data.size() - sizeof(quint32));

    m_socket->write(data);
}



//                          ####################################################################################
//                          ############################### Réception ##########################################
//                          ####################################################################################

void interSocketClient::donneesRecues() {

    QDataStream in(m_socket);
    if (m_tailleMessage == 0)
    {
        if (m_socket->bytesAvailable() < (int)sizeof(quint32))
             return;
        in >> m_tailleMessage;
    }

    if (m_socket->bytesAvailable() < m_tailleMessage)
        return;

    // Si on arrive jusqu'à cette ligne, on peut récupérer le message entier
    QStandardItemModel *model = new QStandardItemModel;
    int id, idRetour, nbRows, nbColums, idAction; // int d'identification du traitement à suivre
    in >> id;
    //QMessageBox::information(0, "debug", "recoit "+QString::number(id));

    switch (id) {
    case 0:
    {
        int nbIter;
        in >> nbIter;
        std::map<QString, QVariant>::iterator it = m_infosPersos.begin();

        // remplissage de la map
        for (int i=0; i<nbIter; ++i) {
            QString str;
            in >> str;
            QVariant variantItem;
            in >> variantItem;
            m_infosPersos.insert (it, std::pair<QString, QVariant>(str, variantItem));
        }
        // on appelle la méthode de construction des infos persos
        m_IG -> buildInfosPersos(m_infosPersos);
    }
        break;

    case 1:
        in >> idRetour;
        //QMessageBox::information(0, "debug", "recoit "+QString::number(idRetour));
        int nbIter;
        in >> nbIter;

        // on remplit m_queue
        for (int i=0; i<nbIter; ++i) {
            QString str;
            in >> str;
            m_queue.push(str);
        }
        traitementRetour(idRetour, model);
        break;

    case 2:
        in >> idRetour;
        //QMessageBox::information(0, "debug", "recoit "+QString::number(idRetour));
        bool retour;
        in >> retour;
        traitementRetour(idRetour, model, 0, retour);
        break;

    case 3:
        in >> idRetour;
        //QMessageBox::information(0, "debug", "recoit "+QString::number(idRetour));
        in >> idAction;
        in >> nbRows;
        in >> nbColums;
        for (int i = 0; i<nbRows; ++i) {
            for (int j=0; j<nbColums; ++j) {
                QString str;
                in >> str;
                model->setItem(i, j, new QStandardItem(str));
            }
        }
        traitementRetour(idRetour, model, idAction);
        break;

    case 4:
        in >> idRetour;
        //QMessageBox::information(0, "debug", "recoit "+QString::number(idRetour));
    {
        QStandardItem *itemTous = new QStandardItem("Tous");
        QStandardItem *itemPays = new QStandardItem("Classement par pays");
        QStandardItem *itemDpt = new QStandardItem("Classement par département");
        model->appendRow(itemTous);
        model->appendRow(itemDpt);
        model->appendRow(itemPays);

        int nbPays, nbVilles, nbPersonnes;
        in >> nbPays;

        for(int i=0; i<nbPays; ++i) {
            QString paysStr;
            in >> paysStr;
            QStandardItem *pays = new QStandardItem(paysStr);
            itemPays->appendRow(pays);

            in >> nbVilles;

            for (int j=0; j<nbVilles; ++j) {
                QString villeStr;
                in >> villeStr;
                QStandardItem *ville = new QStandardItem(villeStr);
                pays->appendRow(ville);

                in >> nbPersonnes;

                for (int k=0; k<nbPersonnes; ++k) {
                    QString nomStr;
                    in >> nomStr;
                    QStandardItem *nom = new QStandardItem(nomStr);
                    ville->appendRow(nom);
                }
            }
        }
    }
        traitementRetour(idRetour, model);
        break;
    }



    // On remet la taille du message à 0 pour pouvoir recevoir de futurs messages
    m_tailleMessage = 0;
}



//                          ####################################################################################
//                          ################################## Slots ###########################################
//                          ####################################################################################

// Ce slot est appelé lorsque la connexion au serveur a réussi
void interSocketClient::connecte()
{
    m_IG->appendMessage(tr("<em>Connexion réussie !</em>"));
}

// Ce slot est appelé lorsqu'on est déconnecté du serveur
void interSocketClient::deconnecte()
{
    m_IG->appendMessage(tr("<em>Déconnecté du serveur</em>"));
    m_IG->on_actionQuitter_la_fen_tre_triggered();
}

// Ce slot est appelé lorsqu'il y a une erreur
void interSocketClient::erreurSocket(QAbstractSocket::SocketError erreur)
{
    switch(erreur) // On affiche un message différent selon l'erreur qu'on nous indique
    {
        case QAbstractSocket::HostNotFoundError:
            m_IG->appendMessage(tr("<em>ERREUR : le serveur n'a pas pu être trouvé. Vérifiez l'IP et le port.</em>"));
            break;
        case QAbstractSocket::ConnectionRefusedError:
            m_IG->appendMessage(tr("<em>ERREUR : le serveur a refusé la connexion. Vérifiez si le programme \"serveur\" a bien été lancé. Vérifiez aussi l'IP et le port.</em>"));
            break;
        case QAbstractSocket::RemoteHostClosedError:
            m_IG->appendMessage(tr("<em>ERREUR : le serveur a coupé la connexion.</em>"));
            break;
        default:
            m_IG->appendMessage(tr("<em>ERREUR : ") + m_socket->errorString() + tr("</em>"));
    }
}

void interSocketClient::traitementRetour(int id, QStandardItemModel *model, int idAction, bool retour) {
    switch (id) {
    case 1:
        m_IG -> buildDetails(m_queue);
        break;
    case 2:
        m_IG -> buildDetails(m_queue);
        break;
    case 3:
        m_IG -> buildDetails(m_queue);
        break;
    case 4:
        m_IG -> buildDetails(m_queue);
        break;
    case 50:
        m_IG -> buildAjoutProduit(m_queue);
        break;
    case 5:
        m_IG -> buildDeleteProduit(m_queue);
        break;
    case 6:
        m_IG -> buildUpdateProduit(m_queue);
        break;
    case 7:
        m_IG -> buildUpdateProduit(m_queue);
        break;
    case 8:
        if (retour) {
            QMessageBox::information(m_IG, "Modification de la BDD", "Modification de la BDD réussie avec succès!");
        } else {
            QMessageBox::critical(m_IG, "Modification de la BDD", "Problème!");
        }
        break;
    case 81:
        if (retour) {
            QMessageBox::information(m_IG, "Ajout d'un type", "Ajout du nouveau type réussie avec succès!");
        } else {
            QMessageBox::critical(m_IG, "Ajout d'un type", "Problème!");
        }
        break;
    case 9:
        m_IG -> buildComboBoxAffichageType(m_queue);
        break;
    case 10:
        m_IG -> buildComboBoxAffichageProduit(m_queue);
        break;
    case 11:
        m_IG -> buildResultsFen(model, m_stockage, idAction);
        m_stockage = "";
        break;
    case 12:
        m_IG->buildHistoUtilisateurs(model);
        break;
    case 13:
        m_IG->buildResultsFenHisto(model, m_stockage, idAction);
        m_stockage = "";
        break;
    }
}
