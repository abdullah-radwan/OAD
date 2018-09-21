#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "addonsops.h"
#include <QMainWindow>
#include <QString>
#include <QMap>
#include <QList>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void on_actionPath_triggered();

    void on_actionRescan_triggered();

    void on_actionAbout_triggered();

    void on_addButton_clicked();

    void on_uninstallButton_clicked();

    void on_disableButton_clicked();

    void on_enableButton_clicked();

private:
    Ui::MainWindow *ui;
    QString orbiterPath;
    AddonsOps addonsOps;
    void updateAddonsList();
    void setWidgets();
    void setWidgets(bool stat);
};

#endif // MAINWINDOW_H
