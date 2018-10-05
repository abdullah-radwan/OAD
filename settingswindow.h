#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QMainWindow>
#include <QMap>

namespace Ui {class SettingsWindow;}

class SettingsWindow : public QMainWindow{Q_OBJECT

public:
    QString orbiterPath, backupDir;

    QList<QString> pathsList, ignoredList;

    QMap<QString, QList<QString>> dbMap;

    explicit SettingsWindow(QWidget *parent = nullptr);

    ~SettingsWindow();

protected:
    void showEvent(QShowEvent *ev);

private slots:
    void on_addPathButton_clicked();

    void on_removePathButton_clicked();

    void on_createButton_clicked();

    void on_importButton_clicked();

    void on_addEntryButton_clicked();

    void on_removeEntryButton_clicked();

    void on_editEntryButton_clicked();

    void on_pathsCombo_currentIndexChanged(const QString &arg1);

    void on_addIgnButton_clicked();

    void on_removeIgnButton_clicked();

signals:
    void closed();

private:
    Ui::SettingsWindow *ui;

    bool checkSnapshot(QList<QString> snapFiles, QList<QString> files);

    void setDbTree();

    void setIgnList();

    void closeEvent(QCloseEvent *bar);

};

#endif // SETTINGSWINDOW_H
