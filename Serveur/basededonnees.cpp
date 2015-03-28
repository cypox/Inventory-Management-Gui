#include "basededonnees.h"

BaseDeDonnees::BaseDeDonnees(QString user, QString passwd, QString database, QString ip)
{
    m_db = QSqlDatabase::addDatabase("QMYSQL");
        m_db.setHostName(ip);
        m_db.setUserName(user);
        m_db.setPassword(passwd);
        m_db.setDatabaseName(database);
}

bool BaseDeDonnees::test() {
    return m_db.open();
}

// appelé par la fenÃªtre d'authentification
int BaseDeDonnees::authCheck(QString login, QString mdp) {
    if (m_db.open()) {

        QByteArray encodedPass_bef = QCryptographicHash::hash(mdp.toUtf8(),QCryptographicHash::Md5);
        QString encodedPass = encodedPass_bef.toHex();

        QSqlQuery query;
        query.exec("SELECT pseudo, password FROM utilisateurs");
		while(query.next()) {
			for (int i=0; i<query.record().count(); i++) {
                if (query.record().fieldName(i) == "pseudo" && query.value(i).toString() == login) {
                    if (query.value(i+1).toString() == encodedPass) {
                        return 0;
					}
				}
            }
        }
		m_db.close();
        return 1;
		
    } else {
        return 2;
    }
}

bool BaseDeDonnees::modifierProduit(QString action, QString nomProduit, QString user, int nombreProduit, QString typeProduit) {
    if (m_db.open()) {

            QString logStr, logStrDeletedState;
            QSqlQuery query;
            QSqlQuery queryLog, queryLogDeletedState;

            if (action == "ajout") {
                action = "ajout d'un produit";
                query.prepare("INSERT INTO produit (produit, type, stock, ajout_date, last_update_date) VALUES (:nom, (SELECT id FROM type WHERE type = :type), :nombre, NOW(), NOW())");
                query.bindValue(":type", typeProduit);
                query.bindValue(":nom", nomProduit);
                query.bindValue(":nombre", nombreProduit);                

            } else if (action == "suppression") {
                action = action = "suppression d'un produit";
                query.prepare("UPDATE produit SET stock='0', is_deleted='1' WHERE produit=(:produit)");
                query.bindValue(":produit", nomProduit);

            } else if (action == "rajouter") {                
                action = "rajout d'une quantité";
                query.prepare("UPDATE produit SET stock = stock + (:nombre), last_update_date = NOW() WHERE produit = (:name)");
                query.bindValue(":name", nomProduit);
                query.bindValue(":nombre", nombreProduit);

            } else if (action == "enlever") {
                action = "retrait d'une quantité";
                query.prepare("UPDATE produit SET stock = stock - (:nombre), last_update_date = NOW() WHERE produit = (:name)");
                query.bindValue(":name", nomProduit);
                query.bindValue(":nombre", nombreProduit);

            } else if (action == "ajout_type") {
                action = "ajout d'un type";
                query.prepare("INSERT INTO type (type) VALUES (:type)");
                query.bindValue(":type", typeProduit);
            }

            logStr = "INSERT INTO logs (utilisateur, date, action, produit, quantite) VALUES ((SELECT id FROM utilisateurs WHERE nom = :user), NOW(), (SELECT id FROM actions WHERE nom = :action), (SELECT id FROM produit WHERE produit = :produit), :quantite)";
            queryLog.prepare(logStr);
            queryLog.bindValue(":user", user);
            queryLog.bindValue(":action", action);
            queryLog.bindValue(":produit", nomProduit);
            queryLog.bindValue(":quantite", nombreProduit);

            if (query.exec()) {
                if (action != "ajout d'un type")
                    queryLog.exec();
                return true;
            } else {
               // QMessageBox::critical(m_parent, "Ajout d'un produit dans la BDD", "Problème!");
               return false;
            }
        return false;
    }
    return false;
}

