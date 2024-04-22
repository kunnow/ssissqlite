#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "databaseconnection.h"
#include <QDebug>
#include <QFile>
#include <QMessageBox>
#include <QModelIndex>
#include <QStandardItem>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QPushButton>
#include <QListView>
#include <QRegularExpression>
#include <QSqlQuery>
#include <QSqlError>
#include <QTextStream>
#include <QFileDialog>
#include <QSqlTableModel>
#include <QTableWidget>
#include <QSqlQueryModel>
#include <QFileInfo>
#include <QTabWidget>
#include <QSqlRecord>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)

{

    ui->setupUi(this);

    connect(ui->tableWidget, &QTableWidget::itemClicked, this, &MainWindow::rowstudents);
    connect(ui->tableWidget2, &QTableWidget::itemClicked, this, &MainWindow::rowcourses);

    QFileInfo fileInfo(QCoreApplication::applicationDirPath() + "/mainwindow.db");
    if (fileInfo.exists()) {
        qDebug() << "File exists";
        qDebug() << "Permissions:" << fileInfo.permissions();
    } else {
        qDebug() << "File does not exist";
    }

    databaseconnection con;
    if (con.Connect()){
        qDebug() << "Database is Connected";
    }
    else{
        qDebug() << "Database is NOT Connected";
    }

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("C://Users//Shir Keilah//Documents//ssisdb//database.db");

    if (!db.open()) {
        qDebug() << "Error opening database:" << db.lastError().text();
        return;
    }

    QSqlQuery query("SELECT * FROM students");

    if (!query.exec()) {
        qDebug() << "Error executing query:" << query.lastError().text();
        db.close();
        return;
    }

    ui->tableWidget->setRowCount(0);

    ui->tableWidget->setColumnCount(7);
    ui->tableWidget->setHorizontalHeaderLabels({"Last Name", "First Name", "Middle Name", "ID Number", "Gender", "Year Level", "Course"});

    int row = 0;
    while (query.next()) {
        QString LastName = query.value("LastName").toString();
        QString FirstName = query.value("FirstName").toString();
        QString MiddleName = query.value("MiddleName").toString();
        QString IDNumber = query.value("IDNumber").toString();
        QString Gender = query.value("Gender").toString();
        QString YearLevel = query.value("YearLevel").toString();
        QString Course = query.value("Course").toString();

        ui->tableWidget->insertRow(row);

        ui->tableWidget->setItem(row, 0, new QTableWidgetItem(LastName));
        ui->tableWidget->setItem(row, 1, new QTableWidgetItem(FirstName));
        ui->tableWidget->setItem(row, 2, new QTableWidgetItem(MiddleName));
        ui->tableWidget->setItem(row, 3, new QTableWidgetItem(IDNumber));
        ui->tableWidget->setItem(row, 4, new QTableWidgetItem(Gender));
        ui->tableWidget->setItem(row, 5, new QTableWidgetItem(YearLevel));
        ui->tableWidget->setItem(row, 6, new QTableWidgetItem(Course));

        row++;
    }

    //ui->tableWidget->resizeColumnsToContents();

    query.finish();

    QSqlQuery queryCourses(db);
    queryCourses.prepare("SELECT * FROM courses");

    if (!queryCourses.exec()) {
        qDebug() << "Error executing courses query:" << queryCourses.lastError().text();
        db.close();
        return;
    }

        ui->tableWidget2->setRowCount(0);
        ui->tableWidget2->setColumnCount(2);
        ui->tableWidget2->setHorizontalHeaderLabels({"Course Code", "Course Name", "Credits"});

        row = 0;
        while (queryCourses.next()) {
            QString courseCode = queryCourses.value("CourseCode").toString();
            QString courseName = queryCourses.value("CourseName").toString();

            ui->tableWidget2->insertRow(row);

            ui->tableWidget2->setItem(row, 0, new QTableWidgetItem(courseCode));
            ui->tableWidget2->setItem(row, 1, new QTableWidgetItem(courseName));

            row++;
        }
    ui->tableWidget2->resizeColumnsToContents();
    db.close();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateStudentsTable() {

    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);

    QSqlQuery query("SELECT LastName, FirstName, MiddleName, IDNumber, Gender, YearLevel, Course FROM students");
    int row = 0;
    while (query.next()) {
        ui->tableWidget->insertRow(row);
        for (int col = 0; col < ui->tableWidget->columnCount(); ++col) {
            QTableWidgetItem *item = new QTableWidgetItem(query.value(col).toString());
            ui->tableWidget->setItem(row, col, item);
        }
        ++row;
    }
}

