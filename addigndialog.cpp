#include "addigndialog.h"
#include "ui_addigndialog.h"

AddIgnDialog::AddIgnDialog(QWidget *parent) : QDialog(parent), ui(new Ui::AddIgnDialog){ui->setupUi(this);}

void AddIgnDialog::on_addButton_clicked(){addonName = ui->nameEdit->text(); check = true; close();}

void AddIgnDialog::on_cancelButton_clicked(){close();}

AddIgnDialog::~AddIgnDialog(){delete ui;}
