/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#ifndef SWMODEL_H
#define SWMODEL_H

#include <QObject>
#include <QList>
#include <feedmodel.h>

#include <libsocialweb-qt/swclientdbustypes.h>

class SwClientService;
class SwClientItemView;
class SwClientItem;

class McaActions;

struct SwItem {
    SwClientItem *item;
    McaActions *action;
};

class SwModel: public McaFeedModel
{
    Q_OBJECT

public:
    SwModel(SwClientService *clientService, QObject *parent = 0);
    ~SwModel(){}

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;

protected slots:
    void performAction(QString action, QString uniqueid);

private slots:
    void onItemsAdded(QList<SwClientItem*> items);
    void onItemsChanged(QList<SwClientItem*> items);
    void onItemsRemoved(ArrayOfSwItemId items);

private:
    void openView();
    void closeView();

    SwClientService *mService;
    SwClientItemView *mView;
    QList<struct SwItem> mItems;
};

#endif  // SWMODEL_H
