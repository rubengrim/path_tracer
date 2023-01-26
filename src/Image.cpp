#include "Image.h"
#include "Logger.h"
#include "glad/gl.h"
#include "GLFW/glfw3.h"

Image::Image(int width, int height)
	: m_Width(width), m_Height(height)
{
	InitializeTexture();
}

void Image::InitializeTexture()
{
	// Create a OpenGL texture identifier
	glGenTextures(1, &m_TextureID);
	glBindTexture(GL_TEXTURE_2D, m_TextureID);

	// Setup filtering parameters for display
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same
}

bool Image::UploadDataToTexture(uint32_t* imageData)
{
	if (imageData == nullptr)
	{
		PULSE_WARN("Attempting to upload invalid image data to texture.")
		return false;
	}

#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);

	return true;
}

void Image::Resize(int width, int height)
{
	if (width != m_Width || height != m_Height)
	{
		m_Width = width;
		m_Height = height;
	}
}

void Image::SetImageData(uint32_t* imageData)
{
	UploadDataToTexture(imageData);
}
