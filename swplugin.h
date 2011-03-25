/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#ifndef SWPLUGIN_H
#define SWPLUGIN_H

#include <QObject>
#include <QStringList>

#include <feedplugin.h>

class QAbstractItemModel;
class McaSearchableField;
class SwServiceModel;
class SwClient;
class SwClientService;

// A meego-content plugin for libsocialweb
class SwPlugin: public QObject, public McaFeedPlugin
{
    Q_OBJECT
    Q_INTERFACES(McaFeedPlugin)

public:
    explicit SwPlugin(QObject *parent = 0);
    ~SwPlugin() {};

    QAbstractItemModel *serviceModel();
    QAbstractItemModel *createFeedModel(const QString &service);
    McaSearchableFeed *createSearchModel(const QString &service, const QString &search);


private:
    bool checkServiceCaps(const SwClientService *service) const;

    SwServiceModel *mServiceModel;
    SwClient *mClient;
};

#endif  // SWPLUGIN_H
