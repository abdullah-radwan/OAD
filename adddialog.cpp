#include "adddialog.h"
#include "ui_adddialog.h"
#include <QFileDialog>

AddDialog::AddDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddDialog)
{
    ui->setupUi(this);

    // Disable check if user didn't click on Ok
    check = false;

    ui->pathLabel->hide();
}

AddDialog::~AddDialog(){delete ui;}


void AddDialog::on_selectButton_clicked()
{
    pathAddon = QFileDialog::getExistingDirectory(this, "Select addon folder");

    if(!pathAddon.isEmpty() && !pathAddon.isNull()){

        // Add / to make life eaiser
        pathAddon = pathAddon + "/";

        // Show label if it hidden
        if(ui->pathLabel->isHidden()) ui->pathLabel->show();

        ui->pathLabel->setText("Selected folder: " + pathAddon);

    } else {if(!ui->pathLabel->isHidden()) ui->pathLabel->hide();}
}


void AddDialog::on_installCheck_toggled(bool checked)
{
    if(checked){

        ui->remCheck->show();

        ui->sdkCheck->show();

    } else {

        ui->remCheck->setChecked(false);

        ui->sdkCheck->setChecked(false);

        ui->remCheck->hide();

        ui->sdkCheck->hide();
    }
}

void AddDialog::on_buttonsBox_accepted()
{
    // Check from given info
    check = true;

    // Set variables
    nameAddon = ui->nameEdit->text();

    install = ui->installCheck->isChecked();

    removeDir = ui->remCheck->isChecked();

    sdkInstall = ui->sdkCheck->isChecked();
}
