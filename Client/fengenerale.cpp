#include "fengenerale.h"
#include "ui_fengenerale.h"

FenGenerale::FenGenerale(QString address, int port, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FenGenerale)
{
    ui->setupUi(this);
    this->showMaximized();

    ui->dockWidgetContents_3->setFixedWidth(415);
    ui->dockWidgetContents_4->setFixedWidth(415);

    // définitions des attributs
    m_index = 0;
    m_sousIndex = 0;
    m_action = "";
    m_infobdd1 = "null";
    m_infobdd2 = "null";
    m_infobddnb1 = -1;    

    m_file.push("blue");
    m_file.push("red");

    // construction du socket qui fera le lien réseau avec le serveur tcp
    m_socket = new interSocketClient(address, port, this);

    connect(ui->comboBoxtype, SIGNAL(activated(QString)), this, SLOT(setBddInfo2(QString)));
    connect(this, SIGNAL(m_fenAffichBoolChanged(bool)), this, SLOT(m_fenAffichBoolConsequence(bool)));

    m_fenaffichbool = true;
    emit m_fenAffichBoolChanged(true);
}

FenGenerale::~FenGenerale()
{
    delete ui;
}

// accesseur aux paramètres locaux qui permettent d'enregistrer les infos à mettre dans la bdd
void FenGenerale::setBoolFenAffichage (bool val) {
    m_fenaffichbool = val;
    emit m_fenAffichBoolChanged(true);
    ui->widgetFullGrey->setVisible(true);
}

void FenGenerale::m_fenAffichBoolConsequence(bool val) {
    ui->actionActualiser_l_onglet->setEnabled(!val);
    ui->actionD_placer_dans_une_nouvelle_fen_tre->setEnabled(!val);
    ui->actionEnregistrer_les_r_sultats->setEnabled(!val);
    ui->actionFermer_l_onget_courant->setEnabled(!val);
    ui->actionImprimer->setEnabled(!val);
    ui->actionOnglet_pr_c_dent->setEnabled(!val);
    ui->actionOnglet_suivant->setEnabled(!val);
}

void FenGenerale::setBddInfo1 (QString info) {
    m_infobdd1 = info;
    ui->label_14->setText(info);
}

void FenGenerale::setBddInfo2 (QString info) {
    m_infobdd2 = info;
}

void FenGenerale::setBddInfoNb1 (int info) {
    m_infobddnb1 = info;
}

interSocketClient* FenGenerale::mySocket() {
    return this->m_socket;
}

TabResults* FenGenerale::myTab() {
    return this->m_fenAffichage;
}

// slot pour quitter l'app
void FenGenerale::on_actionQuitter_la_fen_tre_triggered()
{
    this->close();
}
void FenGenerale::on_actionA_propos_de_Qt_triggered()
{
    qApp->aboutQt();
}

// actions / requetes à lancer lorsqu'on change d'onglet sur la fenêtre
void FenGenerale::on_tabWidget_selected(const QString &arg1)
{

    if (arg1 == "Affichage") {
        deleteItemsComboBox(ui->comboBoxAffichageProduit);
        deleteItemsComboBox(ui->comboBoxAffichageType);
        m_socket -> formAffiche(1, "none");
    } else if (arg1 == "Historique") {
        m_socket -> formHisto();
        ui->dateEdit_2->setDate(QDate::currentDate());
    }

}

void FenGenerale::appendMessage(QString mess) {
}


// #################################    Onglet Accueil  #######################################################

