#include "authform.h"
#include "ui_authform.h"
#include "fengenerale.h"

AuthForm::AuthForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AuthForm)
{
    ui->setupUi(this);
    setFixedWidth(this->width());

    // param�trage du socket
    m_socket = new QTcpSocket(this);
    connect(m_socket, SIGNAL(connected()), this, SLOT(connecte()));
    connect(m_socket, SIGNAL(disconnected()), this, SLOT(deconnecte()));
    connect(m_socket, SIGNAL(readyRead()), this, SLOT(reponseAuth()));
    connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(erreurSocket(QAbstractSocket::SocketError)));

    // d�finition du focus
    ui->LoginEdit->setFocus();

    m_tailleMessage = 0;
}

AuthForm::~AuthForm()
{
    delete ui;
}

// Tentative de connexion au serveur indiqu� sur le port indiqu�
void AuthForm::on_ConnexionButton_clicked()
{
    QString serveur = ui->ServeurEdit->text();
    int port = ui->PortEdit->text().toInt();

    ui->listeMessages->append(tr("<em>Tentative de connexion en cours...</em>"));
    m_socket->abort(); // On d�sactive les connexions pr�c�dentes s'il y en a
    m_socket->connectToHost(serveur, port); // On se connecte au serveur demand�
}

void AuthForm::on_QuitButton_clicked()
{
    this->close();
}

// Ce slot est appel� lorsque la connexion au serveur a r�ussi
void AuthForm::connecte()
{
    ui->listeMessages->append(tr("<font color='#00910a'><em>Connexion r�ussie !</em></font>"));

    // une fois connect�, on cherche � s'authentifier
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
    if (m_tailleMessage == 0) // Si on ne conna�t pas encore la taille du message, on essaie de la r�cup�rer
    {
        if (m_socket->bytesAvailable() < (int)sizeof(quint32)) // On n'a pas re�u la taille du message en entier
             return;
        in >> m_tailleMessage; // Si on a re�u la taille du message en entier, on la r�cup�re
    }

    // Si on conna�t la taille du message, on v�rifie si on a re�u le message en entier
    if (m_socket->bytesAvailable() < m_tailleMessage) // Si on n'a pas encore tout re�u, on arr�te la m�thode
        return;

    int retour;
    in >> retour;
    int port;
    in >> port;
    m_tailleMessage = 0;

    // suivant la valeur, on lance la nouvelle fen�tre ou on renvoie l'erreur adequate
    switch (retour) {
    case 0:
    {
        ui->listeMessages->append("<font color='#00910a'><em>Authentification r�ussie !</em></font>");
        ui->listeMessages->append("<em>D�marrage de l'interface Client...<br /></em>");
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
        QMessageBox::critical(this, "Acc�s � la BDD", "Erreur rencontr�e lors de la tentative d'acc�s � la BDD. <br /> Raison possible: le serveur MySql est peut-�tre hors ligne.");
        ui->listeMessages->append("<em>Erreur pendant l'authentification<br /></em>");
        break;
    default:
        break;
    }
}

// Ce slot est appel� lorsqu'on est d�connect� du serveur
void AuthForm::deconnecte()
{
    ui->listeMessages->append(tr("<em>D�connect� du serveur<br /></em>"));
}

// Ce slot est appel� lorsqu'il y a une erreur
void AuthForm::erreurSocket(QAbstractSocket::SocketError erreur)
{
    switch(erreur) // On affiche un message diff�rent selon l'erreur qu'on nous indique
    {
        case QAbstractSocket::HostNotFoundError:
            ui->listeMessages->append(tr("<font color='#bd0000'><em>ERREUR : le serveur n'a pas pu �tre trouv�. V�rifiez l'IP et le port.<br /></em></font>"));
            break;
        case QAbstractSocket::ConnectionRefusedError:
            ui->listeMessages->append(tr("<font color='#bd0000'><em>ERREUR : le serveur a refus� la connexion. V�rifiez si le programme \"serveur\" a bien �t� lanc�. V�rifiez aussi l'IP et le port.<br /></em></font>"));
            break;
        case QAbstractSocket::RemoteHostClosedError:
            ui->listeMessages->append(tr("<font color='#bd0000'><em>ERREUR : le serveur a coup� la connexion.</em></font>"));
            break;
        default:
            ui->listeMessages->append(tr("<font color='#bd0000'><em>ERREUR : ") + m_socket->errorString() + tr("<br /></em></font>"));
    }

}
