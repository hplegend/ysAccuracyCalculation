#include "CImageCapture.h"

CImageCapture::CImageCapture()
{
    m_nFileCounter = 0;
}

void CImageCapture::SnapShot(int ox,int oy, int wWidth, int wHeight, char* filename)
{
	unsigned char *bmpBuffer = new unsigned char [wWidth*wHeight*3];
    unsigned char temp;
    long size,i = 0;

	if  (!bmpBuffer) return;

	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glReadPixels(ox, oy, wWidth, wHeight, GL_RGB, GL_UNSIGNED_BYTE, bmpBuffer);

	size = 3*wWidth*wHeight;
	//convert to BGR format
    while (i < size)
    {
        temp = bmpBuffer[i];       //grab blue
        bmpBuffer[i] = bmpBuffer[i+2];//assign red to blue
        bmpBuffer[i+2] = temp;     //assign blue to red
        i += 3;     //skip to next blue byte
    }

	FILE *filePtr = fopen(filename,  "wb");
	if (!filePtr)
		return;

	BITMAPFILEHEADER  bitmapFileHeader;
	BITMAPINFOHEADER  bitmapInfoHeader;

	bitmapFileHeader.bfType = 0x4D42;  //"BM"
	bitmapFileHeader.bfSize =  wWidth*wHeight*3;
	bitmapFileHeader.bfReserved1 =  0;
	bitmapFileHeader.bfReserved2 = 0;
	bitmapFileHeader.bfOffBits  =
		sizeof(BITMAPFILEHEADER) +  sizeof(BITMAPINFOHEADER);

	bitmapInfoHeader.biSize =  sizeof(BITMAPINFOHEADER);
	bitmapInfoHeader.biWidth =  wWidth;
	bitmapInfoHeader.biHeight =  wHeight;
	bitmapInfoHeader.biPlanes = 1;
	bitmapInfoHeader.biBitCount  = 24;
	bitmapInfoHeader.biCompression =  BI_RGB;
	bitmapInfoHeader.biSizeImage = 0;
	bitmapInfoHeader.biXPelsPerMeter  = 0;
	bitmapInfoHeader.biYPelsPerMeter = 0;
	bitmapInfoHeader.biClrUsed = 0;
	bitmapInfoHeader.biClrImportant =  0;

	fwrite(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1,  filePtr);
	fwrite(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1,  filePtr);
	fwrite(bmpBuffer, size, 1,  filePtr);
	fclose(filePtr);

	delete []bmpBuffer;
}

void CImageCapture::StartCapture()
{
    m_nFileCounter = 0;
    m_bStartCapture = true;
    printf("Start capturing...\n");
}

void CImageCapture::StopCapture()
{
    m_bStartCapture = false;
    printf("Stop capturing...\n");
}

void CImageCapture::Capturing()
{
    char m_saveFileName[128];
    GLint view[4];

    if(!m_bStartCapture) return;

	if(m_nFileCounter<MAX_IMAGE_SIZE)
	{
		if(m_nFileCounter<10)
			sprintf(m_saveFileName,".\\images\\snap00%d.bmp",m_nFileCounter);
		else if(m_nFileCounter>99)
			sprintf(m_saveFileName,".\\images\\snap%d.bmp",m_nFileCounter);
		else
			sprintf(m_saveFileName,".\\images\\snap0%d.bmp",m_nFileCounter);
		glGetIntegerv(GL_VIEWPORT,view);
		SnapShot(view[0],view[1],view[2]-view[0],view[3]-view[0], m_saveFileName);
		m_nFileCounter++;
	}
	else
	{
	    m_bStartCapture = false;
	    printf("Stop capturing...\n");
	}
}

CImageCapture::~CImageCapture()
{
    //dtor
}