void FenGenerale::buildInfosPersos(std::map<QString, QVariant> &infosPersos) {

    QString nom;

    std::map<QString, QVariant>::iterator trouve = infosPersos.find("nom");
    ui->nomLineEdit->setText((trouve->second).toString());
    nom += (trouve->second).toString();
    nom += " ";

    trouve = infosPersos.find("prenom");
    ui->prNomLineEdit->setText((trouve->second).toString());
    nom += (trouve->second).toString();

    ui->idLabel->setText(nom);

    trouve = infosPersos.find("login");
    ui->loginLineEdit->setText((trouve->second).toString());

    trouve = infosPersos.find("password");
    ui->passwordLineEdit->setText((trouve->second).toString());

    trouve = infosPersos.find("departement");
    ui->dPartementLineEdit->setText((trouve->second).toString());

    trouve = infosPersos.find("poste");
    ui->posteLineEdit->setText((trouve->second).toString());

    trouve = infosPersos.find("ville");
    ui->villeLineEdit->setText((trouve->second).toString());

    trouve = infosPersos.find("anciennete");
    ui->anciennetSpinBox->setValue((trouve->second).toInt());

    /*for(std::map<QString, QVariant>::iterator it=infosPersos.begin(); it!=infosPersos.end(); ++it) {
        ui->textEdit->append(it->first+ " = " +(it->second).toString());
    }*/
}



// #################################    Onglet Actions  #######################################################


// Lance le QWidget qui permet l'ajout d'un type
void FenGenerale::on_ajoutTypePushButton_clicked()
{
    FormType *w = new FormType(this);
    w->show();
}

// lance l'ajout du nouveau type
void FenGenerale::ajoutNewType(QString type) {
    //QMessageBox::information(0, "debug", type);
    m_socket->modifierProduit("ajout_type", "null", type, 0);
}

// slot lancés par un changement de la valeur de la combo box de choix d'action
void FenGenerale::on_comboBox_activated(QString action)
{
    if (action == "- action -") {
        action = "Détails";
    }
    ui->detailsGroupBox->setTitle(action);

    // clean bddinfo2 où est stocké l'information de type à chaque fois qu'on change d'action
    setBddInfo2("null");
}

// changement de type
void FenGenerale::on_comboBox_activated(int index)
{
    m_index = index;

    // clean les éléments de la fenêtre
    deleteItemsComboBox(ui->comboBoxtype);
    ui->listWidgetProducts->clear();
    ui->nombreProduit->setValue(0);
    ui->nomProduit->clear();
    ui->detailsAjout->setEnabled(false);

    // contacte le client qui va faire la requete sql
    m_socket -> formAction(index);
    ui->detailsGroupBox->setEnabled(true);

    if (index == 0) {
        ui -> detailsGroupBox -> setEnabled(false);
    }
}

void FenGenerale::on_comboBoxtype_activated(QString typeChoisi)
{
    if (typeChoisi == "- type -") {
        ui->listWidgetProducts->clear();
        ui->nombreProduit->setValue(0);
        ui->nomProduit->clear();
        ui->detailsAjout->setEnabled(false);
    } else {
    // contacte le client qui va faire la requete sql
        ui->listWidgetProducts->clear();
        ui->nombreProduit->setValue(0);
        ui->nomProduit->clear();
        ui->detailsAjout->setEnabled(false);
        m_socket -> formActionFurther(m_index, typeChoisi);
    }
}

// étape intermédiaire pour le formulaire d'action
void FenGenerale::buildDetails(std::queue<QString> &queue) {

    while (!queue.empty()) {
        ui->comboBoxtype ->addItem(queue.front());
        queue.pop();
    }

    if (ui->comboBox->currentIndex() == 1) { ui->ajoutTypePushButton->setEnabled(true);} else {ui->ajoutTypePushButton->setEnabled(false);}
    ui -> comboBoxtype -> setEnabled(true);
    ui -> comboBoxTypeLabel -> setEnabled(true);
}

// appelé lors de l'ajout d'un produit, vérifie que le nom n'est pas déjà présent dans la base (tout au moins dans le type de produits sélectionnés)
bool FenGenerale::checkExistence(QString info) {
    for (int i=0; i<ui->listWidgetProducts->count(); ++i) {
        if (ui->listWidgetProducts->item(i)->text() == info)
            return false;
    }
    return true;
}

// construit le layout correspondant à l'ajout d'un produit dans la bdd
void FenGenerale::buildAjoutProduit(std::queue<QString> &queue) {

    ui->detailsGroupBox->setEnabled(true);
    ui->detailsAjout->setEnabled(true);
    ui->nomProduit->setEnabled(true);

    while (!queue.empty()) {
        ui->listWidgetProducts->addItem(queue.front());
        queue.pop();
    }

    ui->listWidgetProducts->setSelectionMode(QAbstractItemView::NoSelection);

    connect(ui->nomProduit, SIGNAL(textChanged(QString)), this, SLOT(setBddInfo1(QString)));
    connect(ui->nombreProduit, SIGNAL(valueChanged(int)), this, SLOT(setBddInfoNb1(int)));
}

