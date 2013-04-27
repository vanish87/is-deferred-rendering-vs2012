#ifndef CONFIGURE_H_
#define CONFIGURE_H_

#pragma once

#include <string>

namespace MocapGE
{
	struct RenderSetting
	{
		bool	full_screen;
		int		left;
		int		top;
		int		width;
		int		height;

		bool	msaa4x;
		bool	gamma;

		bool	deferred_rendering;
		int		gbuffer_size;

	};
	struct ContextConfig
	{
		std::string render_factory_name;

		RenderSetting render_setting;
	};
}

#endif