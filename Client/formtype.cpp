#include "formtype.h"
#include "ui_formtype.h"
#include "fengenerale.h"

FormType::FormType(FenGenerale *w, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormType)
{
    ui->setupUi(this);
    m_parent = w;
    this->setFixedSize(this->width(), this->height());
}

FormType::~FormType()
{
    delete ui;
}

void FormType::on_buttonBox_accepted()
{
    m_parent -> ajoutNewType(ui->lineEdit->text());
    this->close();
}

void FormType::on_buttonBox_rejected()
{
    this->close();
}