void BaseDeDonnees::retourneValeursColonne(QString nomColonne, QString nomtable, QString where, std::queue<QString> &queue) {

    bool passe;
    if (m_db.open()) {

            QSqlQuery query;
            QString wherestr = " WHERE", rqt;

            if (where == "none" || where == "Tous") {
                rqt = "SELECT "+nomColonne+" FROM "+nomtable+"";
                if (nomtable == "produit") {rqt+=" WHERE is_deleted='0'";}
                query.prepare(rqt);
                passe = false;
            } else {
                if (nomtable == "produit") {wherestr +=" is_deleted='0' AND";}
                rqt = "SELECT "+nomColonne+" FROM "+nomtable+wherestr+" type = (SELECT id FROM type WHERE type = (:typeChoisi))";
                query.prepare(rqt);
                query.bindValue(":typeChoisi", where);
                passe = true;
            }

            if (query.exec()) {
                while(query.next()) {
                    for (int i=0; i<query.record().count(); i++) {
                        if (!passe || query.value(i) != where) {
                            queue.push(query.value(i).toString());
                        }
                    }
                }

                m_db.close();
              //  QMessageBox::information(m_parent, "Selection dans la BDD", "Sélection réussie !");
            } else {
              //  QMessageBox::critical(m_parent, "Sélection dans la BDD", "Problème!");
            }
    }
}

void BaseDeDonnees::renvoieValeursUtilisateur(QString login, std::map<QString, QVariant> &valeurs) {

    if (m_db.open()) {

            QSqlQuery query;
            query.prepare("SELECT pseudo, password, nom, prenom, anciennete, departement, poste, ville FROM utilisateurs WHERE pseudo=(:name)");
            query.bindValue(":name", login);

            if (query.exec()) {
                while(query.next()) {
                    for (int i=0; i<query.record().count(); i++) {
                        valeurs[query.record().fieldName(i)] = query.value(i);
                    }
                }
                m_db.close();
            } else {
              //  QMessageBox::critical(m_parent, "Sélection dans la BDD", "Problème!");
            }
    }
}

void BaseDeDonnees::reqAffichage(QString type, QString produit, QString classement, bool ordreDESC, int nbTotal, bool dateAjout, bool lastUpdate, QStandardItemModel* model) {

    // #######################
    // préparation de la future forme de la requête en fonction des arguments qui arrivent

    QString wherestr, reqstr1, reqstr2, reqstr3, reqstr4, reqstr5;
    wherestr = "WHERE ";
    if (dateAjout) {reqstr1 = ", DATE_FORMAT(ajout_date, \'%d/%m/%Y à %H:%i:%s\') AS ajout_date";} else {reqstr1 = "";}
    if (lastUpdate) {reqstr2 = ", DATE_FORMAT(last_update_date, \'%d/%m/%Y à %H:%i:%s\') AS last_update_date";} else {reqstr2 = "";}

    if (type != "Tous") {
        if (produit == "Tous") {
            reqstr3 = "type=(SELECT id FROM type WHERE type = (:type)) AND is_deleted=0";
        } else {
            reqstr3 = "(type=(SELECT id FROM type WHERE type = (:type)) AND produit=(:produit)) AND is_deleted=0";
        }
    } else if (type == "Tous") {
        if (produit == "Tous") {
            reqstr3 = " is_deleted=0";
        } else if (produit != "Tous") {
            reqstr3 = "produit=(:produitselect) AND is_deleted=0";
        }
    }

    reqstr4 = " ORDER BY "+classement;
    if (ordreDESC) {
        reqstr4 += " DESC";
    }

    reqstr5 = " LIMIT 0,"+QString::number(nbTotal);

    // #######################

    // lancement du processus de requête
    if (m_db.open()) {

        QSqlQuery query;
        query.prepare("SELECT produit, type, stock"+reqstr1+reqstr2+", is_deleted FROM produit "+wherestr+reqstr3+reqstr4+reqstr5);

        // en fonction des arguments type et produit demandés, on a besoin de préparer plus ou moins la requêtre avec un WHERE
        if (type != "Tous") {
            if (produit == "Tous") {
                query.bindValue(":type", type);
            } else {
                query.bindValue(":type", type);
                query.bindValue(":produit", produit);
            }
        } else if (type == "Tous") {
            if (produit != "Tous") {
                query.bindValue(":produitselect", produit);
            }
        }

        // execution de la requête
        if (query.exec()) {
            QSqlRecord record = query.record();
            int i=0, j;

            while (query.next()) {
                j=0;

                QString produitString = query.value(record.indexOf("produit")).toString();
                int typeStringAConvertir = query.value(record.indexOf("type")).toInt();
                QString typeString = conversion(typeStringAConvertir, "type");
                QString stockString = query.value(record.indexOf("stock")).toString();

                QStandardItem *itemProduit = new QStandardItem;
                QStandardItem *itemType = new QStandardItem;
                QStandardItem *itemStock = new QStandardItem;

                itemProduit->setText(produitString);
                itemType->setText(typeString);
                itemStock->setText(stockString);
                model->setItem(i, j, itemProduit);
                model->setItem(i, j+1, itemType);
                model->setItem(i, j+2, itemStock);

                int x = j+3;

                if (dateAjout) {
                    QString date_ajoutString = query.value(record.indexOf("ajout_date")).toString();
                    QStandardItem *itemAjoutDate = new QStandardItem;
                    itemAjoutDate->setText(date_ajoutString);
                    model->setItem(i, x, itemAjoutDate);
                    x = x+1;
                }
                if (lastUpdate) {
                    QString updateString = query.value(record.indexOf("last_update_date")).toString();
                    QStandardItem *itemUpdate = new QStandardItem;
                    itemUpdate->setText(updateString);
                    model->setItem(i, x, itemUpdate);
                }

                i++;
            }
            m_db.close();
        }
    }
}

