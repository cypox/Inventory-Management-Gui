#ifndef WIDGETAFFICHAGE_H
#define WIDGETAFFICHAGE_H

#include <QWidget>
#include <QMessageBox>
#include <QCloseEvent>
#include <QStandardItemModel>
#include <queue>

namespace Ui {
class WidgetAffichage;
}

class TabResults;

class WidgetAffichage : public QWidget
{
    Q_OBJECT
    
public:
    explicit WidgetAffichage(QStandardItemModel *model, std::queue<QString> &queue, QString rappelReq, QString type, TabResults *parent = 0);
    ~WidgetAffichage();

    std::queue<QString>& myQueue();
    QString myReq();
    QStandardItemModel* myModel();
    QString myType();

    void changerModeleReq(QStandardItemModel *model, QString rappelReq, bool dateAjout, bool lastUpdate);
    void changerModeleHisto(QStandardItemModel *model, QString rappelReq);
    
private:
    Ui::WidgetAffichage *ui;
    std::queue<QString> m_queue;
    QString m_req;
    QStandardItemModel *m_model;
    QString m_type;
    TabResults *m_parent;

protected:
    void closeEvent(QCloseEvent *event);
};

#endif // WIDGETAFFICHAGE_H
