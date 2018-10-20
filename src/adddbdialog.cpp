#include "adddbdialog.h"
#include "ui_adddbdialog.h"
#include <QFileDialog>

AddDbDialog::AddDbDialog(QWidget *parent) : QDialog(parent), ui(new Ui::AddDbDialog){

    ui->setupUi(this);

    fileChecked = false;

    filesChecked = false;

}

void AddDbDialog::showEvent(QShowEvent *ev){

    QDialog::showEvent(ev);

    ui->nameEdit->setText(addonName);

    ui->filesEdit->setText(addonFiles);

    ui->pathLabel->hide();

    ui->folderRadio->setChecked(folderChecked);

    ui->fileRadio->setChecked(fileChecked);

    ui->filesRadio->setChecked(filesChecked);

    if(filesChecked) {

        ui->remCheck->setDisabled(true);

        ui->remCheck->setChecked(false);

    }
}

void AddDbDialog::on_folderButton_clicked(){

    QString path = QFileDialog::getExistingDirectory(this, "Select the add-on folder");

    if(path.isEmpty()) return;

    addonPath = path;

    if(ui->pathLabel->isHidden()) ui->pathLabel->show();

    ui->pathLabel->setText("Selected folder: " + path);
}

void AddDbDialog::on_fileButton_clicked(){

    QString path = QFileDialog::getOpenFileName(this, "Select the add-on file", "", "Zip file (*.zip)");

    if(path.isEmpty()) return;

    addonPath = path;

    if(ui->pathLabel->isHidden()) ui->pathLabel->show();

    ui->pathLabel->setText("Selected file: " + path);

}

void AddDbDialog::on_addButton_clicked(){

   addonName = ui->nameEdit->text();

   addonFiles = ui->filesEdit->text();

   folderChecked = ui->folderRadio->isChecked();

   fileChecked = ui->fileRadio->isChecked();

   filesChecked = ui->filesRadio->isChecked();

   removeDir = ui->remCheck->isChecked();

   check = true;

   close();
}

void AddDbDialog::on_cancelButton_clicked(){close();}

AddDbDialog::~AddDbDialog(){delete ui;}

void AddDbDialog::on_filesRadio_clicked(){ui->remCheck->setDisabled(true); ui->remCheck->setChecked(false);}

void AddDbDialog::on_fileRadio_clicked(){ui->remCheck->setEnabled(true); ui->remCheck->setChecked(true);}

void AddDbDialog::on_folderRadio_clicked(){ui->remCheck->setEnabled(true); ui->remCheck->setChecked(true);}
