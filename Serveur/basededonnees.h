#ifndef BASEDEDONNEES_H
#define BASEDEDONNEES_H

#include <QTextEdit>
#include <QtCore>
#include <QSqlDatabase>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlRecord>
#include <map>
#include <queue>

#include <QStandardItem>
#include <QStandardItemModel>

#include <iostream>
#include <map>
#include <list>

#define qtoc(string) string.toStdString()

class BaseDeDonnees
{

public:
    BaseDeDonnees(QString user, QString passwd, QString database, QString ip);

    bool test();
    int authCheck(QString login, QString mdp);

    void renvoieValeursUtilisateur(QString login, std::map<QString, QVariant> &valeurs);
    void retourneValeursColonne(QString nomColonne, QString nomtable, QString where, std::queue<QString> &queue);

    bool modifierProduit(QString action, QString nomProduit, QString user, int nombreProduit, QString typeProduit="null");
    void reqAffichage(QString type, QString produit, QString classement, bool ordreDESC, int nbTotal, bool dateAjout, bool lastUpdate, QStandardItemModel *model);

    void reqUtilisateursHistorique(std::map< QString, std::list<QString> > &map);
    void reqHisto(QString elem, QString type, QString date1, QString date2, int nbTotal, QStandardItemModel *model);

private:
    QSqlDatabase m_db;

    QString conversion(int typeAConvert, QString idstr, int typeAction=-1);
};

#endif // BASEDEDONNEES_H
