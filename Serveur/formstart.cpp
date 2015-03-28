#include "formstart.h"
#include "ui_formstart.h"

FormStart::FormStart(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormStart)
{
    ui->setupUi(this);
    setFixedHeight(this->height());
    setFixedWidth(this->width());

    // settings
    QSettings settings("settings.ini", QSettings::IniFormat);

    hostname = settings.value("hostname").toString();
    login = settings.value("login").toString();
    password = settings.value("password").toString();

    ui->hostname->setText(hostname);
    ui->login->setText(login);
    ui->password->setText(password);
}

FormStart::~FormStart()
{
    delete ui;
}

void FormStart::on_pushButton_clicked()
{
    hostname = ui->hostname->text();
    login = ui->login->text();
    password = ui->password->text();

    FenServeur *fen = new FenServeur(login, password, hostname);
    fen->show();

    // enregistrement settings
    if (ui->retainCheck->isChecked()) {
        QSettings settings("settings.ini", QSettings::IniFormat);
        settings.setValue("hostname", hostname);
        settings.setValue("login", login);
        settings.setValue("password", password);
    }

    this->close();
}

void FormStart::on_pushButton_2_clicked()
{

    // enregistrement settings
    if (ui->retainCheck->isChecked()) {
        QSettings settings("settings.ini", QSettings::IniFormat);
        settings.setValue("hostname", hostname);
        settings.setValue("login", login);
        settings.setValue("password", password);
    }
    this->close();
}
