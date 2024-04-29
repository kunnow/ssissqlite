#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlTableModel>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QPushButton>
#include <QTableWidget>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QLineEdit *searchLineEdit;
    QLineEdit *lastnameLineEdit;
    QLineEdit *idnumberLineEdit;
    QLineEdit *selectedDataLineEdit;
    QSpinBox *yearlevelSpinBox;
    QComboBox *genderComboBox;
    QLineEdit *firstnameLineEdit;
    QLineEdit *middlenameLineEdit;
    QLineEdit *courseLineEdit;
    QModelIndex selectedIndex;
    QPushButton *EditButton;
    QPushButton *DeleteButton;

    void studentsEditable(bool editable);
    void coursesEditable(bool editable);

private slots:
    void on_savebutton_clicked();
    void on_editbutton_clicked();
    void on_reload_clicked();
    void on_addbutton_clicked();
    //void search();
    //void updateTables();
    void updateStudentsTable();
    //void searchAndPopulateTables();
    //void on_searchstudent_clicked();
    void on_searchbutton_clicked();
    void displayAllData();
    void populateTable(QTableWidget *tableWidget, QSqlQuery &query);
    void rowstudents(QTableWidgetItem *item);
    void rowcourses(QTableWidgetItem *item);
    void on_addcourse_clicked();
    void on_deletebutton_clicked();
    void on_tableWidget_itemSelectionChanged();
    void on_editcourse_clicked();
    void on_savecourse_clicked();
    void on_deletecourse_clicked();
    void editCourse(const QString& courseCode);
    void callDatabase();

    void on_code_textChanged(const QString &text);
private:
    Ui::MainWindow *ui;
    QSqlTableModel *model;

    bool editMode;
    QTableWidget *tableWidget;
    QSqlDatabase db;

    void searchInTable(QTableWidget *tableWidget, const QString &searchText);
    void resetTableVisibility(QTableWidget *tableWidget);
    void updateCoursesTable();
    QString originalCourseCode;
    void updateOriginalCourseCode();

};
#endif // MAINWINDOW_H
