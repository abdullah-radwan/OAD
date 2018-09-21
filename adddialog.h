#ifndef ADDDIALOG_H
#define ADDDIALOG_H

#include <QDialog>
#include <QString>

namespace Ui {
class AddDialog;
}

class AddDialog : public QDialog
{
    Q_OBJECT

public:
    QString name;
    QString path;
    bool check;
    bool install;
    bool installSdk;
    bool removeAddonDir;
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
