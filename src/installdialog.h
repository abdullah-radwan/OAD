#ifndef INSTALLDIALOG_H
#define INSTALLDIALOG_H

#include <QDialog>

namespace Ui {class InstallDialog;}

class InstallDialog : public QDialog{Q_OBJECT

public:
    QString addonName, addonPath;

    bool check, installSources, removeAddonDir, compChecked;

    explicit InstallDialog(QWidget *parent = nullptr);

    ~InstallDialog();

private slots:
    void on_folderButton_clicked();

    void on_installButton_clicked();

    void on_cancelButton_clicked();

    void on_fileButton_clicked();

private:
    Ui::InstallDialog *ui;

};

#endif // INSTALLDIALOG_H
