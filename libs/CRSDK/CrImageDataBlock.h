#ifndef CRIMAGEDATABLOCK_H
#define CRIMAGEDATABLOCK_H

#include "CrDefines.h"

#if defined(_MSC_VER)
// Windows definitions
	#ifdef CR_SDK_EXPORTS
		#define SCRSDK_API __declspec(dllexport)
	#else
		#define SCRSDK_API __declspec(dllimport)
	#endif
// End Windows definitions
#else
	#if defined(__GNUC__)
		#ifdef CR_SDK_EXPORTS
			#define SCRSDK_API __attribute__ ((visibility ("default")))
		#else
			#define SCRSDK_API
		#endif
	#endif
#endif

namespace SCRSDK
{

class SCRSDK_API CrImageInfo
{
public:
	CrImageInfo();
	~CrImageInfo();

	CrInt32u GetBufferSize();

private:
	CrInt32u width;
	CrInt32u height;
	CrInt32u bufferSize;
};

class SCRSDK_API CrImageDataBlock
{
public:
	CrImageDataBlock();
	~CrImageDataBlock();

	CrInt32u GetFrameNo();

	void SetSize(CrInt32u size);
	CrInt32u GetSize();

	void SetData(CrInt8u* data);

	CrInt32u GetImageSize();
	CrInt8u* GetImageData();

	CrInt32u GetTimeCode();

private:
	CrInt32u frameNo;
	//Buffer size (byte) of pData.
	CrInt32u size;
	//Buffer to store the image body.
	CrInt8u* pData;
	//Size (byte) of image body.
	CrInt32u imageSize;
	//SMPTE 12M time-code from Monitoring library.
	CrInt32u timeCode;
};

// Twice the maximum size of the OSD image 
// # Calculate the method : ((640[width] x 480[height] x 4 + 4096[memory alignment]) x 2)
#define CR_OSD_IMAGE_MAX_SIZE	(((640 * 480 * 4) + 4096) *2)

enum CrIsLvPosExist : CrInt32u
{
	CrIsLvPosExist_Disable = 0,
	CrIsLvPosExist_Enable,
};

class SCRSDK_API CrOSDImageMetaInfo
{
public:
	CrOSDImageMetaInfo();
	~CrOSDImageMetaInfo();

	CrIsLvPosExist isLvPosExist;
	CrInt32u osdWidth;
	CrInt32u osdHeight;
	CrInt32u lvPosX;
	CrInt32u lvPosY;
	CrInt32u lvWidth;
	CrInt32u lvHeight;
	CrInt32u degree;
};

class SCRSDK_API CrOSDImageDataBlock
{
public:
	CrOSDImageDataBlock();
	~CrOSDImageDataBlock();

	CrInt32u GetFrameNo();
	CrInt32u GetImageSize();
	CrInt8u* GetImageData();
	void SetData(CrInt8u* data);
	CrOSDImageMetaInfo GetMetaInfo();

private:
	CrInt32u frameNo;
	CrInt8u* pData;
	CrInt32u imageSize;
	CrOSDImageMetaInfo metaInfo;
};

}
#endif //CRIMAGEDATABLOCK_H