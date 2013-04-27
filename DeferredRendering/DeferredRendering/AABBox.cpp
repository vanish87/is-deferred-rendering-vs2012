#include "AABBox.h"
#include <limits>
#include "Math.h"

namespace MocapGE
{
	AABBox::AABBox(void)
	{
	}


	AABBox::~AABBox(void)
	{
	}


	AABBox::AABBox(std::vector<VertexType*> vertice, uint32_t vcout )
	{
		ComputeAABB(vertice, vcout);
	}

	void AABBox::ComputeAABB( std::vector<VertexType*> vertice, uint32_t vcout )
	{
		float3 min( std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
		float3 max(-std::numeric_limits<float>::max(),-std::numeric_limits<float>::max(),-std::numeric_limits<float>::max());
		for(size_t i=0; i< vcout; i++)
		{
			float3 pos = vertice[i]->position;

			max = Math::Max(pos, max);
			min = Math::Min(pos, min);
		}

		max_ = max;
		min_ = min;
	}

	MocapGE::float3 AABBox::Min()
	{
		return min_;
	}

	MocapGE::float3 AABBox::Max()
	{
		return max_;
	}

}
