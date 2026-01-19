#ifndef __LOCALDATA_H__
#define __LOCALDATA_H__
#include "LocalDBMap.h"
#include <cstddef>
#include <qcontainerfwd.h>
#include <qlogging.h>
#include <qobject.h>
#include "qmap.h"
#include "QSharedData"
class LocalDataOfUser{
    private:
    QString user = "null";
    QSharedPointer<CloudLocalMapping> mapping;
     
    public:
    LocalDataOfUser(QString user = "default")
        :user(user),
        mapping(new CloudLocalMapping(user)){

    }

    inline static QSharedPointer<LocalDataOfUser> getUser(QString user = "default"){
        static QSharedPointer<LocalDataOfUser> data;

        if((!data)||(user!=data->user)){
            data.reset(new LocalDataOfUser(user));
        }
        return data;
    }
    inline QSharedPointer<CloudLocalMapping> getMapping(){
        return mapping;
    }

};
#endif // __LOCALDATA_H__