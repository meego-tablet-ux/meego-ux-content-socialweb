/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

/*
 * MeeGo Content Aggregator - Sample Plugin
 * Copyright © 2010, Intel Corporation.
 */

#include <QDebug>
#include <QtPlugin>

#include <libsocialweb-qt/swclient.h>
#include <libsocialweb-qt/swclientservice.h>

#include "swplugin.h"
#include "swmodel.h"
#include "swservicemodel.h"

SwPlugin::SwPlugin(QObject *parent) :
    QObject(parent),
    McaFeedPlugin(),
    mClient(new SwClient(QDBusConnection::sessionBus(), this))
{
    mServiceModel = new SwServiceModel(mClient, this);
    qDebug("JEA instanciating SwPlugin!");
}

QAbstractItemModel *SwPlugin::serviceModel()
{
    return mServiceModel;
}

QAbstractItemModel *SwPlugin::createFeedModel(const QString &service)
{
    qDebug() << "SwPlugin::createFeedModel: " << service;

    SwClientService *svc = mClient->getService(service);
    if (!svc || !checkServiceCaps(svc))
        return 0;

    return new SwModel(svc, this);
}

McaSearchableFeed *SwPlugin::createSearchModel(const QString &service, const QString &search)
{
    return 0;
}

//Private functions:

bool SwPlugin::checkServiceCaps(const SwClientService *service) const
{
    if (!service->isConfigured())
        return false;
    if ((service->credsState() == SwClientService::CredsValid)
        && (service->hasOpenView()))
        return true;
    return false;
}


Q_EXPORT_PLUGIN2(swplugin, SwPlugin)
