#ifndef FINDDIALOG_H
#define FINDDIALOG_H
#include <QDialog>

#include <QDialog>
#include <QPushButton>
#include <QLineEdit>
#include <QString>
#include <QLabel>
#include <QHBoxLayout>
#include <QMessageBox>

class FindDialog : public QDialog
{
    Q_OBJECT

public:
    FindDialog(QWidget *parent = 0);
    QString getFindText();

public slots:
    void findClicked();

private:
    QPushButton *findButton;
    QLineEdit *lineEdit;
    QString findText;
};

#endif // FINDDIALOG_H
