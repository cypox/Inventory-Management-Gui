#ifndef FORMSTART_H
#define FORMSTART_H

#include <QWidget>
#include "fenserveur.h"
#include <QSettings>

namespace Ui {
class FormStart;
}

class FormStart : public QWidget
{
    Q_OBJECT
    
public:
    explicit FormStart(QWidget *parent = 0);
    ~FormStart();
    
private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::FormStart *ui;
    QString hostname, login, password;
};

#endif // FORMSTART_H