void BaseDeDonnees::reqHisto(QString elem, QString type, QString date1, QString date2, int nbTotal, QStandardItemModel *model) {

    QString where = " WHERE ";

    if (type == "ville") {
        where+="u.ville=(:value) ";
    } else if (type=="pays") {
        where+="u.pays=(:value) ";
    } else if (type=="utilisateur") {
        where+="u.nom=(:value) ";
        elem = elem.split(" ").first();
        /*QTextEdit *fen = new QTextEdit(elem);
        fen->show();*/
    }

    QString reqstr = where+"AND (date>='"+date1+"' AND date<='"+date2+"')";
    QString reqstr2 = " LIMIT 0,"+QString::number(nbTotal);

    // lancement du processus de requête
    if (m_db.open()) {

        QSqlQuery query;
        QString requete = "SELECT l.utilisateur, DATE_FORMAT(l.date, \'%d/%m/%Y à %H:%i:%s\') AS date, l.action, l.produit, l.quantite, u.ville, u.pays FROM logs l INNER JOIN utilisateurs u ON u.id = l.utilisateur"+reqstr+reqstr2;
        query.prepare(requete);
        query.bindValue(":value", elem);
        /*QTextEdit *fen = new QTextEdit(elem+"//////"+requete); fen->show();*/

        // execution de la requête
        if (query.exec()) {
            QSqlRecord record = query.record();
            int i=0, j;

            while (query.next()) {
                j=0;

                int etatString = query.value(record.indexOf("is_deleted")).toInt();
                QString userString = conversion(query.value(record.indexOf("utilisateur")).toInt(), "utilisateurs");
                QString dateString = query.value(record.indexOf("date")).toString();
                QString actionString = conversion(query.value(record.indexOf("action")).toInt(), "actions");
                QString produitString = conversion(query.value(record.indexOf("produit")).toInt(), "produit", etatString);
                QString quantiteString = query.value(record.indexOf("quantite")).toString();

                QStandardItem *itemUser = new QStandardItem;
                QStandardItem *itemDate = new QStandardItem;
                QStandardItem *itemAction = new QStandardItem;
                QStandardItem *itemProduit = new QStandardItem;
                QStandardItem *itemQuantite = new QStandardItem;

                itemUser->setText(userString);
                itemDate->setText(dateString);
                itemAction->setText(actionString);
                itemProduit->setText(produitString);
                itemQuantite->setText(quantiteString);
                model->setItem(i, j, itemUser);
                model->setItem(i, j+1, itemDate);
                model->setItem(i, j+2, itemAction);
                model->setItem(i, j+3, itemProduit);
                model->setItem(i, j+4, itemQuantite);

                i++;
            }
            m_db.close();
        }
    }
}

