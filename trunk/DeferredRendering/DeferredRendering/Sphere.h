#ifndef SPHERE_H_
#define SPHERE_H_

#pragma once
#include "Vector.h"

namespace MocapGE
{
	template <typename T>
	class Sphere
	{
	public:
		Sphere(void);
		~Sphere(void);

		Vec3<T>& Center()
		{
			return center_;
		}

		T& Raduis()
		{
			return raduis_;
		}

	private:
		Vec3<T> center_;
		T raduis_; 
	};

}

#endif

