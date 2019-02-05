#include "opsdialog.h"
#include "ui_opsdialog.h"
#include <QKeyEvent>

OpsDialog::OpsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OpsDialog)
{
    ui->setupUi(this);
}

OpsDialog::OpsDialog(QWidget *parent, QString title, QString label) :
    QDialog(parent),
    ui(new Ui::OpsDialog)
{
    ui->setupUi(this);
    setWindowTitle(title);
    ui->label->setText(label);
}

void OpsDialog::reject()
{
    return;
}

OpsDialog::~OpsDialog()
{
    delete ui;
}
