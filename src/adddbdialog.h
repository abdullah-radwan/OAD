#ifndef ADDDBDIALOG_H
#define ADDDBDIALOG_H

#include <QDialog>

namespace Ui {class AddDbDialog;}

class AddDbDialog : public QDialog{Q_OBJECT

public:
    QString addonName, addonFiles, addonPath;

    bool check, folderChecked, filesChecked, fileChecked, removeDir;

    explicit AddDbDialog(QWidget *parent = nullptr);

    ~AddDbDialog();

protected:
    void showEvent(QShowEvent *ev);

private slots:
    void on_addButton_clicked();

    void on_cancelButton_clicked();

    void on_folderButton_clicked();

    void on_fileButton_clicked();

                                   void on_filesRadio_clicked();

                                                                  void on_fileRadio_clicked();

                                                                                                 void on_folderRadio_clicked();

                               private:
    Ui::AddDbDialog *ui;

};

#endif // ADDDBDIALOG_H
