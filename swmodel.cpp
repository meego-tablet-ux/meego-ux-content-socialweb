/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#include "swmodel.h"

#include <QDebug>

#include <QDateTime>
#include <QStringList>
#include <QDesktopServices>

#include <libsocialweb-qt/swclientservice.h>
#include <libsocialweb-qt/swclientitemview.h>
#include <libsocialweb-qt/swclientitem.h>

#include <actions.h>

SwModel::SwModel(SwClientService *clientService, QObject *parent):
    McaFeedModel(parent),
    mService(clientService),
    mView(0)

{
    openView();
}
/*
// Developers: Please see annotations in emailmodel.cpp for help on how to
//   implement your own feed model. This one is no different.
SocialModel::SocialModel(QObject *parent): McaFeedModel(parent)
{
    m_size = 15;
    m_socials = new Social[m_size];

    // ensure Socials generated are always the same
    qsrand(0);

    QDateTime date = QDateTime::currentDateTime();
    for (int i=0; i<m_size; i++) {
        date = date.addSecs(-qrand() % 10000);
        m_socials[i].timestamp = date;

        // use consistent uuids for testing
        m_socials[i].uuid = QString::number(20000 + i);

        QStringList list;
        list << "New friend" << "Sarah is listening" <<
                "Photo album uploaded" << "John likes MeeGo";
        int msg = qrand() % list.size();
        m_socials[i].subject = list.at(msg);

        list.clear();
        list << "Alex has accepted your friend request"
                <<
                "Sarah is listening to Beethoven's 5th Symphony"
                <<
                "Photos from the last week of my road trip to Paris"
                <<
                "Hey guys, check out these cool new MeeGo applications";
        m_socials[i].body = list.at(msg);

        connect(&m_socials[i].actions, SIGNAL(standardAction(QString,QString)),
                this, SLOT(performAction(QString,QString)));
    }
}

SocialModel::~SocialModel()
{
    delete [] m_socials;
}
*/

int SwModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return mItems.count();
}

QVariant SwModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    if ((row >= mItems.count()) || (row < 0))
        return QVariant();
//    qDebug() << QString("mItems.count(): ") << mItems.count();
/*
    switch (role) {
    case RequiredTypeRole:
        return QString("content");

    case RequiredUniqueIdRole:
        return m_socials[row].uuid;

    case RequiredTimestampRole:
        return m_socials[row].timestamp;

    case GenericTitleRole:
        return m_socials[row].subject;

    case GenericContentRole:
        return m_socials[row].body;

    case GenericAvatarUrlRole:
        if (index.row() % 2)
            return QString("file://tmp/happy.svg");
        else
            return QString("file://tmp/wink.svg");

    case CommonActionsRole:
        return QVariant::fromValue<McaActions*>(&m_socials[row].actions);

    default:
        return QVariant();
    };
    */
 //   qDebug() << QString("row: %1, mItems.count(): %2").arg(QString::number(row), QString::number(mItems.count()));
