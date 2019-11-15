/*************************************************************************
 *
 * Copyright (c) 2017-2019, Klaralvdalens Datakonsult AB (KDAB)
 * All rights reserved.
 *
 * See the LICENSE.txt file shipped along with this file for the license.
 *
 *************************************************************************/

#ifndef SETUP_H
#define SETUP_H

class QSqlDatabase;

namespace Setup
{
    /**
     * Create an empty database, removing any previous data
     */
    QSqlDatabase createDatabase();

    /**
     * Create the table schema in the database
     */
    bool createTables(const QSqlDatabase &db);

    /**
     * Put some mock data into the tables
     */
    bool fillTables(const QSqlDatabase &db, int maxUsers);
}

#endif // SETUP_H
