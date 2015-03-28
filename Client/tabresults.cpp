#include "tabresults.h"
#include "ui_tabresults.h"
#include "fengenerale.h"

TabResults::TabResults(QString type, QStandardItemModel *model, QString rappelReq, FenGenerale *gestionFen, std::queue<QString> &queue, FenGenerale *parent) :
    QTabWidget(parent),
    ui(new Ui::TabResults)
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

TabResults::~TabResults()
{
    delete ui;
}

void TabResults::buildNewOngletReq(QStandardItemModel *model, std::queue<QString> &queue, QString rappelReq) {

    if (m_compteur == 0) {removeTab(0);}

    m_compteurReq += 1;
    m_compteur += 1;
    QWidget *onglet = new WidgetAffichage(model, queue, rappelReq, "requete");
    QString label = "Req" + QString::number(m_compteurReq);

    addTab(onglet, label);
    setCurrentWidget(onglet);

}

void TabResults::buildNewOngletHisto(QStandardItemModel *model, std::queue<QString> &queue, QString rappelReq) {

    if (m_compteur == 0) {removeTab(0);}

    m_compteurHisto += 1;
    m_compteur += 1;
    QWidget *onglet = new WidgetAffichage(model, queue, rappelReq, "historique");
    QString label = "Hist" + QString::number(m_compteurHisto);

    addTab(onglet, label);
    setCurrentWidget(onglet);

}

void TabResults::on_actionQuitter_la_fen_tre_triggered()
{
    m_parent -> setBoolFenAffichage(true);
    this -> close();
}

// event démasqué pour fermer
void TabResults::closeEvent(QCloseEvent *event)
{
    m_parent -> setBoolFenAffichage(true);
    event->accept();
}

void TabResults::on_actionFermer_l_onget_courant_triggered()
{
    int number = count();
    if (number == 1) {
        on_actionQuitter_la_fen_tre_triggered();
    }
    int current = currentIndex();
    removeTab(current);
}

void TabResults::on_actionOnglet_suivant_triggered()
{
    int number = count();
    int current = currentIndex();
    setCurrentIndex((current+1)%number);
}

void TabResults::on_actionOnglet_pr_c_dent_triggered()
{
    int number = count();
    int current = currentIndex();
    if (current != 0) {
        setCurrentIndex((current-1));
    } else {
        setCurrentIndex(number-1);
    }
}

/*
void TabResults::on_actionImprimer_triggered()
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

}*/

void TabResults::on_actionEnregistrer_les_r_sultats_triggered() {
    QPrinter *printer = new QPrinter(QPrinter::HighResolution);
    printer->setPaperSize (QPrinter::A4);
    printer->setOrientation(QPrinter::Portrait);

    QWidget *onglet = currentWidget();
    m_ongletEnReactualisation = currentIndex();
    WidgetAffichage *ongletCourant = qobject_cast<WidgetAffichage *>(onglet);

    QStandardItemModel *model = ongletCourant->myModel();

    bool b_First;
    QString qs_Header;
    QString qs_Row;
    qs_Row = "";
    qs_Header = "";
    b_First = true;

    for (int i=0; i<model->rowCount(); i++) {

        qs_Row = qs_Row+"<tr>";
        for (int j=0; j<model->columnCount(); j++) {
            if (b_First) {
                qs_Header = qs_Header+"<th>"+model->headerData(j, Qt::Horizontal, 0).toString()+"</th>";
            }
            qs_Row = qs_Row+"<td>"+model->index(i, j).data().toString()+"</td>";
        }

        b_First = false;
        qs_Row = qs_Row+"</tr>";
    }

    QString m_journal = "<html><body><center><br /><br /><h3>Rappel de la requête</h3><br /><br />"+ongletCourant->myReq()+"<br /><br /><br /><br /><h3>Résultats</h3><div class='tableimprim'><table class='table table-condensed table-bordered'><tbody><thead><tr><strong>" + qs_Header + "</strong></tr></thead>" + qs_Row + "</tbody></table></div></center></body></html>";
    printer->setFullPage(true);

    QWebView *journal_document = new QWebView;
    //journal_document->setDefaultFont(QFont("Arial", 10));
    journal_document->setHtml(m_journal);
    journal_document->settings()->setUserStyleSheetUrl(QUrl::fromLocalFile("bootstrap.css"));
    printer->setOutputFormat(QPrinter::NativeFormat);

    QString fichier = QFileDialog::getSaveFileName(this, "Enregistrer un fichier", "D:/sauve.pdf", "pdf (*.pdf)");

    printer->setOutputFileName(fichier);
    journal_document->print(printer);
}

