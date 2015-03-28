#include "widgetaffichage.h"
#include "ui_widgetaffichage.h"
#include "tabresults.h"

WidgetAffichage::WidgetAffichage(QStandardItemModel* model, std::queue<QString> &queue, QString rappelReq, QString type, TabResults *parent):
    ui(new Ui::WidgetAffichage)
{
    ui->setupUi(this);
    ui->tableView->setModel(model);
    ui->tableView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    ui->labelReq->setText(rappelReq);

    ui->tableView->setAlternatingRowColors(true);
    m_type = type;

    if (type == "requete") {
        for (int i=0; i<7; i++) {QString str = queue.front(); m_queue.push(str); queue.pop();}
    } else if (type == "historique") {
        for (int i=0; i<5; i++) {QString str = queue.front(); m_queue.push(str); queue.pop();}
    }

    m_req = rappelReq;
    m_model = model;
    m_parent = parent;
}

WidgetAffichage::~WidgetAffichage()
{
    delete ui;
}

// event démasqué pour fermer
void WidgetAffichage::closeEvent(QCloseEvent *event)
{
    m_parent->buildNewOngletReq(m_model, m_queue, m_req);
    event->accept();
}

std::queue<QString>& WidgetAffichage::myQueue() {
    return m_queue;
}

QString WidgetAffichage::myReq() {
    return m_req;
}

QString WidgetAffichage::myType() {
    return m_type;
}

QStandardItemModel* WidgetAffichage::myModel() {
    return m_model;
}

void WidgetAffichage::changerModeleReq(QStandardItemModel *model, QString rappelReq, bool dateAjout, bool lastUpdate) {

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

    ui->tableView->setModel(model);
    m_req = rappelReq;
}

void WidgetAffichage::changerModeleHisto(QStandardItemModel *model, QString rappelReq) {
    ui->tableView->setModel(model);
    m_req = rappelReq;
}
