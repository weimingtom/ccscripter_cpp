//
//  TYLIBSNDFileDecorder.h
//  Tukuyomi
//
//  Created by toveta on Sat Sep 28 2002.
//  Copyright (c) 2002 toveta All rights reserved.
//

#ifndef TYLIBSNDFILEDECORDER_H
#define TYLIBSNDFILEDECORDER_H

#include "TYWaveDecorder.h"
#include <QObject>
#include <QString>
#include <QByteArray>

class TYLIBSNDFileDecorder : public QObject, public TYWaveDecorder {
    Q_OBJECT
public:
    explicit TYLIBSNDFileDecorder(QObject* parent = nullptr);
    virtual ~TYLIBSNDFileDecorder();

    // TYWaveDecorder interface
    virtual bool initWithData(const QByteArray& sourceData) override;
    virtual bool getSamples(short** samples_ptr,
                           int* frames,
                           int* channels,
                           int* rate) override;

private:
    QString m_tempPath;
    short* m_samples;
};

#endif // TYLIBSNDFILEDECORDER_H
