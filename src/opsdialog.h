#ifndef OPSDIALOG_H
#define OPSDIALOG_H

#include <QDialog>
#include <QString>

namespace Ui {
class OpsDialog;
}

class OpsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OpsDialog(QWidget *parent = nullptr);
    OpsDialog(QWidget *parent, QString title, QString label);
    ~OpsDialog();

private:
    Ui::OpsDialog *ui;
    void reject();
};

#endif // OPSDIALOG_H
