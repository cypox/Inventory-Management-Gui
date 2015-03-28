#include "authform.h"
#include "ui_authform.h"
#include "fengenerale.h"

AuthForm::AuthForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AuthForm)
{
    ui->setupUi(this);
    setFixedWidth(this->width());

    // paramétrage du socket
    m_socket = new QTcpSocket(this);
    connect(m_socket, SIGNAL(connected()), this, SLOT(connecte()));
    connect(m_socket, SIGNAL(disconnected()), this, SLOT(deconnecte()));
    connect(m_socket, SIGNAL(readyRead()), this, SLOT(reponseAuth()));
    connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(erreurSocket(QAbstractSocket::SocketError)));

    // définition du focus
    ui->LoginEdit->setFocus();

    m_tailleMessage = 0;
}

AuthForm::~AuthForm()
{
    delete ui;
}

// Tentative de connexion au serveur indiqué sur le port indiqué
void AuthForm::on_ConnexionButton_clicked()
{
    QString serveur = ui->ServeurEdit->text();
    int port = ui->PortEdit->text().toInt();

    ui->listeMessages->append(tr("<em>Tentative de connexion en cours...</em>"));
    m_socket->abort(); // On désactive les connexions précédentes s'il y en a
    m_socket->connectToHost(serveur, port); // On se connecte au serveur demandé
}

void AuthForm::on_QuitButton_clicked()
{
    this->close();
}

// Ce slot est appelé lorsque la connexion au serveur a réussi
void AuthForm::connecte()
{
    ui->listeMessages->append(tr("<font color='#00910a'><em>Connexion réussie !</em></font>"));

    // une fois connecté, on cherche à s'authentifier
    ui->listeMessages->append("<em>Tentative d'authentification en cours...</em>");

    QString login = ui->LoginEdit->text();
    QString mdp = ui->PasswdEdit->text();

    // envoie des infos de login au serveur
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << (quint32)0; // taille
    stream << login;
    stream << mdp;
    stream.device()->seek(0);
    stream << (quint32)(data.size() - sizeof(quint32));
    m_socket->write(data);
}


void AuthForm::reponseAuth()
{
    QDataStream in(m_socket);
    if (m_tailleMessage == 0) // Si on ne connaît pas encore la taille du message, on essaie de la récupérer
    {
        if (m_socket->bytesAvailable() < (int)sizeof(quint32)) // On n'a pas reçu la taille du message en entier
             return;
        in >> m_tailleMessage; // Si on a reçu la taille du message en entier, on la récupère
    }

    // Si on connaît la taille du message, on vérifie si on a reçu le message en entier
    if (m_socket->bytesAvailable() < m_tailleMessage) // Si on n'a pas encore tout reçu, on arrête la méthode
        return;

    int retour;
    in >> retour;
    int port;
    in >> port;
    m_tailleMessage = 0;

    // suivant la valeur, on lance la nouvelle fenêtre ou on renvoie l'erreur adequate
    switch (retour) {
    case 0:
    {
        ui->listeMessages->append("<font color='#00910a'><em>Authentification réussie !</em></font>");
        ui->listeMessages->append("<em>Démarrage de l'interface Client...<br /></em>");
        FenGenerale *w = new FenGenerale(ui->ServeurEdit->text(), port);
        w->show();
        break;
    }
    case 1:
        QMessageBox::critical(this, "Authentification", "Erreur: identifiants incorrects.");
        ui->listeMessages->append("<font color='#bd0000'><em>ERREUR : Echec de l'authentification, identifiants incorrects.<br /></em></font>");
        ui->LoginEdit->setText("");
        ui->PasswdEdit->setText("");
        break;
    case 2:
        QMessageBox::critical(this, "Accès à la BDD", "Erreur rencontrée lors de la tentative d'accès à la BDD. <br /> Raison possible: le serveur MySql est peut-être hors ligne.");
        ui->listeMessages->append("<em>Erreur pendant l'authentification<br /></em>");
        break;
    default:
        break;
    }
}

// Ce slot est appelé lorsqu'on est déconnecté du serveur
void AuthForm::deconnecte()
{
    ui->listeMessages->append(tr("<em>Déconnecté du serveur<br /></em>"));
}

// Ce slot est appelé lorsqu'il y a une erreur
void AuthForm::erreurSocket(QAbstractSocket::SocketError erreur)
{
    switch(erreur) // On affiche un message différent selon l'erreur qu'on nous indique
    {
        case QAbstractSocket::HostNotFoundError:
            ui->listeMessages->append(tr("<font color='#bd0000'><em>ERREUR : le serveur n'a pas pu être trouvé. Vérifiez l'IP et le port.<br /></em></font>"));
            break;
        case QAbstractSocket::ConnectionRefusedError:
            ui->listeMessages->append(tr("<font color='#bd0000'><em>ERREUR : le serveur a refusé la connexion. Vérifiez si le programme \"serveur\" a bien été lancé. Vérifiez aussi l'IP et le port.<br /></em></font>"));
            break;
        case QAbstractSocket::RemoteHostClosedError:
            ui->listeMessages->append(tr("<font color='#bd0000'><em>ERREUR : le serveur a coupé la connexion.</em></font>"));
            break;
        default:
            ui->listeMessages->append(tr("<font color='#bd0000'><em>ERREUR : ") + m_socket->errorString() + tr("<br /></em></font>"));
    }

}
