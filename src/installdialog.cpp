#include "installdialog.h"
#include "ui_installdialog.h"
#include <QFileDialog>

InstallDialog::InstallDialog(QWidget *parent) : QDialog(parent), ui(new Ui::InstallDialog){

    ui->setupUi(this);

    // Disable check if user didn't click on Install
    check = false;

    ui->pathLabel->hide();
}

void InstallDialog::on_folderButton_clicked(){

    addonPath = QFileDialog::getExistingDirectory(this, "Select the add-on folder");

    if(!addonPath.isEmpty()){

        // Add / to make life eaiser
        addonPath = addonPath + "/";

        if(ui->pathLabel->isHidden()) ui->pathLabel->show();

        ui->pathLabel->setText("Selected folder: " + addonPath);

    } else {if(!ui->pathLabel->isHidden()) ui->pathLabel->hide();}
}

void InstallDialog::on_fileButton_clicked(){

    addonPath = QFileDialog::getOpenFileName(this, "Select the add-on file", "", "Zip file (*.zip)");

    if(!addonPath.isEmpty()){

        if(ui->pathLabel->isHidden()) ui->pathLabel->show();

        ui->pathLabel->setText("Selected file: " + addonPath);

    } else {if(!ui->pathLabel->isHidden()) ui->pathLabel->hide();}

}

void InstallDialog::on_installButton_clicked(){

    check = true;

    addonName = ui->nameEdit->text();

    compChecked = ui->fileRadio->isChecked();

    installSources = ui->sourcesCheck->isChecked();

    removeAddonDir = ui->remCheck->isChecked();

    close();
}

void InstallDialog::on_cancelButton_clicked(){close();}

InstallDialog::~InstallDialog(){delete ui;}