void TabResults::on_actionImprimer_triggered() {

    QPrinter *printer = new QPrinter(QPrinter::HighResolution);
    printer->setPaperSize (QPrinter::A4);
    printer->setOrientation(QPrinter::Portrait);

    QWidget *onglet = currentWidget();
    m_ongletEnReactualisation = currentIndex();
    WidgetAffichage *ongletCourant = qobject_cast<WidgetAffichage *>(onglet);

    QStandardItemModel *model = ongletCourant->myModel();

    bool b_First;
    QString qs_Header;
    QString qs_Row;
    qs_Row = "";
    qs_Header = "";
    b_First = true;

    for (int i=0; i<model->rowCount(); i++) {

        qs_Row = qs_Row+"<tr>";
        for (int j=0; j<model->columnCount(); j++) {
            if (b_First) {
                qs_Header = qs_Header+"<th>"+model->headerData(j, Qt::Horizontal, 0).toString()+"</th>";
            }
            qs_Row = qs_Row+"<td>"+model->index(i, j).data().toString()+"</td>";
        }

        b_First = false;
        qs_Row = qs_Row+"</tr>";
    }

    QString m_journal = "<html><body><center><br /><br /><h3>Rappel de la requête</h3><br /><br />"+ongletCourant->myReq()+"<br /><br /><br /><br /><h3>Résultats</h3><div class='tableimprim'><table class='table table-condensed table-bordered'><tbody><thead><tr><strong>" + qs_Header + "</strong></tr></thead>" + qs_Row + "</tbody></table></div></center></body></html>";
    printer->setFullPage(true);

    QWebView *journal_document = new QWebView;
    //journal_document->setDefaultFont(QFont("Arial", 10));
    journal_document->setHtml(m_journal);
    journal_document->settings()->setUserStyleSheetUrl(QUrl::fromLocalFile("bootstrap.css"));
    printer->setOutputFormat(QPrinter::NativeFormat);

    //QString PATH = "D:/";
    //printer->setOutputFileName(PATH +"MonNomDeFichier.pdf");

    QPrintDialog *dialog = new QPrintDialog(printer);
    if (dialog->exec() == QDialog::Accepted) {
        journal_document->print(printer);
    }
}


void TabResults::on_actionA_propos_de_Qt_triggered()
{
    qApp->aboutQt();
}

void TabResults::on_actionActualiser_l_onglet_triggered()
{
    QWidget *onglet = currentWidget();
    m_ongletEnReactualisation = currentIndex();
    WidgetAffichage *ongletCourant = qobject_cast<WidgetAffichage *>(onglet);

    QString type = ongletCourant->myType();
    std::queue<QString> queue = ongletCourant->myQueue();

    if (type == "requete") {

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

    } else if (type == "historique") {
        QString elem = queue.front(); queue.pop();
        QString type = queue.front(); queue.pop();
        QString date1 = queue.front(); queue.pop();
        QString date2 = queue.front(); queue.pop();
        int nbTotal = queue.front().toInt(); queue.pop();

        QString rappelReq = ongletCourant->myReq();
        m_parent->mySocket()->reqHisto(elem, type, date1, date2, nbTotal, rappelReq, 2);
    }
}

void TabResults::actualiserOngletReq(QStandardItemModel *model,QString rappelReq) {
    qobject_cast<WidgetAffichage *>(widget(m_ongletEnReactualisation))->changerModeleReq(model, rappelReq, m_dateAjoutEnReactualisation, m_lastUpdateEnReactualisation);
    setCurrentIndex(m_ongletEnReactualisation);
}

void TabResults::actualiserOngletHisto(QStandardItemModel *model,QString rappelReq) {
    qobject_cast<WidgetAffichage *>(widget(m_ongletEnReactualisation))->changerModeleHisto(model, rappelReq);
    setCurrentIndex(m_ongletEnReactualisation);
}

void TabResults::on_actionD_placer_dans_une_nouvelle_fen_tre_triggered()
{
    QWidget *onglet = currentWidget();
    WidgetAffichage *ongletCourant = qobject_cast<WidgetAffichage *>(onglet);

    WidgetAffichage *fen = new WidgetAffichage(ongletCourant->myModel(), ongletCourant->myQueue(), ongletCourant->myReq(), ongletCourant->myType(), this);
    fen->show();

    this->on_actionFermer_l_onget_courant_triggered();
}
