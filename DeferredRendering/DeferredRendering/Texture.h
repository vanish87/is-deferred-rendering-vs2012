#ifndef TEXTURE_H_
#define TEXTURE_H_

#pragma once

#include "PreDec.h"
#include "TypeAndFormat.h"

namespace MocapGE
{
	class Texture
	{
	public:
		Texture(void);
		Texture(AccessType access_type, uint32_t array_size, 
				Format format, uint32_t mip_level, uint32_t sample_count, uint32_t sample_quality, TextureUsage usage, TextureType type);
		~Texture(void);

		void SetUsage(TextureUsage usage);
		TextureType GetType();
	protected:
		AccessType access_type_;
		uint32_t array_size_;
		Format format_;
		uint32_t mip_level_;
		uint32_t sample_count_;
		uint32_t sample_quality_;
		TextureUsage usage_;
		TextureType type_;
	};
}

#endif

