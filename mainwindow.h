#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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
    QString dirBackup;
    QString pathOrbiter;
    QMap<QString, QList<QString>> dataAddons;
    QMap<QString, QList<QString>> addons;
    explicit MainWindow(QWidget *parent = nullptr);
    void setWidgets();
    void addonsScan();
    ~MainWindow();
private slots:
    void on_actionPath_triggered();

    void on_actionAbout_triggered();

    void on_addButton_clicked();

    void on_uninstallButton_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