// construit le layout correspondant à la mise à jour d'un produit d'un produit dans la bdd
void FenGenerale::buildUpdateProduit(std::queue<QString> &queue) {

    ui->detailsGroupBox->setEnabled(true);
    ui->detailsAjout->setEnabled(false);

    while (!queue.empty()) {
        ui->listWidgetProducts->addItem(queue.front());
        queue.pop();
    }

    ui->listWidgetProducts->setSelectionMode(QAbstractItemView::SingleSelection);

    connect(ui->listWidgetProducts, SIGNAL(currentTextChanged(QString)), this, SLOT(setBddInfo1(QString)));
    connect(ui->nombreProduit, SIGNAL(valueChanged(int)), this, SLOT(setBddInfoNb1(int)));
}

// construit le layout correspondant à la suppression d'un produit dans la bdd
void FenGenerale::buildDeleteProduit(std::queue<QString> &queue) {

    ui->detailsGroupBox->setEnabled(true);
    ui->detailsAjout->setEnabled(false);

    while (!queue.empty()) {
        ui->listWidgetProducts->addItem(queue.front());
        queue.pop();
    }

    ui->listWidgetProducts->setSelectionMode(QAbstractItemView::SingleSelection);

    connect(ui->listWidgetProducts, SIGNAL(currentTextChanged(QString)), this, SLOT(setBddInfo1(QString)));
}

// slot lancé à l'appui sur le bouton OK
void FenGenerale::on_ButtonOkAction_clicked()
{
    switch (m_index) {
    case 0:
        break;

    case 1:
        if (checkExistence(m_infobdd1)) {
            if (m_infobdd1 != "null" && m_infobdd1 != "" && m_infobdd2 != "null" && m_infobddnb1 != -1) {
                m_socket->modifierProduit("ajout", m_infobdd1, m_infobdd2, m_infobddnb1);

                // remise à zéro des paramètres pour éviter un double-clic
                ui->nomProduit->clear();
                m_infobdd1 = "null";
                m_infobdd2 = "null";
                m_infobddnb1 = -1;
                ui->comboBox->setCurrentIndex(0);
            } else { QMessageBox::critical(this, "Ajout d'un produit dans la BDD", "Veuillez renseigner tous les champs");}
        } else {QMessageBox::critical(this, "Ajout d'un produit dans la BDD", "Le produit semble déjà présent dans la BDD, vérifiez le nom donné");}
        break;

    case 2:
        if (m_infobdd1 != "null") {
            m_socket->modifierProduit("suppression", m_infobdd1);

            // remise à zéro des paramÃ¨tres pour éviter un double-clic
            m_infobdd1 = "null";
            ui->comboBox->setCurrentIndex(0);
        } else {
            QMessageBox::critical(this, "Suppression d'un produit dans la BDD", "Veuillez renseigner tous les champs");
        }
        break;

    case 3:
        if (m_infobdd1 != "null" && m_infobddnb1 != -1) {
            m_socket->modifierProduit("rajouter", m_infobdd1, "null", m_infobddnb1);

            // remise à zéro des paramÃ¨tres pour éviter un double-clic
            m_infobdd1 = "null";
            m_infobddnb1 = -1;
            ui->comboBox->setCurrentIndex(0);
        } else {
            QMessageBox::critical(this, "Rajout d'un produit dans la BDD", "Veuillez renseigner tous les champs");
        }
        break;

    case 4:
        if (m_infobdd1 != "null" && m_infobddnb1 != -1) {
            m_socket->modifierProduit("enlever", m_infobdd1, "null", m_infobddnb1);

            // remise à zéro des paramÃ¨tres pour éviter un double-clic
            m_infobdd1 = "null";
            m_infobddnb1 = -1;
            ui->comboBox->setCurrentIndex(0);
        } else {
            QMessageBox::critical(this, "Retrait d'un produit dans la BDD", "Veuillez renseigner tous les champs");
        }
        break;
    }
}

