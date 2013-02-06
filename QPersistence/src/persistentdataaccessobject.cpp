#include <QPersistence/persistentdataaccessobject.h>

#include <QDataSuite/error.h>
#include <QtCore/QVariant>

namespace QPersistence {

class PersistentDataAccessObjectBasePrivate : public QSharedData
{
public:
    SqlDataAccessObjectHelper *sqlDataAccessObjectHelper;
    QDataSuite::MetaObject metaObject;
};

PersistentDataAccessObjectBase::PersistentDataAccessObjectBase(const QMetaObject &metaObject,
                                                               const QSqlDatabase &database,
                                                               QObject *parent) :
    AbstractDataAccessObject(parent),
    d(new PersistentDataAccessObjectBasePrivate)
{
    d->sqlDataAccessObjectHelper = SqlDataAccessObjectHelper::forDatabase(database);
    d->metaObject = QDataSuite::MetaObject::metaObject(metaObject);
}

PersistentDataAccessObjectBase::~PersistentDataAccessObjectBase()
{
}

SqlDataAccessObjectHelper *PersistentDataAccessObjectBase::sqlDataAccessObjectHelper() const
{
    return d->sqlDataAccessObjectHelper;
}

QDataSuite::MetaObject PersistentDataAccessObjectBase::dataSuiteMetaObject() const
{
    return d->metaObject;
}

QList<QVariant> PersistentDataAccessObjectBase::allKeys() const
{
    QList<QVariant> result = d->sqlDataAccessObjectHelper->allKeys(d->metaObject);

    if(d->sqlDataAccessObjectHelper->lastError().isValid())
        setLastError(d->sqlDataAccessObjectHelper->lastError());

    return result;
}

QList<QObject *> PersistentDataAccessObjectBase::readAllObjects() const
{
    QList<QObject *> result;
    Q_FOREACH(const QVariant key, allKeys()) result.append(readObject(key));
    return result;
}

QObject *PersistentDataAccessObjectBase::readObject(const QVariant &key) const
{
    QObject *object = createObject();

    if(!d->sqlDataAccessObjectHelper->readObject(d->metaObject, key, object)) {
        setLastError(d->sqlDataAccessObjectHelper->lastError());
        delete object;
        return nullptr;
    }

    return object;
}

bool PersistentDataAccessObjectBase::insertObject(QObject * const object)
{
    if(!d->sqlDataAccessObjectHelper->insertObject(d->metaObject, object)) {
        setLastError(d->sqlDataAccessObjectHelper->lastError());
        return false;
    }

    return true;
}

bool PersistentDataAccessObjectBase::updateObject(QObject *const object)
{
    if(!d->sqlDataAccessObjectHelper->updateObject(d->metaObject, object)) {
        setLastError(d->sqlDataAccessObjectHelper->lastError());
        return false;
    }

    return true;
}

bool PersistentDataAccessObjectBase::removeObject(QObject *const object)
{
    if(d->sqlDataAccessObjectHelper->removeObject(d->metaObject, object)) {
        setLastError(d->sqlDataAccessObjectHelper->lastError());
        return false;
    }

    return true;
}

} // namespace QPersistence