// fonction qui convertit un type sous forme int (classiquement obtenu par requete dans la table "produit" de la bdd) en sa forme string
QString BaseDeDonnees::conversion(int typeAConvert, QString idstr, int typeAction) {

    QString strtolook, strtolook2;
    QSqlQuery query(m_db);

    if (idstr == "type") {
        query.prepare("SELECT id, type FROM type WHERE id = (:typetoselect)");
        strtolook = "type";
    } else if (idstr == "actions") {
        query.prepare("SELECT id, nom FROM actions WHERE id = (:typetoselect)");
        strtolook = "nom";
    } else if (idstr == "produit") {
            query.prepare("SELECT id, produit FROM produit WHERE id = (:typetoselect)");
            strtolook = "produit";
    } else if (idstr == "utilisateurs") {
        query.prepare("SELECT id, nom, prenom FROM utilisateurs WHERE id = (:typetoselect)");
        strtolook = "nom";
        strtolook2 = "prenom";
    }
    query.bindValue(":typetoselect", typeAConvert);

    if (query.exec()) {
        while (query.next()) {
            if (idstr == "utilisateurs") {return query.value(query.record().indexOf(strtolook)).toString()+" "+query.value(query.record().indexOf(strtolook2)).toString();}
            return query.value(query.record().indexOf(strtolook)).toString();
        }
    } else {
        //  QMessageBox::critical(m_parent, "Sélection dans la BDD", "Problème!");
        return "pb";
    }
}


void BaseDeDonnees::reqUtilisateursHistorique(std::map< QString, std::list<QString> > &map) {

    if (m_db.open()) {
        QSqlQuery query;
        query.prepare("SELECT nom, prenom, departement, poste, pays, ville FROM utilisateurs");



        std::map< QString, std::list<QString> >::iterator it = map.begin();
        std::list<QString> list;
        map["pays"] = list;
        bool paysbool = false, villebool = false;

        if (query.exec()) {
            QSqlRecord record = query.record();
            while (query.next()) {

                // on récupère les infos sous forme de String dans la bdd
                QString paysStr = query.value(record.indexOf("pays")).toString();
                QString villeStr = query.value(record.indexOf("ville")).toString();
                QString nomStr = query.value(record.indexOf("nom")).toString()+" "+query.value(record.indexOf("prenom")).toString();

                //QMessageBox::information(0, "debug", paysStr+villeStr+nomStr);

                it = map.find("pays");

                // on regarde si le pays est déjà enregistré dans la liste des pays
                for (std::list<QString>::iterator itList = (it->second).begin(); itList != (it->second).end(); ++itList) {
                    if (*itList == paysStr) {
                        paysbool = true;
                    }
                }

                // s'il ne l'est pas, on crée le pays, la ville dans la map et on rentre la personne dans la liste de la ville
                if (!paysbool) {
                    (it->second).push_back(paysStr);
                    std::list<QString> list, listBis;
                    map[paysStr]=list;
                    it = map.find(paysStr); (it->second).push_back(villeStr);
                    map[villeStr]=listBis;
                    it = map.find(villeStr); (it->second).push_back(nomStr);
                } else {    // s'il y a le pays, on regarde si la ville est déjà enregistrée
                    it = map.find(paysStr);
                    for (std::list<QString>::iterator itList = (it->second).begin(); itList != (it->second).end(); ++itList) {
                        if (*itList == villeStr)
                            villebool = true;
                    }

                    // s'il n'y pas encore cette ville, on la rentre dans la map, puis on met dans la liste de la ville la personne
                    if (!villebool) {
                        (it->second).push_back(villeStr);
                        std::list<QString> list;
                        map[villeStr]=list;
                        it = map.find(villeStr); (it->second).push_back(nomStr);
                    } else {    // si la ville est connue alors on n'a plus qu'à rentrer la personne dans sa liste
                        it = map.find(villeStr);
                        (it->second).push_back(nomStr);
                        villebool = false;
                    }
                    paysbool = false;
                }
            }
        }        

        //QMessageBox::information(0, "debug", "blogque");
        m_db.close();
    }
}