// #################################    Onglet affichage   #######################################################

void FenGenerale::buildComboBoxAffichageType(std::queue<QString> &queue) {

    while (!queue.empty()) {
        ui->comboBoxAffichageType ->addItem(queue.front());
        queue.pop();
    }
}

void FenGenerale::on_comboBoxAffichageType_activated(const QString &typeChoisi) {

    deleteItemsComboBox(ui->comboBoxAffichageProduit);

    // contacte le client qui va faire la requete sql
    m_socket -> formAffiche(2, typeChoisi);
}

void FenGenerale::buildComboBoxAffichageProduit(std::queue<QString> &queue) {

    while (!queue.empty()) {
        ui->comboBoxAffichageProduit ->addItem(queue.front());
        queue.pop();
    }
}

void FenGenerale::on_comboBoxAffichageProduit_currentIndexChanged(int index)
{
    if (index != 0) {
        ui->classementRadioBox->setEnabled(true);
    } else {
        ui->radioButtonClassement1->setChecked(true);
        ui->classementRadioBox->setChecked(false);
        ui->classementRadioBox->setEnabled(false);
        ui->radioButtonOrdre2->setChecked(true);
        ui->ordreRadioBox->setEnabled(false);
    }

    connect(ui->classementRadioBox, SIGNAL(clicked(bool)), ui->ordreRadioBox, SLOT(setEnabled(bool)));
}

void FenGenerale::on_ButtonOkAffichage_clicked()
{
    QString type = ui -> comboBoxAffichageType -> currentText();
    QString produit = ui -> comboBoxAffichageProduit -> currentText();
    int nbTotal = ui -> nbTotalResultsSpinBox -> value();
    bool dateAjout = ui->checkBoxDateAjout->isChecked();
    bool lastUpdate = ui->checkBoxUpdate->isChecked();

    // on écrit le string de rappel de requête qui apparait dans la fenêtre d'affichage
    QString rappelReq = "<p><b>Type:</b> "+type+"  -  <b>Produit:</b> "+produit+"  -  <b>Classement par:</b> ";
    QString classement;
    bool ordreDESC;

    if (ui->classementRadioBox->isChecked()) {
        if (ui->radioButtonClassement1->isChecked()) {classement = "last_update_date"; rappelReq += "Date de dernière MàJ";}
        else if (ui->radioButtonClassement2->isChecked()) {classement = "stock"; rappelReq += "Quantité";}
        else if (ui->radioButtonClassement3->isChecked()) {classement = "type"; rappelReq += "Type";}
        else if (ui->radioButtonClassement4->isChecked()) {classement = "produit"; rappelReq += "Ordre alphabétique";}
        else if (ui->radioButtonClassement5->isChecked()) {classement = "ajout_date"; rappelReq += "Date d'ajout";}

        if (ui->radioButtonOrdre1->isChecked()) {ordreDESC = false; rappelReq += "  -  <b>Ordre:</b> Croissant  <br />  <b>Nombre de résultats affichés:</b> ";}
        else {ordreDESC = true; rappelReq += "  -  <b>Ordre:</b> Décroissant  <br />  <b>Nombre de résultats affichés:</b> ";}
    } else {
        classement = "last_update_date"; ordreDESC = true; rappelReq += "Date de dernière MàJ  -  <b>Ordre:</b> Décroissant  <br />  <b>Nombre de résultats affichés:</b> ";
    }
    rappelReq += QString::number(nbTotal)+"</p>";

    // on remplit la queue qu'on va transmettre à la fenêtre d'affichage, pour qu'elle puisse avoir les infos de requête si elle veut réactualiser
    m_queueAffichage.push(type);
    m_queueAffichage.push(produit);
    m_queueAffichage.push(classement);
    m_queueAffichage.push(QString::number(nbTotal));
    if (ordreDESC) {m_queueAffichage.push("true");} else {m_queueAffichage.push("false");}
    if (dateAjout) {m_queueAffichage.push("true");} else {m_queueAffichage.push("false");}
    if (lastUpdate) {m_queueAffichage.push("true");} else {m_queueAffichage.push("false");}


    // on envoie la requête
    if (type != "- type -" && produit != "- produit -") {
        m_socket -> reqAffichage(type, produit, classement, ordreDESC, nbTotal, dateAjout, lastUpdate, rappelReq, 1);
    } else {
        QMessageBox::critical(this, "Retrait d'un produit dans la BDD", "Veuillez renseigner tous les champs");
    }
}

