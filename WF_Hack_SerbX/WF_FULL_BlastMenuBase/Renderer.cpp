#include "stdafx.h"

std::hash<std::string> Font::hash_fn;
#define MAX_CHARACTERS 0x60
#define START_CHARACTERS 33
#define END_CHARACTERS 127


void MeasureChars(Gdiplus::Font & font, Gdiplus::Graphics & charGraphics, const uint32_t texWidth, uint32_t& texHeight, float& charHeight, float& spaceWidth)
{
	WCHAR allChars[MAX_CHARACTERS];

	for (WCHAR i = 0; i < MAX_CHARACTERS; ++i)
		allChars[i] = START_CHARACTERS + i;

	allChars[MAX_CHARACTERS - 1] = 0;

	Gdiplus::RectF sizeRect;
	charGraphics.MeasureString(allChars, MAX_CHARACTERS, &font, Gdiplus::PointF(0, 0), &sizeRect);
	charHeight = ceilf(sizeRect.Height);

	int numRows = static_cast<int>(sizeRect.Width / texWidth) + 1;
	texHeight = static_cast<int>(numRows * charHeight) + 1;

	WCHAR charString[2] = { ' ', 0 };
	charGraphics.MeasureString(charString, 1, &font, Gdiplus::PointF(0, 0), &sizeRect);
	spaceWidth = ceilf(sizeRect.Width + 0.5f);
}

int32_t GetCharMinX(Gdiplus::Bitmap & charBitmap)
{
	int32_t width = charBitmap.GetWidth();
	int32_t height = charBitmap.GetHeight();

	for (int32_t x = 0; x < width; ++x)
	{
		for (int32_t y = 0; y < height; ++y)
		{
			Gdiplus::Color color;

			charBitmap.GetPixel(x, y, &color);
			if (color.GetAlpha() > 0)
				return x;
		}
	}

	return 0;
}
int32_t GetCharMaxX(Gdiplus::Bitmap & charBitmap)
{
	int32_t width = charBitmap.GetWidth();
	int32_t height = charBitmap.GetHeight();

	for (int32_t x = width - 1; x >= 0; --x)
	{
		for (int32_t y = 0; y < height; ++y)
		{
			Gdiplus::Color color;

			charBitmap.GetPixel(x, y, &color);
			if (color.GetAlpha() > 0)
				return x;
		}
	}

	return width - 1;
}

