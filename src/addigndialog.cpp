#include "addigndialog.h"
#include "ui_addigndialog.h"

AddIgnDialog::AddIgnDialog(QWidget *parent) : QDialog(parent), ui(new Ui::AddIgnDialog) { ui->setupUi(this); }

void AddIgnDialog::showEvent(QShowEvent *ev)
{
	QDialog::showEvent(ev);

	ui->addonCombo->addItem("All");

	ui->addonCombo->addItems(addons);

	ui->filesEdit->setText(addonFiles);
}

void AddIgnDialog::on_addButton_clicked()
{
	addonFiles = ui->filesEdit->text();

	addonName = ui->addonCombo->currentText();

	check = true;

	close();
}

void AddIgnDialog::on_cancelButton_clicked() { close(); }

AddIgnDialog::~AddIgnDialog() { delete ui; }
