#ifndef FORMTYPE_H
#define FORMTYPE_H

#include <QWidget>

class FenGenerale;

namespace Ui {
class FormType;
}

class FormType : public QWidget
{
    Q_OBJECT
    
public:
    explicit FormType(FenGenerale *w, QWidget *parent = 0);
    ~FormType();
    
private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::FormType *ui;
    FenGenerale *m_parent;
};

#endif // FORMTYPE_H
