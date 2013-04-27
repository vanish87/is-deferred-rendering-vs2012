#ifndef RENDERSTATE_H_
#define RENDERSTATE_H_


#pragma once
#include "PreDec.h"
#include "TypeAndFormat.h"

namespace MocapGE
{
	class RenderState
	{
	public:
		RenderState(void);
		~RenderState(void);

	protected:
		RenderStateType state_type_;

	};

}
#endif
