#ifndef AUTHFORM_H
#define AUTHFORM_H

#include <QWidget>
#include <QtNetwork>

class FenGenerale;

namespace Ui {
class AuthForm;
}

class AuthForm : public QWidget
{
    Q_OBJECT
    
public:
    explicit AuthForm(QWidget *parent = 0);
    ~AuthForm();

    //void setServeur(Serveur *serv);

public slots:
    void on_QuitButton_clicked();
    
private slots:
    void on_ConnexionButton_clicked();
    void connecte();
    void reponseAuth();
    void deconnecte();
    void erreurSocket(QAbstractSocket::SocketError erreur);


private:
    Ui::AuthForm *ui;
    QTcpSocket *m_socket;
    //Serveur *m_serveur;

    quint32 m_tailleMessage;
};

#endif // AUTHFORM_H
