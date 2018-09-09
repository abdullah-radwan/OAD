#ifndef INSTALLDIALOG_H
#define INSTALLDIALOG_H

#include <QDialog>
#include <QString>

namespace Ui {
class AddDialog;
}

class AddDialog : public QDialog
{
    Q_OBJECT

public:
    QString nameAddon;
    QString pathAddon;
    bool install;
    bool removeDir;
    bool check;
    bool sdkInstall;
    explicit AddDialog(QWidget *parent = nullptr);
    ~AddDialog();

private slots:
    void on_selectButton_clicked();

    void on_installCheck_toggled(bool checked);

    void on_buttonsBox_accepted();

private:
    Ui::AddDialog *ui;
};

#endif // INSTALLDIALOG_H