void FenGenerale::buildResultsFen(QStandardItemModel* model, QString rappelReq, int id) {

    switch (id) {
    case 1: // construction d'un nouvel onglet
    {
        bool dateAjout = ui->checkBoxDateAjout->isChecked();
        bool lastUpdate = ui->checkBoxUpdate->isChecked();

        // on règle le header de notre modèle en fonction des options d'affichage cochés
        model->setHorizontalHeaderLabels(QStringList() << "Produit" << "Type" << "Stock");
        bool bothPossible = false;
        if (dateAjout) {
            model->setHorizontalHeaderLabels(QStringList() << "Produit" << "Type" << "Stock" << "Date d'ajout");
            bothPossible = true;
        }
        if (lastUpdate) {
            model->setHorizontalHeaderLabels(QStringList() << "Produit" << "Type" << "Stock" << "Dernière MàJ");
            if (bothPossible) {model->setHorizontalHeaderLabels(QStringList() << "Produit" << "Type" << "Stock" << "Date d'ajout" << "Dernière MàJ");}
        }

        if (m_fenaffichbool == true) {
            m_fenAffichage = new TabResults("requete", model, rappelReq, this, m_queueAffichage);
            ui->widgetFullGrey->setVisible(false);
            ui->layoutResults->addWidget(m_fenAffichage);
            m_fenaffichbool = false;
            emit m_fenAffichBoolChanged(false);
        } else {
            m_fenAffichage -> buildNewOngletReq(model, m_queueAffichage, rappelReq);
        }
    }
    break;

    case 2: // actualisation d'un onglet existant
        m_fenAffichage -> actualiserOngletReq(model, rappelReq);
        break;
    }
}



// #################################    Onglet historique   #######################################################

void FenGenerale::buildHistoUtilisateurs(QStandardItemModel* model) {
    ui->treeViewUsers->setModel(model);
}

void FenGenerale::on_buttonOkHistorique_clicked()
{
    // on détermine l'élément sélectionné
    QItemSelectionModel *selection = ui->treeViewUsers->selectionModel();
    QModelIndex indexSelected = selection->currentIndex();
    QVariant elementSelected = ui->treeViewUsers->model()->data(indexSelected, Qt::DisplayRole);

    // on détermine la profondeur de l'objet sélectionné dans l'arbre
    int c=0;
    while ((ui->treeViewUsers->model()->data(indexSelected, Qt::DisplayRole) != "Classement par pays") && (ui->treeViewUsers->model()->data(indexSelected, Qt::DisplayRole) != "Classement par département") && (ui->treeViewUsers->model()->data(indexSelected, Qt::DisplayRole) != "Tous")) {
        indexSelected = indexSelected.parent();
        c++;
    }

    // on stocke les infos à envoyer au serveur pour sa requete
    QString elem = elementSelected.toString();
    QString date1 = QString::number(ui->dateEdit->date().year())+"-"+QString::number(ui->dateEdit->date().month())+"-"+QString::number(ui->dateEdit->date().day())+" 00:00:00";
    QString date2 = QString::number(ui->dateEdit_2->date().year())+"-"+QString::number(ui->dateEdit_2->date().month())+"-"+QString::number(ui->dateEdit_2->date().day())+ " 23:59:59";

    QString type, rappelReq ="<p><b> Actions réalisées</b>";
    int nbTotal = ui->nbTotalResultsSpinBox_2->value();

    // en fonction de la profondeur, on détermine le type de l'objet : pays/ville/utilisateur
    switch (c) {
    case 0:
        return;
        break;

    case 1:
        type = "pays";
        rappelReq += " en "+elem;
        break;

    case 2:
        type = "ville";
        rappelReq += " à "+elem;
        break;

    case 3:
        type = "utilisateur";
        rappelReq += " par "+elem;
        break;
    }
    rappelReq += " <b>entre le </b> "+ui->dateEdit->text()+" <b>et le </b>"+ui->dateEdit_2->text();
    rappelReq += "<br /><b>Nombre de résultats affichés:</b> "+ QString::number(nbTotal)+"</p>";

    // on remplit la queue qu'on va transmettre à la fenêtre d'affichage, pour qu'elle puisse avoir les infos de requête si elle veut réactualiser
    m_queueHisto.push(elem);
    m_queueHisto.push(type);
    m_queueHisto.push(date1);
    m_queueHisto.push(date2);
    m_queueHisto.push(QString::number(nbTotal));

    // on envoie la requête
    if (c != 0) {
        m_socket -> reqHisto(elem, type, date1, date2, nbTotal, rappelReq, 1);
    } else {
        QMessageBox::critical(this, "Affichage d'un historique", "Veuillez renseigner tous les champs");
    }
}

