/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#include "swservicemodel.h"

#include <QDebug>
#include <QString>
#include <QStringList>
#include <QProcess>

#include <libsocialweb-qt/swclient.h>
#include <libsocialweb-qt/swclientservice.h>
#include <libsocialweb-qt/swclientserviceconfig.h>
#include <actions.h>

SwServiceModel::SwServiceModel(SwClient *client, QObject *parent) :
    McaServiceModel(parent),
    mClient(client),
    mConfig(0)
{
    if (!mClient)
        return;

    connect(mClient, SIGNAL(OnlineChanged(bool)),
            this, SLOT(onOnlineChanged(bool)));
    this->setupServices();
}

SwServiceModel::~SwServiceModel()
{
    foreach (struct SwService swService, mServices) {
        //Have to delete by hand for now, as McaActions constructor doesn't take a parent. This will probably change...
        delete swService.action;
    }
    foreach (struct SwService swService, mSvcUnconf) {
        delete swService.action;
    }
}

int SwServiceModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return mServices.count();
}

QVariant SwServiceModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    if ((row >= mServices.count()) || (row < 0))
        return QVariant();

    switch (role) {
    case CommonDisplayNameRole:
        // This display name is a localized name for your service -- if you
        //   provide more than one service, each should have a distinct name
        //   for example, identifying the account. But it should be a title,
        //   preferably under ~32 characters.
        return mServices.at(row).service->getDisplayName();

    case CommonIconUrlRole:
        // Here you can provide a small icon that identifies the service.
        //   This icon would probably be the same for each account if you
        //   provide multiple accounts as separate "services".
        return mServices.at(row).service->getServiceConfig()->getIconPath();

    case RequiredCategoryRole:
        // Currently we define three categories: "social", "email", and "im"
        //   that will be pulled into the Friends panel. In the future we
        //   may extend the categories for other panels and purposes.
        return "social";

    case RequiredNameRole:
        // This field is a unique name for the service within this plugin
        //   If you provide multiple accounts each should have its own
        //   unique id. This is not user-visible.
        return mServices.at(row).service->getServiceName();

    case CommonActionsRole:
        // This is required if you will ever return true for
        //   CommonConfigErrorRole.
        return QVariant::fromValue<McaActions*>(mServices.at(row).action);

    case CommonConfigErrorRole:
        //If our credentials are invalid, return true
        return (mServices.at(row).service->credsState() == SwClientService::CredsInvalid);

    default:
        // There is also the CommonConfigErrorRole which is a bool and just
        //   requests the UI to alert the user to misconfiguration, and gives
        //   them an opportunity to configure your service. If you return
        //   true here, you must also provide the CommonActionsRole above
        //   with a handler watching for a "configure" action.
        qWarning() << "Unhandled data role requested!";
        return QVariant();
    }
}

void SwServiceModel::performAction(QString action, QString uniqueid)
{
    // The mtfcontent sample application provides Configure buttons for each
    //   service you report so you can test that you are receiving the
    //   configure signal properly. In the real application, we plan to only
    //   provide this option to the user if you report that there is a
    //   configuration error through CommonConfigErrorRole.
    if (action == "configure")
        configure(uniqueid);
    else
        qDebug() << QString("Unhandled service action in meego-content-plugin-socialweb: %1 - %2").arg(action, uniqueid);
}


//Private slots:

void SwServiceModel::onCredsStateChanged(SwClientService *service, SwClientService::CredsState state) {
    Q_UNUSED(state);

    int i;
    for (i = 0; i < mServices.count(); ++i) {
        struct SwService swService = mServices.at(i);
        if (swService.service->getServiceName() == service->getServiceName()) {
            QModelIndex qmi = this->createIndex(i, 0, 0);
            emit this->dataChanged(qmi, qmi);
        }
    }
}

