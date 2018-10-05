#ifndef ADDDBDIALOG_H
#define ADDDBDIALOG_H

#include <QDialog>

namespace Ui {class AddDbDialog;}

class AddDbDialog : public QDialog{Q_OBJECT

public:
    QString addonName, addonFiles, addonPath = "";

    bool check, filesChecked;

    explicit AddDbDialog(QWidget *parent = nullptr);

    ~AddDbDialog();

protected:
    void showEvent(QShowEvent *ev);

private slots:
    void on_addButton_clicked();

    void on_cancelButton_clicked();

    void on_selectButton_clicked();

private:
    Ui::AddDbDialog *ui;

};

#endif // ADDDBDIALOG_H
