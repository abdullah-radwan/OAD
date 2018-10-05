#include "adddbdialog.h"
#include "ui_adddbdialog.h"
#include <QFileDialog>

AddDbDialog::AddDbDialog(QWidget *parent) : QDialog(parent), ui(new Ui::AddDbDialog){ui->setupUi(this);}

void AddDbDialog::showEvent(QShowEvent *ev){

    QDialog::showEvent(ev);

    ui->nameEdit->setText(addonName);

    ui->filesEdit->setText(addonFiles);

    ui->folderLabel->hide();

    ui->filesRadio->setChecked(filesChecked);
}

void AddDbDialog::on_selectButton_clicked(){

    QString path = QFileDialog::getExistingDirectory(this, "Select the add-on folder");

    if(path.isEmpty()) return;

    addonPath = path;

    if(ui->folderLabel->isHidden()) ui->folderLabel->show();

    ui->folderLabel->setText("Selected folder: " + path);
}

void AddDbDialog::on_addButton_clicked(){

   addonName = ui->nameEdit->text();

   addonFiles = ui->filesEdit->text();

   filesChecked = ui->filesRadio->isChecked();

   check = true;

   close();
}

void AddDbDialog::on_cancelButton_clicked(){close();}

AddDbDialog::~AddDbDialog(){delete ui;}
