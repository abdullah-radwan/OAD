#include "installdialog.h"
#include "ui_installdialog.h"
#include <QFileDialog>

InstallDialog::InstallDialog(QWidget *parent) : QDialog(parent), ui(new Ui::InstallDialog){

    ui->setupUi(this);

    // Disable check if user didn't click on Ok
    check = false;

    ui->pathLabel->hide();
}

void InstallDialog::on_selectButton_clicked(){

    addonPath = QFileDialog::getExistingDirectory(this, "Select the add-on folder");

    if(!addonPath.isEmpty()){

        // Add / to make life eaiser
        addonPath = addonPath + "/";

        if(ui->pathLabel->isHidden()) ui->pathLabel->show();

        ui->pathLabel->setText("Selected folder: " + addonPath);

    } else {if(!ui->pathLabel->isHidden()) ui->pathLabel->hide();}
}

void InstallDialog::on_installButton_clicked(){

    check = true;

    addonName = ui->nameEdit->text();

    installSources = ui->sourcesCheck->isChecked();

    removeAddonDir = ui->remCheck->isChecked();

    close();
}

void InstallDialog::on_cancelButton_clicked(){close();}

InstallDialog::~InstallDialog(){delete ui;}
