//
//Eggache implementing of template methods.
//The implementation of template class is in Math.hpp file.
//The rest of method implementation is in Math.cpp file.

#ifndef MATH_H_
#define MATH_H_

#pragma once

#include "PreDec.h"

namespace MocapGE
{
	//
	namespace Math
	{
		//constant
		float const PI		= 3.141592f;

		template <typename T>
		Matrix<T> Multiply(const Matrix<T> & lhs, const Matrix<T> & rhs);

		float Sqrt(float x);
		float Sin(float x);
		float Cos(float x);
		float ArcCos( float x );
		float Tan(float x);
		float ArcTan( float x );
		float Cot(float x);
		float InvSqrt(float number);

		template <typename T>
		T Min(T& lhs, T& rhs);
		template <typename T>
		T Max(T& lhs, T& rhs);

		template <typename T> 
		T Dot(Vec3<T> const & lhs, Vec3<T> const & rhs);

		template <typename T> 
		Vec3<T> Cross(Vec3<T> const & lhs, Vec3<T> const & rhs);

		template <typename T> 
		T Normalize(T const & rhs);

		template <typename T> 
		Vec3<T> Transform(const Vec3<T> & lhs, const Matrix<T> & rhs);
		template <typename T>
		Vec3<T> TransformNormal(const Vec3<T> & lhs, const Matrix<T> & rhs);

		template <typename T>
		Vec4<T> Transform(const Vec4<T> & lhs, const Matrix<T> & rhs);


		template <typename T>
		Matrix<T> LookAtLH(const Vec3<T> & eye, const Vec3<T> & at, const Vec3<T> & up);

		template <typename T>
		Matrix<T> PerspectiveFovLH(const T fovy, const T aspect, const T zn,const T zf);

		template <typename T>
		void Identity(Matrix<T> & lhs);

		template <typename T>
		Matrix<T>  Inverse(Matrix<T> & lhs);

		template <typename T>
		Matrix<T>  Transpose(Matrix<T> & lhs);

		template <typename T>
		Matrix<T>  InverTranspose(Matrix<T> & lhs);

		template <typename T>
		void XRotation(Matrix<T> & lhs, float theta);

		template <typename T>
		void YRotation(Matrix<T> & lhs, float theta);

		template <typename T>
		void ZRotation(Matrix<T> & lhs, float theta);

		template <typename T>
		void RotationAxis(Matrix<T> & lhs, Vec3<T> & axis, float theta);

		template <typename T>
		void Translate(Matrix<T> & lhs, float x, float y, float z);

		template <typename T>
		void Scale(Matrix<T> & lhs, float scale);

		template <typename T>
		Vec3<T> Max(Vec3<T> & lhs, Vec3<T> & rhs);

		template <typename T>
		Vec3<T> Min(Vec3<T> & lhs, Vec3<T> & rhs);

		bool IntersectRayAABB(Ray* ray, AABBox* aabb);
		bool IntersectRayTriangle(Ray* ray, float3 a, float3 b, float3 c, float & t);
	}

}

#include "Math.hpp"
#endif