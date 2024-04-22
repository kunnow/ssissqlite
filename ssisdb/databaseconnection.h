#ifndef DATABASECONNECTION_H
#define DATABASECONNECTION_H
#include <QSqlDatabase>

class databaseconnection
{
private:
    QSqlDatabase database;

public:
    databaseconnection();
    bool Connect();
    void Disconnect();
};

#endif // DATABASECONNECTION_H
