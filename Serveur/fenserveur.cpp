#include "fenserveur.h"
#include "ui_fenserveur.h"

FenServeur::FenServeur(QString dblogin, QString dbpassword, QString hostname, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FenServeur)
{
    ui->setupUi(this);
    setFixedSize(this->width(), this->height());

    // Association vue <-> mod�le
    m_modele = new QStandardItemModel();
    ui->vue->setModel(m_modele);
    ui->vue->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    ui->vue->setAlternatingRowColors(true);

    // Construction lien avec bdd
    m_bdd = new BaseDeDonnees(dblogin, dbpassword, "projet", hostname);
    if (m_bdd->test()) {
        ui->etatServeurBDD->setText("La connexion au serveur MySQL a r�ussi.");
    } else {
        ui->etatServeurBDD->setText("<font color='#bd0000'>Echec de la connexion au serveur MySQL.</font>");
    }

    // Gestion du serveur TCP
    serveur = new QTcpServer(this);
    if (!serveur->listen(QHostAddress::Any, 50885)) // D�marrage du serveur sur toutes les IP disponibles et sur le port 50585
    {
        // Si le serveur n'a pas �t� d�marr� correctement
        ui->etatServeur->setText(tr("Le serveur n'a pas pu �tre d�marr�. Raison :<br />") + serveur->errorString());
    }
    else
    {
        // Si le serveur a �t� d�marr� correctement
        ui->etatServeur->setText(tr("Le serveur a �t� d�marr� sur le port <strong>") + QString::number(serveur->serverPort()) + tr("</strong>.<br />Des clients peuvent maintenant se connecter."));
        connect(serveur, SIGNAL(newConnection()), this, SLOT(nouvelleConnexion()));
    }

    m_portClient = 50885;
    connect(this, SIGNAL(nbClientshasChanged(int)), ui->nbClients, SLOT(setValue(int)));
    m_tailleMessage = 0;
}

FenServeur::~FenServeur()
{
    delete ui;
}

// ################ slots ################################

void FenServeur::nouvelleConnexion() {
    //QMessageBox::information(0, "Connexion d'un nouveau client", "Connexion!!");

    TcpSocketClient *nouveauClient = new TcpSocketClient(serveur->nextPendingConnection(), this, "null");
    nouveauClient->setPortFwd(50885);
    m_clients << nouveauClient;
    refonteModele();

    connect(nouveauClient->mySocket(), SIGNAL(readyRead()), this, SLOT(authRecu()));
    connect(nouveauClient->mySocket(), SIGNAL(disconnected()), this, SLOT(deconnexionClient()));
}

void FenServeur::authRecu()
{
    // On d�termine quel client envoie le message (recherche du QTcpSocket du client)
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    if (socket == 0) // Si par hasard on n'a pas trouv� le client � l'origine du signal, on arr�te la m�thode
        return;

    // on retrouve quel client a ce socket dans notre QList
    TcpSocketClient *client;
    int i;
    for (i=0; i<m_clients.count(); ++i) {
        if ((m_clients.at(i)->mySocket())==socket)
            client = m_clients.at(i);
    }

    QDataStream in(socket);
    if (m_tailleMessage == 0) // Si on ne conna�t pas encore la taille du message, on essaie de la r�cup�rer
    {
        if (socket->bytesAvailable() < (int)sizeof(quint32)) // On n'a pas re�u la taille du message en entier
             return;
        in >> m_tailleMessage; // Si on a re�u la taille du message en entier, on la r�cup�re
    }

    // Si on conna�t la taille du message, on v�rifie si on a re�u le message en entier
    if (socket->bytesAvailable() < m_tailleMessage) // Si on n'a pas encore tout re�u, on arr�te la m�thode
        return;

    // Si ces lignes s'ex�cutent, c'est qu'on a re�u tout le message : on peut le r�cup�rer !
    QString login;
    in >> login;
    QString mdp;
    in >> mdp;

    m_tailleMessage = 0;

    // requete sql pour savoir si les logins rentr�s sont bons
    int retourAuth = m_bdd->authCheck(login, mdp);
    int portFwd;

    if (retourAuth == 0) {
        // d�termination du port pour le futur serveur tcp "priv�" du client
        m_portClient++;
        portFwd = m_portClient;

        client->setMyLogin(login, portFwd);
        refonteModele();
    }

    // renvoi de la r�ponse au client
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << (quint32)0;
    stream << retourAuth;
    stream << portFwd;
    stream.device() -> seek(0);
    stream << (quint32)(data.size() - sizeof(quint32));
    socket->write(data);
}

