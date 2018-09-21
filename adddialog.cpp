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

void AddDialog::on_selectButton_clicked()
{
    path = QFileDialog::getExistingDirectory(this, "Select the add-on folder");

    if(!path.isEmpty() && !path.isNull()){

        // Add / to make life eaiser
        path = path + "/";

        if(ui->pathLabel->isHidden()) ui->pathLabel->show();

        ui->pathLabel->setText("Selected folder: " + path);

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
    // Check the given info
    check = true;

    // Set variables
    name = ui->nameEdit->text();

    install = ui->installCheck->isChecked();

    removeAddonDir = ui->remCheck->isChecked();

    installSdk = ui->sdkCheck->isChecked();
}

AddDialog::~AddDialog(){delete ui;}
