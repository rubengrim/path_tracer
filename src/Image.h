#pragma once
#include <stdint.h>

typedef unsigned int GLuint;

class Image
{
public:
	Image(int width, int height);

	void InitializeTexture();
	bool UploadDataToTexture(uint32_t* imageData);

	void Resize(int width, int height);
	void SetImageData(uint32_t* imageData);

	inline int GetWidth() { return m_Width; }
	inline int GetHeight() { return m_Height; }
	inline unsigned int GetTextureId() { return m_TextureID; }

private:
	int m_Width = 0;
	int m_Height = 0;
	GLuint m_TextureID = 0;
};