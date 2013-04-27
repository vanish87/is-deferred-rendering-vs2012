#ifndef AABBOX_H_
#define AABBOX_H_

#pragma once
#include <vector>

#include "Vector.h"
#include "TypeAndFormat.h"

namespace MocapGE
{
	class AABBox
	{
	public:
		AABBox(void);
		~AABBox(void);

		AABBox(std::vector<VertexType*> vertice, uint32_t vcout);

		void ComputeAABB(std::vector<VertexType*> vertice, uint32_t vcout);
		float3 Min();
		float3 Max();

	private:
		Vec3<float> min_;
		Vec3<float> max_;
	};


}
#endif
