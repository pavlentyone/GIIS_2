#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QString>
#include <QGroupBox>
#include <QPushButton>
#include <QMessageBox>
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include "finddialog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pbAdd_clicked();
    void on_pbEdit_clicked();
    void on_pbRemove_clicked();
    void on_pbSubmit_clicked();
    void on_pbCancel_clicked();
    void on_pbPrevious_clicked();
    void on_pbNext_clicked();
    void on_pbFind_clicked();
    void on_pbLoad_clicked();
    void on_pbSave_clicked();
    void on_pbExport_clicked();

private:
    Ui::MainWindow *ui;
    QMap<QString, QString> contacts;
    QString oldNAme;
    QString oldAddress;
    enum Mode { NavigationMode, AddingMode, EditingMode } currentMode;
    void updateInterface(Mode mode);
    FindDialog *dialog;
};

#endif // MAINWINDOW_H
