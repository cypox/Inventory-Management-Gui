#include "fenaffichage.h"
#include "ui_fenaffichage.h"
#include "fengenerale.h"

fenAffichage::fenAffichage(QString type, QStandardItemModel* model, QString rappelReq, FenGenerale *gestionFen, std::queue<QString> &queue, FenGenerale *parent) :
    QMainWindow(parent),
    ui(new Ui::fenAffichage)
{
    ui->setupUi(this);
    m_compteurHisto = 0;
    m_compteurReq = 0;
    m_compteur = 0;
    m_parent = gestionFen;

    //setFixedWidth(737);

    /*ui->tableView->setModel(model);
    ui->tableView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    ui->labelReq->setText(rappelReq);*/

    if (type == "requete") {buildNewOngletReq(model, queue, rappelReq);} else if (type == "historique") {buildNewOngletHisto(model, queue, rappelReq);}
}

fenAffichage::~fenAffichage()
{
    delete ui;
}

void fenAffichage::buildNewOngletReq(QStandardItemModel *model, std::queue<QString> &queue, QString rappelReq) {

    if (m_compteur == 0) {ui->tabWidget->removeTab(0);}

    m_compteurReq += 1;
    m_compteur += 1;
    QWidget *onglet = new WidgetAffichage(model, queue, rappelReq, "requete");
    QString label = "Req" + QString::number(m_compteurReq);

    ui -> tabWidget -> addTab(onglet, label);
    ui -> tabWidget -> setCurrentWidget(onglet);

}

void fenAffichage::buildNewOngletHisto(QStandardItemModel *model, std::queue<QString> &queue, QString rappelReq) {

    m_compteurHisto += 1;
    m_compteur += 1;
    QWidget *onglet = new WidgetAffichage(model, queue, rappelReq, "historique");
    QString label = "Hist" + QString::number(m_compteurHisto);

    ui -> tabWidget -> addTab(onglet, label);
    ui -> tabWidget -> setCurrentWidget(onglet);

}

void fenAffichage::on_actionQuitter_la_fen_tre_triggered()
{
    m_parent -> setBoolFenAffichage(true);
    this -> close();
}

// event démasqué pour fermer, pour sauvegarder les paramètres
void fenAffichage::closeEvent(QCloseEvent *event)
{
    m_parent -> setBoolFenAffichage(true);
    event->accept();
}

void fenAffichage::on_actionFermer_l_onget_courant_triggered()
{
    int number = ui -> tabWidget -> count();
    if (number == 1) {
        on_actionQuitter_la_fen_tre_triggered();
    }
    int current = ui -> tabWidget -> currentIndex();
    ui -> tabWidget -> removeTab(current);
}

void fenAffichage::on_actionOnglet_suivant_triggered()
{
    int number = ui -> tabWidget -> count();
    int current = ui -> tabWidget -> currentIndex();
    ui -> tabWidget -> setCurrentIndex((current+1)%number);
}

void fenAffichage::on_actionOnglet_pr_c_dent_triggered()
{
    int number = ui -> tabWidget -> count();
    int current = ui -> tabWidget -> currentIndex();
    if (current != 0) {
        ui -> tabWidget -> setCurrentIndex((current-1));
    } else {
        ui -> tabWidget -> setCurrentIndex(number-1);
    }
}

void fenAffichage::on_actionImprimer_triggered()
{
    QPrinter *printer = new QPrinter();
    printer->setPageSize(QPrinter::A4);

    QPrintDialog *dialog = new QPrintDialog(printer);
    if (dialog->exec() == QDialog::Accepted) {
        QPainter Painter;
        Painter.begin(printer);
        Painter.setPen(Qt::blue);
        Painter.setFont(QFont("Arial", 30));
        Painter.drawText(QPoint(10,10), "Qt");
        Painter.end();
    }

}

void fenAffichage::on_actionA_propos_de_Qt_triggered()
{
    qApp->aboutQt();
}

void fenAffichage::on_actionActualiser_l_onglet_triggered()
{
    QWidget *onglet = ui->tabWidget->currentWidget();
    m_ongletEnReactualisation = ui->tabWidget->currentIndex();
    WidgetAffichage *ongletCourant = qobject_cast<WidgetAffichage *>(onglet);

    std::queue<QString> queue = ongletCourant->myQueue();

    QString type = queue.front(); queue.pop();
    QString produit = queue.front(); queue.pop();
    QString classement = queue.front(); queue.pop();
    int nbTotal = queue.front().toInt(); queue.pop();

    QString booleen;
    bool ordreDESC, dateAjout, lastUpdate;

    booleen = queue.front(); queue.pop();
    if (booleen == "true") {ordreDESC = true;} else {ordreDESC = false;}

    booleen = queue.front(); queue.pop();
    if (booleen == "true") {dateAjout = true;} else {dateAjout = false;}

    booleen = queue.front(); queue.pop();
    if (booleen == "true") {lastUpdate = true;} else {lastUpdate = false;}

    m_dateAjoutEnReactualisation = dateAjout;
    m_lastUpdateEnReactualisation = lastUpdate;

    QString rappelReq = ongletCourant->myReq();

    m_parent->mySocket()->reqAffichage(type, produit, classement, ordreDESC, nbTotal, dateAjout, lastUpdate, rappelReq, 2);
}

void fenAffichage::actualiserOngletReq(QStandardItemModel *model,QString rappelReq) {
    qobject_cast<WidgetAffichage *>(ui->tabWidget->widget(m_ongletEnReactualisation))->changerModele(model, rappelReq, m_dateAjoutEnReactualisation, m_lastUpdateEnReactualisation);
    ui -> tabWidget -> setCurrentIndex(m_ongletEnReactualisation);
}

void fenAffichage::on_actionD_placer_dans_une_nouvelle_fen_tre_triggered()
{
    QWidget *onglet = ui->tabWidget->currentWidget();
    WidgetAffichage *ongletCourant = qobject_cast<WidgetAffichage *>(onglet);

    WidgetAffichage *fen = new WidgetAffichage(ongletCourant->myModel(), ongletCourant->myQueue(), ongletCourant->myReq(), ongletCourant->myType());
    fen->show();

    this->on_actionFermer_l_onget_courant_triggered();
}