//    qDebug() << QString("mItems.at(row): %1/%2").arg(QString::number((int)(mItems.at(row).item)), QString::number((int)(mItems.at(row).action))) ;
    SwItem item = mItems.at(row);
    SwClientItem *swItem = item.item;
    if (!swItem)
        return QVariant();

    switch (role) {
    case RequiredTypeRole:
        {
/*            QString itemType;

            switch (swItem->getItemType()) {
            case SwClientItem::ItemTypeText:
                itemType = "text";
                break;
            case SwClientItem::ItemTypePic:
                itemType = "picture";
                break;
            case SwClientItem::ItemTypeLast:
            default:
                itemType = "unknown";
                qDebug() << QString("Got unknown item type for SwClientItem of service %1! Props:").arg(swItem->getServiceName());
                qDebug() << swItem->getSwItemProps();
                break;
            }

            return QVariant::fromValue<QString>(QString("%1/%2").arg(itemType, swItem->getServiceName()));*/
            return "content";
            break;
        }

    case RequiredUniqueIdRole:
        {
            return QVariant::fromValue<QString>(swItem->getID());
            break;
        }

    case RequiredTimestampRole:
        {
            return QVariant::fromValue<QDateTime>(swItem->getDateTime());
            break;
        }

    case CommonUuidRole:
        {
            return QVariant::fromValue<QString>(swItem->getSwUUID());
            break;
        }

    case GenericTitleRole:
        {
            return QVariant::fromValue<QString>(swItem->getAuthorName());
            break;
        }

//    case GenericIconUrlRole:
//        {
//            //TODO: Ugly - need to resolve this in libsocialweb-qt -
//            //getService gives us a const SwClientService, but the
//            //SwClientService::getServiceConfig is not const, as it will
//            //instanciate a SwClientServiceConfig private member object if
//            //it's not already instanciated...
//            SwClientService *swService = const_cast<SwClientService *>(swItem->getService());

//            QString path = swService->getServiceConfig()->getIconPath();
//            if (path.isEmpty())
//                return QVariant();
//            return QVariant::fromValue<QString>(QString("file://%1").arg(path));
//            break;
//        }

    case GenericRelevanceRole:
        {
            return QVariant::fromValue<qreal>(1.0);
            break;
        }

    case GenericPictureUrlRole:
        {
            QString path = swItem->getThumbnailPath();
            if (path.isEmpty())
                return QVariant();
            return QVariant::fromValue<QString>(QString("file://%1").arg(path));
            break;
        }
    case GenericAvatarUrlRole:
        {
            QString path = swItem->getAuthorIconPath();
            if (path.isEmpty())
                return QVariant();
            return QVariant::fromValue<QString>(QString("file://%1").arg(path));
            break;
        }
    case GenericContentRole:
        {
            //Note that getContent will return the message text for
            //ItemTypeText, and will appropriately fall back to the
            //picture title on ItemTypePic
            return QVariant::fromValue<QString>(swItem->getContent());
        }

    case CommonActionsRole:
        return QVariant::fromValue<McaActions*>(item.action);

    default:
        return QVariant();
    }

    return QVariant();

}

void SwModel::performAction(QString action, QString uniqueid)
{
    qDebug() << "Action" << action << "called for libsocialweb item" << uniqueid;
    //Right now, assume only default action.
    foreach (struct SwItem item, mItems) {
        if (item.item->getID() == uniqueid) {
            QDesktopServices::openUrl(item.item->getURL());
            break;
        }

    }

}

//Private slots:

void SwModel::onItemsAdded(QList<SwClientItem *> items)
{
    foreach (SwClientItem *swItem, items) {
        struct SwItem item;
        item.item = swItem;
        item.action = new McaActions();
        connect(item.action,
                SIGNAL(standardAction(QString,QString)),
                this,
                SLOT(performAction(QString,QString)));
        this->beginInsertRows(QModelIndex(), mItems.count(), mItems.count());
        mItems.append(item);
        this->endInsertRows();
    }
}

void SwModel::onItemsChanged(QList<SwClientItem *> items)
{
    int i;
    //TODO - see if there's a better way than an inner and outer loop...
    foreach (SwClientItem *swItem, items) {
        for (i = 0; i < mItems.count(); ++i) {
            if (swItem->getID() == mItems.at(i).item->getID()) {
                struct SwItem item;
                item.action = mItems.at(i).action;
                item.item = swItem;
                QModelIndex qmi = this->createIndex(i, 0, 0);
                mItems.replace(i, item);
                emit this->dataChanged(qmi, qmi);
                break;
            }
        }
    }
}

void SwModel::onItemsRemoved(ArrayOfSwItemId items)
{
    int i;
    foreach (SwItemId itemID, items) {
        for (i = 0; i < mItems.count(); ++i) {
            struct SwItem item = mItems.at(i);
            if (itemID.uuid == item.item->getSwUUID()) {
                this->beginRemoveRows(QModelIndex(), i, i);
                mItems.removeAt(i);
                this->endInsertRows();
                break;
            }
        }
    }
}

//Private functions:

void SwModel::openView()
{
    //If we had a view open previously and gotten items, this will
    //ensure we don't get duplicate items from the new view...
    this->beginRemoveRows(QModelIndex(), 0, mItems.count());
    mItems.clear();
    this->endRemoveRows();

    mView = mService->openView("feed");
    connect(mView,
            SIGNAL(ItemsAdded(QList<SwClientItem*>)),
            this,
            SLOT(onItemsAdded(QList<SwClientItem*>)));
    connect(mView,
            SIGNAL(ItemsChanged(QList<SwClientItem*>)),
            this,
            SLOT(onItemsChanged(QList<SwClientItem*>)));
    connect(mView,
            SIGNAL(ItemsRemoved(ArrayOfSwItemId)),
            this,
            SLOT(onItemsRemoved(ArrayOfSwItemId)));
    mView->startView();
}

void SwModel::closeView()
{
    mView->stopView();
    mView->closeView();
    delete mView;
    mView = 0;
}
