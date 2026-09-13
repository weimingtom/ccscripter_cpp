//
//  TYFullScreenContentView.h
//  Tukuyomi
//
//  Created by toveta on Sat Feb 16 2002.
//  Copyright (c) 2001 toveta All rights reserved.
//

#ifndef TYFULLSCREENCONTENTVIEW_H
#define TYFULLSCREENCONTENTVIEW_H

#include <QWidget>

class TYFullScreenContentView : public QWidget {
    Q_OBJECT

public:
    explicit TYFullScreenContentView(QWidget* parent = nullptr);
    ~TYFullScreenContentView();

protected:
    void paintEvent(QPaintEvent* event) override;
};

#endif // TYFULLSCREENCONTENTVIEW_H
