#ifndef ADDIGNDIALOG_H
#define ADDIGNDIALOG_H

#include <QDialog>

namespace Ui {class AddIgnDialog;}

class AddIgnDialog : public QDialog {Q_OBJECT

public:
    QString addonName, addonFiles;

    QStringList addons;

    bool check;

    explicit AddIgnDialog(QWidget *parent = nullptr);

    ~AddIgnDialog();

protected:
    void showEvent(QShowEvent *ev);

private slots:
    void on_addButton_clicked();

    void on_cancelButton_clicked();

private:
    Ui::AddIgnDialog *ui;
};

#endif // ADDIGNDIALOG_H
