//
//  TYLIBSNDFileDecorder.m
//  Tukuyomi
//
//  Created by toveta on Sat Sep 28 2002.
//  Copyright (c) 2002 toveta All rights reserved.
//

#include "TYLIBSNDFileDecorder.h"
#include <sndfile.h>
#include <QCoreApplication>
#include <QFile>
#include <QDir>
#include <cstdlib>
#include <cstdio>

TYLIBSNDFileDecorder::TYLIBSNDFileDecorder(QObject* parent)
    : QObject(parent)
    , m_samples(nullptr)
{
}

TYLIBSNDFileDecorder::~TYLIBSNDFileDecorder()
{
}

bool TYLIBSNDFileDecorder::initWithData(const QByteArray& sourceData)
{
    m_tempPath = QCoreApplication::applicationDirPath() + "/.TEMP";
    
    QFile tempFile(m_tempPath);
    if (!tempFile.open(QIODevice::WriteOnly)) {
        return false;
    }
    tempFile.write(sourceData);
    tempFile.close();
    
    return true;
}

bool TYLIBSNDFileDecorder::getSamples(short** samples_ptr,
                                       int* frames,
                                       int* channels,
                                       int* rate)
{
    SNDFILE *infile;
    SF_INFO in_info;

    /*
    if((in_info = malloc(sizeof(SF_INFO))) == NULL){
        NSLog(@"can't allocate SF_INFO");
        return NO;
    }
    */
    
    memset(&in_info, 0, sizeof(in_info));
    
    QByteArray tempPathLocal = m_tempPath.toLocal8Bit();
    if ((infile = sf_open(tempPathLocal.constData(), SFM_READ, &in_info)) == nullptr) {
        sf_perror(infile);
        return false;
    }
    
    m_samples = (short*)malloc(sizeof(short) * in_info.frames * in_info.channels);
    if (m_samples == nullptr) {
        sf_perror(infile);
        sf_close(infile);
        return false;
    }
    
    if (sf_readf_short(infile, m_samples, in_info.frames) != in_info.frames) {
        sf_perror(infile);
        free(m_samples);
        m_samples = nullptr;
        sf_close(infile);
        return false;
    }

    *samples_ptr = m_samples;
    *frames = in_info.frames;
    *channels = in_info.channels;
    *rate = in_info.samplerate;
    
    sf_close(infile);
    
    return true;
}

TYLIBSNDFileDecorder::~TYLIBSNDFileDecorder()
{
    if (!m_tempPath.isEmpty()) { QFile::remove(m_tempPath); }
    
    if (m_samples != nullptr) { free(m_samples); m_samples = nullptr; }

}