void MainWindow::rowstudents(QTableWidgetItem *item)
{
    int row = item->row();

    QString LastName = ui->tableWidget->item(row, 0)->text();
    QString Firstname = ui->tableWidget->item(row, 1)->text();
    QString MiddleName = ui->tableWidget->item(row, 2)->text();
    QString IDNumber = ui->tableWidget->item(row, 3)->text();
    QString Gender = ui->tableWidget->item(row, 4)->text();
    QString YearLevel = ui->tableWidget->item(row, 5)->text();
    QString Course = ui->tableWidget->item(row, 6)->text();

    ui->lastname->setText(LastName);
    ui->firstname->setText(Firstname);
    ui->middlename->setText(MiddleName);
    ui->idnumber->setText(IDNumber);
    ui->gender->setCurrentText(Gender);
    ui->yearlevel->setValue(YearLevel.toInt());
    ui->coursecode->setText(Course);
}

void MainWindow::rowcourses(QTableWidgetItem *item)
{
    int row = item->row();
    QString CourseCode = ui->tableWidget2->item(row, 0)->text();
    QString CourseName = ui->tableWidget2->item(row, 1)->text();

    ui->code->setText(CourseCode);
    ui->name->setText(CourseName);
}

void MainWindow::setWidgetsEditable(bool editable)
{
    ui->lastname->setReadOnly(!editable);
    ui->firstname->setReadOnly(!editable);
    ui->middlename->setReadOnly(!editable);
    ui->idnumber->setReadOnly(!editable);
    ui->gender->setDisabled(!editable);
    ui->yearlevel->setReadOnly(!editable);
    ui->coursecode->setReadOnly(!editable);
}

void MainWindow::on_editbutton_clicked()
{
    if (selectedIndex.isValid())
    {
        if (!editMode)
        {
            editMode = true;
            setWidgetsEditable(true);

            ui->savebutton->setEnabled(true);
            ui->savebutton->setStyleSheet("QPushButton { background-color: #800000; color: white; border: 2px solid #000000; border-radius: 5px; padding: 1px; box-shadow: 0 0 10px rgba(0, 0, 0, 0.5); color: white; } QPushButton:hover { background-color: #944E63; border: 2px solid #944E63; }");

            QString id = model->data(model->index(selectedIndex.row(), 0)).toString();

            QString queryStr = QString("SELECT * FROM students WHERE id = :id");

            QSqlQuery query;
            query.prepare(queryStr);
            query.bindValue(":id", id);

            if(query.exec())
            {
                if(query.next())
                {
                    ui->lastname->setText(query.value("LastName").toString());
                    ui->firstname->setText(query.value("FirstName").toString());
                    ui->middlename->setText(query.value("MiddleName").toString());
                    ui->idnumber->setText(query.value("IDNumber").toString());
                    ui->gender->setCurrentText(query.value("Gender").toString());
                    ui->yearlevel->setValue(query.value("YearLevel").toInt());
                    ui->coursecode->setText(query.value("Course").toString());
                }
                else
                {
                    qDebug() << "No data found for ID: " << id;
                }
            }
            else
            {
                qDebug() << "Query execution failed: " << query.lastError().text();
            }
        }
    }
    else if (!editMode)
    {
        QMessageBox::warning(this, "Edit Error", "Please search for a record before attempting to edit.");
    }
}