void SwServiceModel::onIsConfiguredChanged(SwClientService *service, bool isConfigured) {
    //If the service is now configured, remove it from the unconfigured list, add it
    //to the main list.
    int i;
    if (isConfigured) {
        for (i = 0; i < mSvcUnconf.count(); ++i) {
            struct SwService swService = mSvcUnconf.at(i);
            if (swService.service->getServiceName() == service->getServiceName()) {
                this->beginInsertRows(QModelIndex(), mServices.count(), mServices.count());
                mServices.append(swService);
                this->endInsertRows();
                mSvcUnconf.removeAt(i);
                break;
            }
        }

    } else {
        //Otherwise, do the opposite
        for  (i = 0; i < mServices.count(); ++i) {
            struct SwService swService = mServices.at(i);
            if (swService.service->getServiceName() == service->getServiceName()) {
                mSvcUnconf.append(swService);
                this->beginRemoveRows(QModelIndex(), i, i);
                mServices.removeAt(i);
                this->endInsertRows();
                break;
            }
        }
    }
}

void SwServiceModel::onOnlineChanged(bool isOnline)
{
    //If we're coming back online, force full service refresh...
    if (isOnline)
        setupServices();

}

void SwServiceModel::onConfigProcessFinished(int status)
{
    qDebug() << QString("Config process finished with status %1").arg(QString::number(status));
}

void SwServiceModel::onConfigProcessError(QProcess::ProcessError procError)
{
    Q_UNUSED(procError);
    qDebug() << QString("Config process error: %1!").arg(mConfig->errorString());
}

//Private functions:

void SwServiceModel::configure(QString serviceName)
{
    qDebug() << "Configure called for:" << serviceName;
    //TODO - figure out how to handle multiple platforms/different methods of launching settings...
    QString app = "/usr/bin/meego-qml-launcher";
    QStringList args;
    args << "--app" << "meego-ux-settings"
            << "--opengl" << "--fullscreen" << "--cmd" <<  "showPage"
            << "--cdata" << "Web Accounts"; //,%1").arg(serviceName);

    mConfig = new QProcess(this);
    connect(mConfig,
            SIGNAL(finished(int)),
            this,
            SLOT(onConfigProcessFinished(int)));
    connect(mConfig,
            SIGNAL(error(QProcess::ProcessError)),
            this,
            SLOT(onConfigProcessError(QProcess::ProcessError)));
    mConfig->setProcessChannelMode(QProcess::ForwardedChannels);
    qDebug() << QString("About to start %1, with args ").arg(app) << args;
    mConfig->startDetached(app, args);
//    if (config->e)
}

void SwServiceModel::setupServices()
{
    if (mServices.count()) {
        this->beginRemoveRows(QModelIndex(), 0, mServices.count()-1);
        mServices.clear();
        this->endRemoveRows();
    }
    if (mSvcUnconf.count())
        mSvcUnconf.clear();
    foreach (QString svc, mClient->getServices()) {
        struct SwService swService;
        if (!(swService.service = mClient->getService(svc)))
            continue;
        //If we can't open a view, or we're not configured, don't bother listing...
        if (!swService.service->hasOpenView()) {
            delete swService.service;
            continue;
        }
        connect(swService.service,
                SIGNAL(CredsStateChanged(SwClientService*,SwClientService::CredsState)),
                this,
                SLOT(onCredsStateChanged(SwClientService *, SwClientService::CredsState)));
        connect(swService.service,
                SIGNAL(IsConfiguredChanged(SwClientService*,bool)),
                this,
                SLOT(onIsConfiguredChanged(SwClientService *, bool)));
        swService.action = new McaActions();
        connect(swService.action,
                SIGNAL(standardAction(QString,QString)),
                this,
                SLOT(performAction(QString,QString)));
        //We only list them if they're configured...
        if (swService.service->isConfigured()) {
            this->beginInsertRows(QModelIndex(), mServices.count(), mServices.count());
            mServices.append(swService);
            this->endInsertRows();
        } else {
            mSvcUnconf.append(swService);
        }
    }
}
