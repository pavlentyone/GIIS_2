#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    updateInterface(NavigationMode);

    dialog = new FindDialog;
    ui->pbLoad->setToolTip(tr("Load contacts from a file"));
    ui->pbSave->setToolTip(tr("Save contacts to a file"));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pbAdd_clicked()
{
    oldNAme = ui->leName->text();
    oldAddress = ui->teAddress->toPlainText();

    ui->leName->clear();
    ui->teAddress->clear();

    updateInterface(AddingMode);
}

void MainWindow::on_pbEdit_clicked()
{
    oldNAme = ui->leName->text();
    oldAddress = ui->teAddress->toPlainText();

    updateInterface(EditingMode);
}

void MainWindow::on_pbRemove_clicked()
{
    QString name = ui->leName->text();
    QString address = ui->teAddress->toPlainText();

    if (contacts.contains(name)) {
        int button = QMessageBox::question(this,
            tr("Confirm Remove"),
            tr("Are you sure you want to remove \"%1\"?").arg(name),
            QMessageBox::Yes | QMessageBox::No);
        if (button == QMessageBox::Yes) {
            on_pbPrevious_clicked();
            contacts.remove(name);
            QMessageBox::information(this, tr("Remove Successful"),
                tr("\"%1\" has been removed from your address book.").arg(name));
        }
    }

    updateInterface(NavigationMode);
}

void MainWindow::on_pbSubmit_clicked(){
    QString name = ui->leName->text();
    QString address = ui->teAddress->toPlainText();
    if(currentMode == AddingMode){
        if(name.isEmpty() || address.isEmpty()){
            QMessageBox::information(this, tr("&Empty fields"), tr("&Please, fill name and address fields"));
            return;
        }
        else if(!contacts.contains(name)){
            contacts.insert(name, address);

            QMap<QString, QString>::iterator i = contacts.find(name);
            if(contacts.size() <= 1){
                oldNAme = i.key();
                oldAddress = i.value();
            }

            QMessageBox::information(this, tr("Add successful"), tr("\"%1\" has been added to your address book.").arg(name));

        }
        else{
            QMessageBox::information(this, tr("Add unsuccessful"), tr("\"%1\" is already in your address book").arg(name));
            return;
        }
    }
    else if (currentMode == EditingMode) {
        if (oldNAme != name) {
            if (!contacts.contains(name)) {
                QMessageBox::information(this, tr("Edit Successful"),
                    tr("\"%1\" has been edited in your address book.").arg(oldNAme));
                contacts.remove(oldNAme);
                contacts.insert(name, address);
            } else {
                QMessageBox::information(this, tr("Edit Unsuccessful"),
                    tr("Sorry, \"%1\" is already in your address book.").arg(name));
            }
        } else if (oldAddress != address) {
            QMessageBox::information(this, tr("Edit Successful"),
                tr("\"%1\" has been edited in your address book.").arg(name));
            contacts[name] = address;
        }
    }
    updateInterface(NavigationMode);
}

void MainWindow::on_pbCancel_clicked(){
    updateInterface(NavigationMode);
}

void MainWindow::on_pbPrevious_clicked()
{
    QString name = ui->leName->text();
    QMap<QString, QString>::iterator i = contacts.find(name);

    if (i == contacts.end()){
        ui->leName->clear();
        ui->teAddress->clear();
        return;
    }

    if (i == contacts.begin())
        i = contacts.end();

    i--;
    ui->leName->setText(i.key());
    ui->teAddress->setText(i.value());
}

void MainWindow::on_pbNext_clicked()
{
    QString name = ui->leName->text();
    QMap<QString, QString>::iterator i = contacts.find(name);

    if (i != contacts.end())
        i++;

    if (i == contacts.end())
        i = contacts.begin();

    ui->leName->setText(i.key());
    ui->teAddress->setText(i.value());
}

