#ifndef CIMAGECAPTURE_H
#define CIMAGECAPTURE_H

#include "stdafx.h"

const int MAX_IMAGE_SIZE = 600;

class CImageCapture
{
    int m_nFileCounter;
    bool m_bStartCapture;
    void SnapShot(int ox,int oy, int wWidth, int wHeight, char* filename);
public:
    void StartCapture();
    void StopCapture();
    void Capturing();
    CImageCapture();
    ~CImageCapture();
};

#endif // CIMAGECAPTURE_H
