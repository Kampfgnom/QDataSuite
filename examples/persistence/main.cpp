#include <QApplication>

#include <seriesModel/seriesmodel.h>

#include <QPersistence/databaseschema.h>
#include <QPersistence/persistentdataaccessobject.h>
#include <QDataSuite/error.h>

#include <QDebug>
#include <QSqlError>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Database connection
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("test.sqlite");
    if(!db.open()) {
        qCritical() << db.lastError();
    }

    // Register types
    QDataSuite::registerMetaObject<Series>();
    QDataSuite::registerMetaObject<Season>();
    QPersistence::PersistentDataAccessObject<Series> seriesDao(db);
    QPersistence::PersistentDataAccessObject<Season> seasonDao(db);
    QDataSuite::registerDataAccessObject<Series>(&seriesDao, db.connectionName());
    QDataSuite::registerDataAccessObject<Season>(&seasonDao, db.connectionName());

    // Drop and create tables
    QPersistence::DatabaseSchema databaseSchema(db);
//    databaseSchema.adjustSchema();
    databaseSchema.createCleanSchema();

    // Series
    QList<int> seriesids;
    seriesids << 1  // Simpsons
              << 2; // white collar

    // Seasons
    QList<QList<int> > seasons;
    seasons << (QList<int>() << 11 << 12); // Simpsons
    seasons << (QList<int>() << 21 << 22); // White collar

    // Insert
    for(int i = 0; i < seriesids.size(); ++i) {
        Series *series = seriesDao.create();
        QScopedPointer<Series> s(series);
        series->setTvdbId(seriesids.at(i));

        if(!seriesDao.insert(series)) {
            qCritical() << seriesDao.lastError();
        }

        QList<int> seasonIds = seasons.at(i);
        for(int j = 0; j < seasonIds.size(); ++j) {
            Season *season = seasonDao.create();
            QScopedPointer<Season> se(season);
            season->setTvdbId(seasonIds.at(j));
            season->setSeries(series);

            if(!seasonDao.insert(season)) {
                qCritical() << seasonDao.lastError();
            }
        }
    }

    // Update
    foreach(Season *season, seasonDao.readAll()) {
        QScopedPointer<Season> s(season);
        season->setNumber(123);
        seasonDao.update(season);
    }

    foreach(Series *series, seriesDao.readAll()) {
        QScopedPointer<Series> s(series);
        series->setImdbId("asd");
        seriesDao.update(series);

        foreach(Season *season, series->seasons()) {
            season->setNumber(123);
            seasonDao.update(season);
        }
    }

    // Delete
    foreach(Series *series, seriesDao.readAll()) {
        QScopedPointer<Series> s(series);
        seriesDao.remove(series);
    }

    foreach(Season *season, seasonDao.readAll()) {
        QScopedPointer<Season> s(season);
        seasonDao.remove(season);
    }
    return a.exec();
}
