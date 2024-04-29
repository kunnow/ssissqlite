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
    connect(ui->tableWidget->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(on_tableWidget_itemSelectionChanged()));
    connect(ui->code, &QLineEdit::textChanged, this, &MainWindow::on_code_textChanged);
    connect(ui->code, &QLineEdit::textChanged, this, &MainWindow::updateOriginalCourseCode);

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

    callDatabase();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::callDatabase(){
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

void MainWindow::studentsEditable(bool editable)
{
    ui->lastname->setReadOnly(!editable);
    ui->firstname->setReadOnly(!editable);
    ui->middlename->setReadOnly(!editable);
    ui->idnumber->setReadOnly(!editable);
    ui->gender->setDisabled(!editable);
    ui->yearlevel->setDisabled(!editable);
    ui->coursecode->setReadOnly(!editable);
}

void MainWindow::coursesEditable(bool editable)
{
    ui->code->setReadOnly(!editable);
    ui->name->setReadOnly(!editable);
}

void MainWindow::on_editbutton_clicked()
{
    if (!ui->lastname->text().isEmpty() || !ui->firstname->text().isEmpty() || !ui->middlename->text().isEmpty() ||
        !ui->idnumber->text().isEmpty() || !ui->coursecode->text().isEmpty()) {

        if (!editMode) {
            editMode = true;
            studentsEditable(true);

            ui->savebutton->setEnabled(true);
            ui->savebutton->setStyleSheet("QPushButton { background-color: #800000; color: white; border: 2px solid #000000; border-radius: 5px; padding: 1px; box-shadow: 0 0 10px rgba(0, 0, 0, 0.5); color: white; } QPushButton:hover { background-color: #944E63; border: 2px solid #944E63; }");
        }
    } else if (!ui->tableWidget->selectedItems().isEmpty()) {
        if (!editMode) {
            editMode = true;
            coursesEditable(true);

            ui->savebutton->setEnabled(true);
            ui->savebutton->setStyleSheet("QPushButton { background-color: #800000; color: white; border: 2px solid #000000; border-radius: 5px; padding: 1px; box-shadow: 0 0 10px rgba(0, 0, 0, 0.5); color: white; } QPushButton:hover { background-color: #944E63; border: 2px solid #944E63; }");
        } else {
        QMessageBox::warning(this, "Edit Error", "Please select a record before attempting to edit.");
    }
}
}

void MainWindow::on_savebutton_clicked()
{
    QString last = ui->lastname->text();
    QString first = ui->firstname->text();
    QString middle = ui->middlename->text();
    QString id = ui->idnumber->text();
    QString gen = ui->gender->currentText();
    QString year = QString::number(ui->yearlevel->value());
    QString course = ui->coursecode->text().trimmed();

    if (last.isEmpty() || first.isEmpty() || id.isEmpty() || ui->yearlevel->value() == 0 || course.isEmpty())
    {
        QMessageBox::critical(this, "Validation Error", "Please fill out all required fields.");
        return;
    }

    static const QRegularExpression idRegex("^\\d{4}-\\d{4}$");
    if (!idRegex.match(id).hasMatch())
    {
        QMessageBox::critical(this, "ID Format Error", "Please enter the correct ID number format");
        return;
    }

    databaseconnection con;
    if (!con.Connect()) {
        qDebug() << "Database connection failed";
        return;
    }
    qDebug() << "Database is connected";

    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery idQuery(db);
    idQuery.prepare("SELECT COUNT(*) FROM students WHERE IDNumber = :id");
    idQuery.bindValue(":id", id);
    if (!idQuery.exec() || !idQuery.next()) {
        qDebug() << "Error executing ID query:" << idQuery.lastError().text();
        QMessageBox::critical(this, "Database Error", "Error checking ID number: " + idQuery.lastError().text());
        con.Disconnect();
        return;
    }

    int idCount = idQuery.value(0).toInt();
    if (idCount == 0) {
        qDebug() << "ID does not exist";
        QMessageBox::critical(this, "ID not found", "The specified ID number does not exist.");
        con.Disconnect();
        return;
    }

    QSqlQuery query(db);
    QString queryString = "UPDATE students SET LastName = :last, FirstName = :first, "
                          "MiddleName = :middle, Gender = :gen, YearLevel = :year, Course = :course "
                          "WHERE IDNumber = :id";
    query.prepare(queryString);
    query.bindValue(":last", last);
    query.bindValue(":first", first);
    query.bindValue(":middle", middle);
    query.bindValue(":gen", gen);
    query.bindValue(":year", year);
    query.bindValue(":course", course);
    query.bindValue(":id", id);
    if (!query.exec()) {
        qDebug() << "Query execution failed:" << query.lastError().text();
        QMessageBox::critical(this, "Query Error", "Error executing query: " + query.lastError().text());
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
    QMessageBox::information(this, "Data Updated", "Data updated in the database.");

    updateStudentsTable();
    updateCoursesTable();
    ui->savebutton->setDisabled(true);
    ui->savebutton->setStyleSheet("color: gray;");
    studentsEditable(true);
    editMode = false;
    con.Disconnect();
}

void MainWindow::on_reload_clicked()
{
    ui->searchlineedit->clear();
    callDatabase();
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

    ui->lastname->setReadOnly(true);
    ui->firstname->setReadOnly(true);
    ui->middlename->setReadOnly(true);
    ui->idnumber->setReadOnly(true);
    ui->gender->setDisabled(true);
    ui->yearlevel->setDisabled(true);
    ui->coursecode->setReadOnly(true);
    studentsEditable(false);
}

void MainWindow::on_addcourse_clicked()
{
    if (ui->code->text().isEmpty() ||
        ui->name->text().isEmpty())
    {
        QMessageBox::critical(this, "Validation Error", "Please fill out all required fields.");
        return;
    }

    QString CourseCode = ui->code->text();
    QString CourseName = ui->name->text().trimmed();

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

    ui->code->clear();
    ui->name->clear();

    qDebug() << "Query executed successfully";
    QMessageBox::information(this, "Course Added", "Course added to the database.");

    ui->code->clear();
    ui->name->clear();
    updateCoursesTable();
    con.Disconnect();
}

void MainWindow::updateCoursesTable()
{
    ui->tableWidget2->clearContents();

    QSqlQuery coursesQuery("SELECT * FROM courses");
    populateTable(ui->tableWidget2, coursesQuery);
}

void MainWindow::on_deletebutton_clicked()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("C://Users//Shir Keilah//Documents//ssisdb//database.db");

    if (!db.open()) {
        qDebug() << "Error opening database:" << db.lastError().text();
        return;
    }

    QString id = ui->idnumber->text();

    QSqlQuery query;
    query.prepare("DELETE FROM students WHERE IDNumber = :id");
    query.bindValue("id", id);

    if (query.exec())
    {
        QMessageBox::information(this, "Success", "Student record deleted.");
        for (int row = 3; row < ui->tableWidget->rowCount(); ++row) {
            if (ui->tableWidget->item(row, 3)->text() == id) {
                ui->tableWidget->removeRow(row);
                break;
            }
        }
        ui->lastname->clear();
        ui->firstname->clear();
        ui->middlename->clear();
        ui->idnumber->clear();
        ui->gender->setCurrentIndex(0);
        ui->yearlevel->setValue(0);
        ui->coursecode->clear();
    }
}

void MainWindow::on_tableWidget_itemSelectionChanged()
{
    QModelIndexList selectedstudent = ui->tableWidget->selectionModel()->selectedIndexes();
    if (!selectedstudent.isEmpty()) {
        selectedIndex = selectedstudent.at(0);
    } else {
        selectedIndex = QModelIndex();
    }

    QModelIndexList selectedcourse = ui->tableWidget2->selectionModel()->selectedIndexes();
    if (!selectedcourse.isEmpty()) {
        selectedIndex = selectedcourse.at(0);
        QString code = model->data(model->index(selectedIndex.row(), 0)).toString();
        QString name = model->data(model->index(selectedIndex.row(), 1)).toString();
        ui->code->setText(code);
        ui->name->setText(name);
    } else {
        selectedIndex = QModelIndex();
        ui->code->clear();
        ui->name->clear();
    }
}

void MainWindow::on_editcourse_clicked()
{
    if (!editMode) {
        editMode = true;
        coursesEditable(true);

        ui->savecourse->setEnabled(true);
        ui->savecourse->setStyleSheet("QPushButton { background-color: #800000; color: white; border: 2px solid #000000; border-radius: 5px; padding: 1px; box-shadow: 0 0 10px rgba(0, 0, 0, 0.5); color: white; } QPushButton:hover { background-color: #944E63; border: 2px solid #944E63; }");
    }
    ui->tableWidget2->clearSelection();
}

void MainWindow::on_savecourse_clicked()
{
    QString newCourseCode = ui->code->text();
    QString newCourseName = ui->name->text();
    QString newCourse = ui->coursecode->text();

    if (newCourseCode.isEmpty() || newCourseName.isEmpty()) {
        QMessageBox::warning(this, "Validation Error", "Please fill out all required fields.");
        return;
    }

    databaseconnection con;
    if (!con.Connect()) {
        qDebug() << "Database connection failed";
        return;
    }
    qDebug() << "Database is connected";

    QSqlQuery query(db);

    if (editMode && (newCourseCode != originalCourseCode)) {
        QString queryString = "UPDATE students SET course = :newCourseCode WHERE course = :originalCourseCode";
        query.prepare(queryString);
        query.bindValue(":newCourseCode", newCourseCode);
        query.bindValue(":originalCourseCode", originalCourseCode);
        if (!query.exec()) {
            qDebug() << "Failed to update students' course code:" << query.lastError().text();
            QMessageBox::critical(this, "Query Error", "Failed to update students' course code: " + query.lastError().text());
            con.Disconnect();
            return;
        }
    }

    QString updateQueryString = "UPDATE courses SET CourseCode = :co, CourseName = :na WHERE CourseCode = :originalCode";
    query.prepare(updateQueryString);
    query.bindValue(":co", newCourseCode);
    query.bindValue(":na", newCourseName);
    query.bindValue(":originalCode", originalCourseCode);

    if (!query.exec()) {
        qDebug() << "Query execution failed:" << query.lastError().text();
        QMessageBox::critical(this, "Query Error", "Error executing update query: " + query.lastError().text());
        con.Disconnect();
        return;
    }

    ui->code->clear();
    ui->name->clear();

    qDebug() << "Update query executed successfully";
    QMessageBox::information(this, "Data Updated", "Data updated in the database.");

    updateStudentsTable();
    updateCoursesTable();
    con.Disconnect();
    ui->savecourse->setDisabled(true);
    ui->savecourse->setStyleSheet("color: gray;");
    editMode = false;
}

void MainWindow::updateOriginalCourseCode() {
    originalCourseCode = ui->code->text();
}

void MainWindow::on_code_textChanged(const QString &text)
{
    Q_UNUSED(text);
    updateOriginalCourseCode();
}

void MainWindow::editCourse(const QString& courseCode) {
    originalCourseCode = courseCode;
}

void MainWindow::on_deletecourse_clicked()
{
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isValid()) {
        db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("C://Users//Shir Keilah//Documents//ssisdb//database.db");
    }

    if (!db.open()) {
        qDebug() << "Error opening database:" << db.lastError().text();
        return;
    }

    QString courseCode = ui->code->text();
    QString courseName = ui->name->text();

    if (!courseCode.isEmpty() || !courseName.isEmpty()) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Confirmation", "Are you sure you want to delete this course and its associated students?", QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes)
        {
            QSqlQuery selectStudentsQuery;
            selectStudentsQuery.prepare("SELECT * FROM students WHERE course = :courseCode");
            selectStudentsQuery.bindValue(":courseCode", courseCode);
            if (!selectStudentsQuery.exec()) {
                qDebug() << "Error selecting students:" << selectStudentsQuery.lastError().text();
                QMessageBox::critical(this, "Error", "Failed to select students for deletion.");
                return;
            }

            while (selectStudentsQuery.next()) {
                QString studentID = selectStudentsQuery.value("idnumber").toString();
                QString studentName = selectStudentsQuery.value("firstname").toString() + " " + selectStudentsQuery.value("lastname").toString();
                QString studentCourseCode = selectStudentsQuery.value("course").toString();

                if (studentCourseCode != courseCode) {
                    QMessageBox::warning(this, "Error", "Student " + studentName + " (ID: " + studentID + ") is not enrolled in the selected course. Aborting deletion.");
                    return;
                }
            }

            QSqlQuery deleteStudentsQuery;
            deleteStudentsQuery.prepare("DELETE FROM students WHERE course = :courseCode");
            deleteStudentsQuery.bindValue(":courseCode", courseCode);
            if (!deleteStudentsQuery.exec()) {
                qDebug() << "Failed to delete students:" << deleteStudentsQuery.lastError().text();
                QMessageBox::critical(this, "Delete Error", "Failed to delete students: " + deleteStudentsQuery.lastError().text());
                return;
            }

            QSqlQuery deleteQuery;
            deleteQuery.prepare("DELETE FROM courses WHERE coursecode = :courseCode AND coursename = :courseName");
            deleteQuery.bindValue(":courseCode", courseCode);
            deleteQuery.bindValue(":courseName", courseName);
            if (!deleteQuery.exec()) {
                QMessageBox::critical(this, "Error", "Failed to delete course.");
                db.rollback();
                return;
            }

            ui->code->clear();
            ui->name->clear();

            QMessageBox::information(this, "Delete", "Data deleted.");

            updateStudentsTable();
            updateCoursesTable();
        }
    }
    else
    {
        QMessageBox::warning(this, "Delete Error", "No data selected for deletion.");
    }
}
