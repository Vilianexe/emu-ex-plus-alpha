#pragma once

/*  This file is part of Imagine.

	Imagine is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Imagine is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Imagine.  If not, see <http://www.gnu.org/licenses/> */

#include <imagine/config/defs.hh>
#include <imagine/gfx/defs.hh>
#include <imagine/gfx/TextureConfig.hh>
#include <imagine/gfx/TextureSamplerConfig.hh>
#include <imagine/util/typeTraits.hh>

class GfxImageSource;

namespace Gfx
{

class Renderer;
class TextureSampler;
class PixmapTexture;

class GLTextureSampler
{
public:
	constexpr GLTextureSampler() {}
	GLTextureSampler(Renderer &r, TextureSamplerConfig config);
	~GLTextureSampler();
	void setTexParams(GLenum target) const;
	void deinit();
	GLuint name() const;
	const char *label() const;

protected:
	Renderer *r{};
	GLuint name_ = 0;
	GLenum minFilter = 0;
	GLenum magFilter = 0;
	GLenum xWrapMode_ = 0;
	GLenum yWrapMode_ = 0;
	[[no_unique_address]] IG::UseTypeIf<Config::DEBUG_BUILD, const char *> debugLabel{};
};

using TextureSamplerImpl = GLTextureSampler;

class GLLockedTextureBuffer
{
public:
	constexpr GLLockedTextureBuffer() {}
	constexpr GLLockedTextureBuffer(void *bufferOffset, IG::Pixmap pix, IG::WindowRect srcDirtyRect,
		uint16_t lockedLevel, bool shouldFreeBuffer, GLuint pbo = 0):
		bufferOffset_{bufferOffset}, pix{pix}, srcDirtyRect{srcDirtyRect}, pbo_{pbo},
		lockedLevel{lockedLevel}, shouldFreeBuffer_{shouldFreeBuffer}
	{}
	uint16_t level() const { return lockedLevel; }
	GLuint pbo() const { return pbo_; }
	bool shouldFreeBuffer() const { return shouldFreeBuffer_; }
	void *bufferOffset() const { return bufferOffset_; }

protected:
	void *bufferOffset_{};
	IG::Pixmap pix{};
	IG::WindowRect srcDirtyRect{};
	GLuint pbo_ = 0;
	uint16_t lockedLevel = 0;
	bool shouldFreeBuffer_ = false;
};

using LockedTextureBufferImpl = GLLockedTextureBuffer;

class GLTexture
{
public:
	constexpr GLTexture() {}
	GLTexture(Renderer &r, TextureConfig config, IG::ErrorCode *errorPtr = nullptr);
	GLTexture(Renderer &r, GfxImageSource &img, bool makeMipmaps, IG::ErrorCode *errorPtr = nullptr);
	~GLTexture();
	IG::ErrorCode init(Renderer &r, TextureConfig config);
	TextureConfig baseInit(Renderer &r, TextureConfig config);
	GLuint texName() const;
	void bindTex(RendererCommands &cmds, const TextureSampler &sampler) const;
	bool canUseMipmaps(Renderer &r) const;
	bool isExternal() const;

protected:
	Renderer *r{};
	TextureRef texName_ = 0;
	mutable GLuint sampler = 0; // used when separate sampler objects not supported
	IG::PixmapDesc pixDesc;
	uint16_t levels_ = 0;
	#ifdef CONFIG_GFX_OPENGL_SHADER_PIPELINE
	uint8_t type_ = TEX_UNSET;
	#else
	static constexpr uint8_t type_ = TEX_2D_4;
	#endif

	void deinit();
	static void setSwizzleForFormat(Renderer &r, IG::PixelFormatID format, GLuint tex, GLenum target);
	void updateFormatInfo(IG::PixmapDesc desc, uint16_t levels, GLenum target = GL_TEXTURE_2D);
	void updateLevelsForMipmapGeneration();
	GLenum target() const;
};

using TextureImpl = GLTexture;

class GLPixmapTexture
{
public:
	constexpr GLPixmapTexture() {}
	IG::ErrorCode init(PixmapTexture &self, TextureConfig config);
	void updateUsedPixmapSize(IG::PixmapDesc usedDesc, IG::PixmapDesc fullDesc);

protected:
	GTexCPoint uv{};
	IG::WP usedSize{};
};

using PixmapTextureImpl = GLPixmapTexture;

}
