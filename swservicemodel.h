/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#ifndef SWSERVICEMODEL_H
#define SWSERVICEMODEL_H

#include <QList>
#include <QProcess>

#include <servicemodel.h>
#include <libsocialweb-qt/swclientservice.h>

class McaActions;

class SwClient;

struct SwService {
    SwClientService *service;
    McaActions *action;
};

class SwServiceModel: public McaServiceModel
{
    Q_OBJECT

public:
    SwServiceModel(SwClient *client, QObject *parent = NULL);
    ~SwServiceModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;

public slots:
    void performAction(QString action, QString uniqueid);

private slots:
    void onCredsStateChanged(SwClientService *service, SwClientService::CredsState state);
    void onIsConfiguredChanged(SwClientService *service, bool isConfigured);
    void onOnlineChanged(bool isOnline);

    void onConfigProcessFinished(int status);
    void onConfigProcessError(QProcess::ProcessError procError);

private:
    void configure(QString serviceName);
    void setupServices();

    SwClient *mClient;
    QList<struct SwService> mServices;
    QList<struct SwService> mSvcUnconf;

    QProcess *mConfig;

};

#endif  // SWSERVICEMODEL_H