void FenServeur::deconnexionClient()
{
    // On d�termine quel client se d�connecte
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    /*if (socket == 0) // Si par hasard on n'a pas trouv� le client � l'origine du signal, on arr�te la m�thode
        return;*/

    for (int i=0; i<m_clients.count(); ++i) {
        if ((m_clients.at(i)->mySocket())==socket) {
            if (m_clients.at(i)->myServerPort() != 50885)
                m_clients.at(i)->myServer()->deleteLater();
            m_clients.removeAt(i);
            socket->deleteLater();
        }
    }
    refonteModele();
}

void FenServeur::on_pushButton_clicked()
{
    for (int i=0; i<m_clients.count(); ++i) {
            m_clients.at(i)->myServer()->deleteLater();
            m_clients.at(i)->mySocket()->deleteLater();
            m_clients.removeAt(i);
    }
    this->close();
}

// r�organisation de l'architecture MVC qui affiche les clients connect�s
void FenServeur::refonteModele() {
    int j;
    m_modele->setHorizontalHeaderLabels(QStringList() << "Ip" << "Login" << "Port d'acc�s" );
    m_modele->setRowCount(m_clients.count());

    for (int i=0; i<m_clients.count(); ++i) {
        j=0;

        QString Ip, socketName, portFwd;
        Ip = m_clients.at(i)->mySocket()->peerAddress().toString();
        socketName = m_clients.at(i)->myLogin();
        portFwd = QString::number(m_clients.at(i)->myServerPort());

        QStandardItem *itemIp = new QStandardItem(Ip);
        QStandardItem *itemName = new QStandardItem(socketName);
        QStandardItem *itemPort = new QStandardItem(portFwd);

        m_modele->setItem(i, j, itemIp);
        m_modele->setItem(i, j+1, itemName);
        m_modele->setItem(i, j+2, itemPort);
    }

    // mise � jour du nb de clients dans la spinBox
    emit nbClientshasChanged(m_clients.count());
}

// ################ onglet accueil ################################

int FenServeur::authCheck(QString login, QString mdp) {
    return (m_bdd -> authCheck(login, mdp));
}

void FenServeur::renvoieValeursUtilisateur(QString login, std::map<QString, QVariant> &valeurs) {
    m_bdd -> renvoieValeursUtilisateur(login, valeurs);
}

// ################ onglet action ################################

void FenServeur::retourneValeursColonne(QString nomColonne, QString nomtable, QString where, std::queue<QString> &queue) {
    m_bdd -> retourneValeursColonne(nomColonne, nomtable, where, queue);
}

bool FenServeur::modifierProduit(QString action, QString nomProduit, QString user, QString typeProduit, int nombreProduit) {
    return m_bdd-> modifierProduit(action, nomProduit, user, nombreProduit, typeProduit);
}

// ################ onglet affichage ################################

void FenServeur::reqAffichage(QString type, QString produit, QString classement, bool ordreDESC, int nbTotal, bool dateAjout, bool lastUpdate, QStandardItemModel *model) {
    m_bdd -> reqAffichage(type, produit, classement, ordreDESC, nbTotal, dateAjout, lastUpdate, model);
}

// ################ onglet historique ################################

void FenServeur::reqUtilisateursHistorique(std::map<QString, std::list<QString> > &map) {
    m_bdd -> reqUtilisateursHistorique(map);
}

void FenServeur::reqHisto(QString elem, QString type, QString date1, QString date2, int nbTotal, QStandardItemModel *model) {
    m_bdd->reqHisto(elem, type, date1, date2, nbTotal, model);
}