void MainWindow::on_savebutton_clicked()
{
    if (ui->lastname->text().isEmpty() ||
        ui->firstname->text().isEmpty() ||
        ui->idnumber->text().isEmpty() ||
        ui->yearlevel->value() == 0 ||
        ui->coursecode->text().trimmed().isEmpty())
    {
        QMessageBox::critical(this, "Validation Error", "Please fill out all required fields.");
        return;
    }

    QString lastName = ui->lastname->text();
    QString firstName = ui->firstname->text();
    QString middleName = ui->middlename->text();
    QString idNumber = ui->idnumber->text();
    QString gender = ui->gender->currentText();
    QString yearLevel = QString::number(ui->yearlevel->value());
    QString courseCode = ui->coursecode->text();

    static const QRegularExpression idRegex("^\\d{4}-\\d{4}$");
    if (!idRegex.match(idNumber).hasMatch())
    {
        QMessageBox::critical(this, "ID Format Error", "Please enter the correct ID number format");
        return;
    }

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("database.db");
    if (!db.open())
    {
        QMessageBox::critical(this, "Database Error", "Unable to open database.");
        return;
    }

    QSqlQuery courseQuery;
    courseQuery.prepare("SELECT COUNT(*) FROM courses WHERE CourseCode = :courseCode");
    courseQuery.bindValue(":courseCode", courseCode);
    if (!courseQuery.exec() || !courseQuery.next() || courseQuery.value(0).toInt() == 0) {
        QMessageBox::critical(this, "Course not found", "The specified course code does not exist.");
        db.close();
        return;
    }

    QSqlQuery idQuery;
    idQuery.prepare("SELECT COUNT(*) FROM students WHERE IDNumber = :idNumber");
    idQuery.bindValue(":idNumber", idNumber);
    if (!idQuery.exec() || !idQuery.next() || idQuery.value(0).toInt() > 0) {
        QMessageBox::critical(this, "ID already exists", "The specified ID number already exists.");
        db.close();
        return;
    }

    QSqlQuery updateQuery;
    updateQuery.prepare("UPDATE students SET LastName = :lastName, FirstName = :firstName, "
                        "MiddleName = :middleName, IDNumber = :idNumber, Gender = :gender, "
                        "YearLevel = :yearLevel, Course = :courseCode WHERE RowID = :rowID");
    updateQuery.bindValue(":lastName", lastName);
    updateQuery.bindValue(":firstName", firstName);
    updateQuery.bindValue(":middleName", middleName);
    updateQuery.bindValue(":idNumber", idNumber);
    updateQuery.bindValue(":gender", gender);
    updateQuery.bindValue(":yearLevel", yearLevel);
    updateQuery.bindValue(":courseCode", courseCode);
    updateQuery.bindValue(":rowid", selectedIndex.row() + 1);

    if (!updateQuery.exec())
    {
        QMessageBox::critical(this, "Query Error", "Error updating student data: " + updateQuery.lastError().text());
        db.close();
        return;
    }

    db.close();

    ui->savebutton->setDisabled(true);
    ui->savebutton->setStyleSheet("color: gray;");
    setWidgetsEditable(false);
    editMode = false;

    QMessageBox::information(this, "Data Updated", "Data updated in the database.");

    updateStudentsTable();
}

void MainWindow::on_reload_clicked()
{
    ui->searchlineedit->clear();
    displayAllData();

}

void MainWindow::displayAllData()
{
    ui->tableWidget->setRowCount(0);
    ui->tableWidget2->setRowCount(0);

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("C://Users//Shir Keilah//Documents//ssisdb//database.db");

    if (!db.open()) {
        qDebug() << "Error opening database:" << db.lastError().text();
        return;
    }

    QSqlQuery studentsQuery("SELECT * FROM students");
    populateTable(ui->tableWidget, studentsQuery);

    QSqlQuery coursesQuery("SELECT * FROM courses");
    populateTable(ui->tableWidget2, coursesQuery);
}

void MainWindow::populateTable(QTableWidget *tableWidget, QSqlQuery &query)
{
    int row = 0;
    while (query.next()) {
        tableWidget->insertRow(row);
        for (int col = 0; col < query.record().count(); ++col) {
            QTableWidgetItem *item = new QTableWidgetItem(query.value(col).toString());
            tableWidget->setItem(row, col, item);
        }
        ++row;
    }
}

void MainWindow::on_addbutton_clicked()
{
    if (ui->lastname->text().isEmpty() ||
        ui->firstname->text().isEmpty() ||
        ui->idnumber->text().isEmpty() ||
        ui->yearlevel->value() == 0 ||
        ui->coursecode->text().trimmed().isEmpty())
    {
        QMessageBox::critical(this, "Validation Error", "Please fill out all required fields.");
        return;
    }

    QString last = ui->lastname->text();
    QString first = ui->firstname->text();
    QString middle = ui->middlename->text();
    QString id = ui->idnumber->text();
    QString gen = ui->gender->currentText();
    QString year = QString::number(ui->yearlevel->value());
    QString course = ui->coursecode->text().trimmed();

    databaseconnection con;
    if (!con.Connect()) {
        qDebug() << "Database connection failed";
        return;
    }
    qDebug() << "Database is connected";

    QSqlQuery query;

    static const QRegularExpression idRegex("^\\d{4}-\\d{4}$");

    if (!idRegex.match(id).hasMatch())
    {
        QMessageBox::critical(this, "ID Format Error", "Please enter the correct ID number format");
        return;
    }

    QSqlQuery courseQuery;
    courseQuery.prepare("SELECT COUNT(*) FROM courses WHERE CourseCode = :courseCode");
    courseQuery.bindValue(":courseCode", course);
    if (!courseQuery.exec() || !courseQuery.next() || courseQuery.value(0).toInt() == 0) {
        qDebug() << "Course does not exist";
        QMessageBox::critical(this, "Course not found", "The specified course code does not exist.");
        return;
    }

    QSqlQuery idQuery;
    idQuery.prepare("SELECT COUNT(*) FROM students WHERE IDNumber = :id");
    idQuery.bindValue(":id", id);
    if (!idQuery.exec() || !idQuery.next() || idQuery.value(0).toInt() > 0) {
        qDebug() << "ID already exists";
        QMessageBox::critical(this, "ID already exists", "The specified ID number already exists.");
        return;
    }

    QString queryString = QString("INSERT INTO students (LastName, FirstName, MiddleName, IDNumber, Gender, YearLevel, Course) "
                                  "VALUES ('%1', '%2', '%3', '%4', '%5', '%6', '%7')")
                              .arg(last).arg(first).arg(middle).arg(id).arg(gen).arg(year).arg(course);
    if (!query.exec(queryString)) {
        qDebug() << "Query execution failed:" << query.lastError().text();
        con.Disconnect();
        return;
    }

    ui->lastname->clear();
    ui->firstname->clear();
    ui->middlename->clear();
    ui->idnumber->clear();
    //ui->gender->clear();
    //ui->yearlevel->clear();
    ui->coursecode->clear();

    qDebug() << "Query executed successfully";
    QMessageBox::information(this, "Data Added", "Data added to the database.");

    updateStudentsTable();
    updateCoursesTable();
    con.Disconnect();
}

