#include "databaseconnection.h"
#include <qdebug.h>

databaseconnection::databaseconnection()
{
    database = QSqlDatabase::addDatabase("QSQLITE");
    database.setDatabaseName("C://Users//Shir Keilah//Documents//ssisdb//database.db");
}

bool databaseconnection::Connect()
{
    if (database.open())
        return true;
    else return false;
}

void databaseconnection::Disconnect()
{
    database.close();
}
