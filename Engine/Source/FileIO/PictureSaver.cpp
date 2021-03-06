#include "FileIO/PictureSaver.h"
#include "Frame/TFrame.h"
#include "Common/assertion.h"

#include "Common/ThirdParty/lib_stb.h"

#include <string>
#include <vector>
#include <limits>

namespace ph
{

const Logger PictureSaver::logger(LogSender("Picture Saver"));

bool PictureSaver::save(const LdrRgbFrame& frame, const Path& filePath)
{
	logger.log(ELogLevel::NOTE_MED, "saving image <" + filePath.toString() + ">");

	return saveFrameViaStb(frame, filePath);
}

bool PictureSaver::save(const HdrRgbFrame& frame, const Path& filePath)
{
	const std::string& ext = filePath.getExtension();
	if(ext == ".png" || ext == ".jpg" || ext == ".bmp" || ext == ".tga")
	{
		LdrRgbFrame        ldrFrame(frame.widthPx(), frame.heightPx());
		HdrRgbFrame::Pixel hdrPixel;
		for(uint32 y = 0; y < frame.heightPx(); y++)
		{
			for(uint32 x = 0; x < frame.widthPx(); x++)
			{
				frame.getPixel(x, y, &hdrPixel);
				hdrPixel.mulLocal(255.0_r).addLocal(0.5_r).clampLocal(0.0_r, 255.0_r);
				ldrFrame.setPixel(x, y, LdrRgbFrame::Pixel(hdrPixel));
			}
		}

		return save(ldrFrame, filePath);
	}
	else
	{
		// TODO
		logger.log(ELogLevel::WARNING_MED,
			"file <" + filePath.toString() + "> is an unsupported saving format");

		return false;
	}
}

bool PictureSaver::saveFrameViaStb(const LdrRgbFrame& frame, const Path& path)
{
	stbi_flip_vertically_on_write(true);

	static_assert(sizeof(LdrComponent) == 1);

	const int w    = static_cast<int>(frame.widthPx());
	const int h    = static_cast<int>(frame.heightPx());
	const int comp = 3;

	PH_ASSERT_MSG(w >= 0 && h >= 0, "picture dimension overflow");

	int returnValue = false;

	const std::string& ext = path.getExtension();
	if(ext == ".png")
	{
		const int stride_in_bytes = w * comp;
		returnValue = stbi_write_png(path.toString().c_str(), w, h, comp, frame.getPixelData(), stride_in_bytes);
	}
	else if(ext == ".jpg")
	{
		const int quality = 10;
		returnValue = stbi_write_jpg(path.toString().c_str(), w, h, comp, frame.getPixelData(), quality);
	}
	else if(ext == ".bmp")
	{
		returnValue = stbi_write_bmp(path.toString().c_str(), w, h, comp, frame.getPixelData());
	}
	else if(ext == ".tga")
	{
		returnValue = stbi_write_tga(path.toString().c_str(), w, h, comp, frame.getPixelData());
	}
	else
	{
		logger.log(ELogLevel::WARNING_MED, 
			"file <" + path.toString() + "> has an unsupported saving format");
	}

	return returnValue != 0;
}

}// end namespace ph