void MainWindow::on_searchbutton_clicked()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("C://Users//Shir Keilah//Documents//ssisdb//database.db");

    if (!db.open()) {
        qDebug() << "Error opening database:" << db.lastError().text();
        return;
    }

    QString searchText = ui->searchlineedit->text().trimmed();

    QSqlQuery studentsQuery;
    studentsQuery.prepare("SELECT * FROM students WHERE IDNumber = :searchText");
    studentsQuery.bindValue(":searchText", searchText);
    if (!studentsQuery.exec()) {
        qDebug() << "Error executing students query:" << studentsQuery.lastError().text();
        return;
    }
    ui->tableWidget->setRowCount(0);
    int row = 0;
    while (studentsQuery.next()) {
        ui->tableWidget->insertRow(row);
        for (int col = 0; col < 7; ++col) {
            QTableWidgetItem *item = new QTableWidgetItem(studentsQuery.value(col).toString());
            ui->tableWidget->setItem(row, col, item);
        }
        ++row;
    }

    QSqlQuery coursesQuery;
    coursesQuery.prepare("SELECT * FROM courses WHERE CourseCode = :searchText");
    coursesQuery.bindValue(":searchText", searchText);
    if (!coursesQuery.exec()) {
        qDebug() << "Error executing courses query:" << coursesQuery.lastError().text();
        return;
    }
    ui->tableWidget2->setRowCount(0);
    row = 0;
    while (coursesQuery.next()) {
        ui->tableWidget2->insertRow(row);
        for (int col = 0; col < 2; ++col) {
            QTableWidgetItem *item = new QTableWidgetItem(coursesQuery.value(col).toString());
            ui->tableWidget2->setItem(row, col, item);
        }
        ++row;
    }

}

void MainWindow::on_addcourse_clicked()
{
    if (ui->coursecode_2->text().isEmpty() ||
        ui->coursecode_3->text().isEmpty())
    {
        QMessageBox::critical(this, "Validation Error", "Please fill out all required fields.");
        return;
    }

    QString CourseCode = ui->coursecode_2->text();
    QString CourseName = ui->coursecode_3->text().trimmed();

    databaseconnection con;
    if (!con.Connect()) {
        qDebug() << "Database connection failed";
        return;
    }
    qDebug() << "Database is connected";

    QSqlQuery query;

    QSqlQuery courseQuery;
    courseQuery.prepare("SELECT COUNT(*) FROM courses WHERE CourseCode = :courseCode");
    courseQuery.bindValue(":courseCode", CourseCode);
    if (!courseQuery.exec() || !courseQuery.next() || courseQuery.value(0).toInt() > 0) {
        qDebug() << "Course already exists";
        QMessageBox::critical(this, "Course already exists", "The specified course code already exists.");
        return;
    }

    QString queryString = QString("INSERT INTO courses (CourseCode, CourseName) "
                                  "VALUES ('%1', '%2')")
                              .arg(CourseCode).arg(CourseName);
    if (!query.exec(queryString)) {
        qDebug() << "Query execution failed:" << query.lastError().text();
        con.Disconnect();
        return;
    }

    ui->coursecode_2->clear();
    ui->coursecode_3->clear();

    qDebug() << "Query executed successfully";
    QMessageBox::information(this, "Course Added", "Course added to the database.");

    updateCoursesTable();
    con.Disconnect();
}

void MainWindow::updateCoursesTable()
{
    ui->tableWidget2->clearContents();

    QSqlQuery coursesQuery("SELECT * FROM courses");
    populateTable(ui->tableWidget2, coursesQuery);
}

/**
void MainWindow::on_searchstudent_clicked()
{

}**/