void MainWindow::updateInterface(Mode mode)
{
    currentMode = mode;

    switch (currentMode) {
        case AddingMode:
        case EditingMode:

            ui->leName->setReadOnly(false);
            ui->leName->setFocus(Qt::OtherFocusReason);
            ui->teAddress->setReadOnly(false);

            ui->pbAdd->hide();
            ui->pbEdit->hide();
            ui->pbRemove->hide();
            ui->pbFind->hide();

            ui->pbNext->setEnabled(false);
            ui->pbPrevious->setEnabled(false);

            ui->pbSubmit->show();
            ui->pbCancel->show();

            ui->pbExport->hide();
            ui->pbLoad->hide();
            ui->pbSave->hide();
            break;
        case NavigationMode:

            if (contacts.isEmpty()) {
                ui->leName->clear();
                ui->teAddress->clear();
            }

            ui->leName->setReadOnly(true);
            ui->teAddress->setReadOnly(true);

            int number = contacts.size();
            ui->pbAdd->show();
            ui->pbEdit->show();
            ui->pbRemove->show();
            ui->pbFind->show();
            ui->pbEdit->setEnabled(number >= 1);
            ui->pbRemove->setEnabled(number >= 1);
            ui->pbNext->setEnabled(number > 1);
            ui->pbPrevious->setEnabled(number > 1);
            ui->pbFind->setEnabled(number > 1);

            ui->pbSubmit->hide();
            ui->pbCancel->hide();

            ui->pbExport->show();
            ui->pbLoad->show();
            ui->pbSave->show();
            break;
    }
}

void MainWindow::on_pbFind_clicked()
{
    dialog->show();
    if (dialog->exec() == QDialog::Accepted) {
        QString contactName = dialog->getFindText();

        if (contacts.contains(contactName)) {
            ui->leName->setText(contactName);
            ui->teAddress->setText(contacts.value(contactName));
        } else {
            QMessageBox::information(this, tr("Contact Not Found"),
                tr("Sorry, \"%1\" is not in your address book.").arg(contactName));
            return;
        }
    }
    updateInterface(NavigationMode);
}

void MainWindow::on_pbLoad_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
             tr("Open Address Book"), "",
             tr("Address Book (*.abk);;All Files (*)"));
    if (fileName.isEmpty())
             return;
     else {
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox::information(this, tr("Unable to open file"),
                file.errorString());
            return;
        }

        QDataStream in(&file);
        in.setVersion(QDataStream::Qt_4_5);
        contacts.empty();   // очищаем существующие контакты
        in >> contacts;
        if (contacts.isEmpty())
            QMessageBox::information(this, tr("No contacts in file"),
                        tr("The file you are attempting to open contains no contacts."));
        else {
            QMap<QString, QString>::iterator i = contacts.begin();
            ui->leName->setText(i.key());
            ui->teAddress->setText(i.value());
        }
    }

    updateInterface(NavigationMode);
}

void MainWindow::on_pbSave_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this,
             tr("Save Address Book"), "",
             tr("Address Book (*.abk);;All Files (*)"));
    if (fileName.isEmpty())
        return;
     else {
         QFile file(fileName);
         if (!file.open(QIODevice::WriteOnly)) {
             QMessageBox::information(this, tr("Unable to open file"),
                 file.errorString());
             return;
         }
         QDataStream out(&file);
         out.setVersion(QDataStream::Qt_4_5);
         out << contacts;
     }
}

void MainWindow::on_pbExport_clicked()
{
    QString name = ui->leName->text();
    QString address = ui->teAddress->toPlainText();
    QString firstName;
    QString lastName;
    QStringList nameList;
    int index = name.indexOf(" ");

    if (index != -1) {
        nameList = name.split(QRegExp("\\s+"), QString::SkipEmptyParts);
        firstName = nameList.first();
        lastName = nameList.last();
    } else {
        firstName = name;
        lastName = "";
    }

    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Export Contact"), "",
        tr("vCard Files (*.vcf);;All Files (*)"));

    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::information(this, tr("Unable to open file"),
            file.errorString());
        return;
    }

    QTextStream out(&file);
    out << "BEGIN:VCARD" << "\n";
    out << "VERSION:2.1" << "\n";
    out << "N:" << lastName << ";" << firstName << "\n";

    if (!nameList.isEmpty())
       out << "FN:" << nameList.join(" ") << "\n";
    else
       out << "FN:" << firstName << "\n";
    address.replace(";", "\\;", Qt::CaseInsensitive);
    address.replace("\n", ";", Qt::CaseInsensitive);
    address.replace(",", " ", Qt::CaseInsensitive);

    out << "ADR;HOME:;" << address << "\n";
    out << "END:VCARD" << "\n";

    QMessageBox::information(this, tr("Export Successful"),
        tr("\"%1\" has been exported as a vCard.").arg(name));
}
