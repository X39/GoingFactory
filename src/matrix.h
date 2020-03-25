#pragma once

#include <cmath>
#include <utility>

namespace x39::goingfactory::math
{
	template<size_t X, size_t Y = X, typename T = float>
	class matrix
	{
		static_assert(X != 0);
		static_assert(Y != 0);
	private:
		T m_values[X * Y];
	public:
		const size_t values_max = X * Y;
		using matrix_self = matrix<X, Y, T>;
		matrix() : m_values({}) {}
		matrix(std::initializer_list<std::initializer_list<T>> init) : m_values({})
		{
			for (size_t x = 0; x < X; x++)
			{
				for (size_t y = 0; y < Y; y++)
				{
					m_values[x + y * Y] = init[y][x];
				}
			}
		}

		T& at(size_t x, size_t y)
		{
			static_assert(x < X);
			static_assert(y < Y);
			return m_values + (x + y * Y);
		}
		template<size_t OtherX, size_t OtherY>
		matrix<X, OtherY, T> operator * (const matrix<OtherX, OtherY, T>& rvalue)
		{
			static_assert(Y == OtherX);
			const size_t A = X;
			const size_t BC = Y;
			const size_t D = OtherY;
			matrix<X, OtherY, T> resultMatrix;
			for (size_t a = 0; a < A; a++)
			{
				for (size_t d = 0; d < D; d++)
				{
					for (size_t bc = 0; bc < BC; bc++)
					{
						resultMatrix.at(a, d) += at(a, bc) * rvalue.at(bc, d);
					}
				}
			}
			return resultMatrix;
		}
		matrix_self& operator *= (const T& value)
		{
			for (size_t i = 0; i < values_max; i++)
			{
				m_values[i] *= value;
			}
			return *this;
		}
	};
	static inline matrix<2, 2, float> rotation2d(float angle_radians)
	{
		return {
			{ std::cosf(angle_radians), std::sinf(angle_radians) },
			{ -std::sinf(angle_radians), std::cosf(angle_radians) }
		};
	}
}