HRESULT FontData::CreateFontObjects(const Font& font, FontData* data, std::unique_ptr<uint32_t[]>& lpBitmapBits)
{
	uint32_t& texWidth = data->m_TexWidth,
		&texHeight = data->m_TexHeight;
	float charHeight;
	if (font.getSize() > 60)
		texWidth = 2048;
	else if (font.getSize() > 30)
		texWidth = 1024;
	else if (font.getSize() > 15)
		texWidth = 512;
	else
		texWidth = 256;

	ULONG_PTR token = 0;
	
	Gdiplus::GdiplusStartupInput startup_input(0, TRUE, TRUE);
	Gdiplus::GdiplusStartupOutput startup_output;

	Gdiplus::GdiplusStartup(&token, &startup_input, &startup_output);

	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::wstring font_family = converter.from_bytes(font.getFontFamily());

	HRESULT hr = S_OK;
	{
		Gdiplus::Font Gfont(font_family.c_str(), font.getSize(), font.getFlags(), Gdiplus::UnitPixel);

		Gdiplus::TextRenderingHint hint = Gdiplus::TextRenderingHintAntiAlias;

		int tempSize = static_cast<int>(ceilf(font.getSize() * 2));
		Gdiplus::Bitmap charBitmap(tempSize, tempSize, PixelFormat32bppARGB);
		Gdiplus::Graphics charGraphics(&charBitmap);

		charGraphics.SetPageUnit(Gdiplus::UnitPixel);
		charGraphics.SetTextRenderingHint(hint);
		charGraphics.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);

		MeasureChars(Gfont, charGraphics, texWidth, texHeight, charHeight, data->m_Spacing);

		Gdiplus::Bitmap fontSheetBitmap(texWidth, texHeight, PixelFormat32bppARGB);
		Gdiplus::Graphics fontSheetGraphics(&fontSheetBitmap);

		fontSheetGraphics.SetCompositingMode(Gdiplus::CompositingModeSourceCopy);
		fontSheetGraphics.Clear(Gdiplus::Color(0, 0, 0, 0));

		WCHAR charString[2] = { ' ', 0 };

		Gdiplus::SolidBrush whiteBrush(Gdiplus::Color(255, 255, 255, 255));
		float fontSheetX = 0;
		float fontSheetY = 0;

		for (UINT i = 0; i < MAX_CHARACTERS; ++i)
		{
			charString[0] = static_cast<WCHAR>(START_CHARACTERS + i);
			charGraphics.Clear(Gdiplus::Color(0, 0, 0, 0));
			charGraphics.DrawString(charString, 1, &Gfont, Gdiplus::PointF(1.0f, 0.0f), &whiteBrush);

			int minX = GetCharMinX(charBitmap);
			int maxX = GetCharMaxX(charBitmap);
			int charWidth = maxX - minX + 1;

			if (fontSheetX + charWidth >= texWidth)
			{
				fontSheetX = 0;
				fontSheetY += static_cast<int>(charHeight) + 1;
			}

			data->m_TexCoords[i] = DirectX::XMFLOAT4A(fontSheetX / texWidth, fontSheetY / texHeight, (fontSheetX + charWidth) / texWidth, (fontSheetY + charHeight) / texHeight);

			fontSheetGraphics.DrawImage(&charBitmap, static_cast<int>(fontSheetX), static_cast<int>(fontSheetY), minX, 0, charWidth, static_cast<int>(charHeight), Gdiplus::UnitPixel);

			fontSheetX += charWidth + 1;
		}


		uint32_t len = texWidth * texHeight;
		auto buffer = std::make_unique<uint32_t[]>(len);

		if (!buffer)
		{
			hr = E_OUTOFMEMORY;
		}
		else
		{
			Gdiplus::BitmapData bmData;

			fontSheetBitmap.LockBits(&Gdiplus::Rect(0, 0, texWidth, texHeight), Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &bmData);
			memcpy((void*)buffer.get(), bmData.Scan0, min(len * sizeof(uint32_t), bmData.Width * bmData.Height * sizeof(uint32_t)));
			fontSheetBitmap.UnlockBits(&bmData);
			lpBitmapBits = std::move(buffer);
		}
	}

	Gdiplus::GdiplusShutdown(token);
	return hr;
}

using namespace std::chrono;

high_resolution_clock::time_point lastTime = high_resolution_clock::time_point();
bool Renderer::Begin(const int fps)
{
	auto now = high_resolution_clock::now();
	auto millis = std::chrono::duration_cast<std::chrono::milliseconds, long long, std::nano>(now - lastTime).count();

	auto time = static_cast<int>(1.f / fps * std::milli::den);
	auto milliPerFrame = duration<long, std::milli>(time).count();
	if (millis >= milliPerFrame)
	{
		lastTime = now;
		return true;
	}
	return false;
}

DirectX::XMFLOAT2 Renderer::MeasureText(const FontData* const data, const std::string& strText, float scale)
{
	DirectX::XMFLOAT2A loc(0.f, 0.f);

	float fStartX = loc.x;
	float w = 0.f, z = 0.f;
	for each(char c in strText)
	{
		if (c < 32 || c >= 128)
		{
			if (c == '\n')
			{
				loc.x = fStartX;
				loc.y += (data->m_TexCoords[c - 32].w - data->m_TexCoords[c - 32].y) * data->m_TexHeight * scale;
			}
			else
				continue;
		}

		c -= 33;

		z = loc.x + ((data->m_TexCoords[c].z - data->m_TexCoords[c].x) * data->m_TexWidth  * scale);
		w = loc.y + ((data->m_TexCoords[c].w - data->m_TexCoords[c].y) * data->m_TexHeight * scale);

		if (c != -1)
		{
			loc.x = z + (0.7f * scale);
		}
		else
			loc.x += data->m_Spacing * scale;
	}
	return loc;
}

void FPSCheck(std::string& str)
{
	static high_resolution_clock::time_point lastTime;
	static int ticks = 0;

	auto now = high_resolution_clock::now();
	auto secs = duration_cast<seconds>(now - lastTime);
	ticks++;
	if (secs.count() >= 1)
	{
		str = std::to_string((ticks / secs.count()/2)) + " FPS";
		ticks = 0;
		lastTime = now;
	}
}