void FenGenerale::buildResultsFenHisto(QStandardItemModel* model, QString rappelReq, int id) {

    switch (id) {
    case 1: // construction d'un nouvel onglet
        model->setHorizontalHeaderLabels(QStringList() << "Utilisateur" << "Date" << "Action" << "Produit" << "Quantité");
    {
        if (m_fenaffichbool == true) {
            m_fenAffichage = new TabResults("historique", model, rappelReq, this, m_queueHisto);
            ui->widgetFullGrey->setVisible(false);
            ui->layoutResults->addWidget(m_fenAffichage);
            m_fenaffichbool = false;
            emit m_fenAffichBoolChanged(false);
        } else {
            m_fenAffichage -> buildNewOngletHisto(model, m_queueHisto, rappelReq);
        }
    }
    break;

    case 2: // actualisation d'un onglet existant
        model->setHorizontalHeaderLabels(QStringList() << "Utilisateur" << "Date" << "Action" << "Produit" << "Quantité");
        m_fenAffichage -> actualiserOngletHisto(model, rappelReq);
        break;
    }
}

// #################################    Fonctions annexes  #######################################################

// supprime les widgets du layout detailsLayoutFurther
/*void FenGenerale::deleteWidgetsLayout() {

        QLayoutItem* item;
        while ( ( item = ui->detailsLayoutFurther->layout()->takeAt( 0 ) ) != NULL )
        {
            delete item->widget();
            delete item;
        }
        ui->detailsLayoutFurther->setContentsMargins(0, 0, 0, 0);
}*/

// supprime les items d'une combo box, sauf le premier
void FenGenerale::deleteItemsComboBox(QComboBox *box) {
    while (box->count()>1) {
        box->removeItem(1);
    }
}

void FenGenerale::on_comboBox_currentIndexChanged(int index)
{
    if (index == 0) {
        on_comboBox_activated(index);
    }
}

// ############ slots appelant ceux du tabWidget d'affichage ############

void FenGenerale::on_actionOnglet_suivant_triggered()
{
    m_fenAffichage->on_actionOnglet_suivant_triggered();
}

void FenGenerale::on_actionOnglet_pr_c_dent_triggered()
{
    m_fenAffichage->on_actionOnglet_pr_c_dent_triggered();
}

void FenGenerale::on_actionActualiser_l_onglet_triggered()
{
    m_fenAffichage->on_actionActualiser_l_onglet_triggered();
}

void FenGenerale::on_actionFermer_l_onget_courant_triggered()
{
    m_fenAffichage->on_actionFermer_l_onget_courant_triggered();
}

void FenGenerale::on_actionD_placer_dans_une_nouvelle_fen_tre_triggered()
{
    m_fenAffichage->on_actionD_placer_dans_une_nouvelle_fen_tre_triggered();
}

void FenGenerale::on_actionImprimer_triggered()
{
    m_fenAffichage->on_actionImprimer_triggered();
}

void FenGenerale::on_actionEnregistrer_les_r_sultats_triggered()
{
    m_fenAffichage->on_actionEnregistrer_les_r_sultats_triggered();
}
