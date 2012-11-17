
#include <cmath>
#include <cstdio>
#include <cstring>
#include <limits>
#include <iostream>

#include "glsl++-swizzle.h"

namespace glsl {

	// TODO: Make this prettier with C++11
	template <typename T, typename U>
	struct bigger_type
	{	
	};

	template <>
	struct bigger_type<float, float>
	{
		typedef float type;
	};

	template <>
	struct bigger_type<float, double>
	{
		typedef double type;
	};

	template <>
	struct bigger_type<float, int>
	{
		typedef double type;
	};

	template <>
	struct bigger_type<float, unsigned>
	{
		typedef double type;
	};

	template <>
	struct bigger_type<double, double>
	{
		typedef double type;
	};

	template <>
	struct bigger_type<double, float>
	{
		typedef double type;
	};

	template <>
	struct bigger_type<double, int>
	{
		typedef double type;
	};

	template <>
	struct bigger_type<double, unsigned>
	{
		typedef double type;
	};

	template <>
	struct bigger_type<int, double>
	{
		typedef double type;
	};

	template <>
	struct bigger_type<int, float>
	{
		typedef double type;
	};

	template <>
	struct bigger_type<int, int>
	{
		typedef int type;
	};

	template <>
	struct bigger_type<int, unsigned>
	{
		typedef double type;
	};

	template <>
	struct bigger_type<unsigned, double>
	{
		typedef double type;
	};

	template <>
	struct bigger_type<unsigned, float>
	{
		typedef double type;
	};

	template <>
	struct bigger_type<unsigned, int>
	{
		typedef double type;
	};

	template <>
	struct bigger_type<unsigned, unsigned>
	{
		typedef unsigned type;
	};

	template <bool b>
	struct tAssert
	{
	};

	template <>
	struct tAssert<true>
	{
		void operator () () {};
	};

	template <typename T, typename U>
	using bt = typename bigger_type<T, U>::type;

	template <typename T, unsigned n>
	class vec;

	template <typename T, unsigned n>
	class rvec
	{
		T &c, &d, &e, &f;

	public:
		typedef T ElemType;
		template <class U, unsigned m>
		friend std::ostream& operator<<(std::ostream& os, rvec<U, m> v);

		T &x, &y, &z, &w;
		T &r, &g, &b, &a;
		T &s, &t, &p, &q;

		rvec(T &c) : c(c), d(c), e(c), f(c), x(c), y(d), z(e), w(f), r(x), g(y), b(z), a(w), s(x), t(y), p(z), q(w) {}
		rvec(T &c, T &d) : c(c), d(d), e(d), f(d), x(c), y(d), z(e), w(f), r(x), g(y), b(z), a(w), s(x), t(y), p(z), q(w) {}
		rvec(T &c, T &d, T &e) : c(c), d(d), e(e), f(e), x(c), y(d), z(e), w(f), r(x), g(y), b(z), a(w), s(x), t(y), p(z), q(w) {}
		rvec(T &c, T &d, T &e, T &f) : c(c), d(d), e(e), f(f), x(c), y(d), z(e), w(f), r(x), g(y), b(z), a(w), s(x), t(y), p(z), q(w) {}

		T& operator [] (const int index) {
			switch (index) {
				case 0:
					return c;
					break;
				case 1:
					return d;
					break;
				case 2:
					return e;
					break;
				default:
					return f;
			}
		}
		
		const T& operator [] (const int index) const {
			switch (index) {
				case 0:
					return c;
					break;
				case 1:
					return d;
					break;
				case 2:
					return e;
					break;
				default:
					return f;
			}
		}

		// TODO: Possible to merge these three operator ='s?
		rvec<T, n>& operator = (const rvec<T, n>& a) {
			for (unsigned i = 0; i < n; ++i) {
				(*this)[i] = a[i];
			}
			return *this;
		}

		template <typename U>
		rvec<T, n>& operator = (const rvec<U, n>& a) {
			for (unsigned i = 0; i < n; ++i) {
				(*this)[i] = a[i];
			}
			return *this;
		}

		template <typename U>
		rvec<T, n>& operator = (const vec<U, n>& a) {
			for (unsigned i = 0; i < n; ++i) {
				(*this)[i] = a[i];
			}
			return *this;
		}

		rvec<T, n> to_rvec() {
			return *this;
		}

		rvec<T, 2> to_rvec(unsigned a, unsigned b) {
			return rvec<T, 2>((*this)[a], (*this)[b]);
		}

		rvec<T, 3> to_rvec(unsigned a, unsigned b, unsigned c) {
			return rvec<T, 3>((*this)[a], (*this)[b], (*this)[c]);
		}

		rvec<T, 4> to_rvec(unsigned a, unsigned b, unsigned c, unsigned d) {
			return rvec<T, 4>((*this)[a], (*this)[b], (*this)[c], (*this)[d]);
		}
	};

	template <typename T, unsigned n>
	class vec
	{
		T data[n];

		template <typename U, unsigned m>
		friend class vec;

	public:
		typedef T ElemType;
		template <class U, unsigned m>
		friend std::ostream& operator<<(std::ostream& os, vec<U, m> v);

		T &x, &y, &z, &w;
		T &r, &g, &b, &a;
		T &s, &t, &p, &q;

		vec() : x(data[0]), y(data[1]), z(data[2]), w(data[3]), r(x), g(y), b(z), a(w), s(x), t(y), p(z), q(w) {
			for (unsigned i = 0; i < n; ++i) {
				data[i] = 0;
			}
		}

		template <typename U>
		vec(U v) : x(data[0]), y(data[1]), z(data[2]), w(data[3]), r(x), g(y), b(z), a(w), s(x), t(y), p(z), q(w) {
			for (unsigned i = 0; i < n; ++i) {
				data[i] = v;
			}
		}

		template <typename U, unsigned m, template <typename V, unsigned o> class C>
		vec(const C<U, m>& a) : x(data[0]), y(data[1]), z(data[2]), w(data[3]), r(x), g(y), b(z), a(w), s(x), t(y), p(z), q(w) {
			tAssert<m <= n>()();
			for (unsigned i = m; i < n; ++i) {
				data[i] = 0;
			}
			for (unsigned i = 0; i < m; ++i) {
				data[i] = a[i];
			}
		}
		
		template <typename U, class... Args>
		vec(U v, Args... args) : x(data[0]), y(data[1]), z(data[2]), w(data[3]), r(x), g(y), b(z), a(w), s(x), t(y), p(z), q(w) {
			memcpy(data, vec<T, n>(args...).data, sizeof(data));
			for (unsigned i = n-1; i >= 1; --i) {
				data[i] = data[i-1];
			}
			data[0] = v;
		}

		template <typename U, unsigned m, class... Args, template <typename V, unsigned o> class C>
		vec(const C<U, m>& a, Args... args) : x(data[0]), y(data[1]), z(data[2]), w(data[3]), r(x), g(y), b(z), a(w), s(x), t(y), p(z), q(w) {
			tAssert<m <= n>()();
			memcpy(data, vec<T, n>(args...).data, sizeof(data));
			for (unsigned i = n-1; i >= m; --i) {
				data[i] = data[i-m];
			}
			for (unsigned i = 0; i < m; ++i) {
				data[i] = a[i];
			}
		}

		vec<T, n>& operator = (const vec<T, n>& a) {
			memcpy(data, a.data, sizeof(data));
			return *this;
		}

		template <typename U>
		vec<T, n>& operator = (const vec<U, n>& a) {
			for (unsigned i = 0; i < n; ++i) {
				data[i] = a.data[i];
			}
			return *this;
		}

		T& operator [] (const int index) {
			return data[index];
		}
		
		const T& operator [] (const int index) const {
			return data[index];
		}
		
		rvec<T, n> to_rvec() {
			return rvec<T, n>(data[0], data[1], data[2], data[3]);
		}

		rvec<T, 2> to_rvec(unsigned a, unsigned b) {
			return rvec<T, 2>(data[a], data[b]);
		}
		
		rvec<T, 3> to_rvec(unsigned a, unsigned b, unsigned c) {
			return rvec<T, 3>(data[a], data[b], data[c]);
		}
		
		rvec<T, 4> to_rvec(unsigned a, unsigned b, unsigned c, unsigned d) {
			return rvec<T, 4>(data[a], data[b], data[c], data[d]);
		}
	};

	// TODO: Merge this one and the next one
	template <typename T, unsigned n>
	std::ostream& operator <<(std::ostream& os, rvec<T, n> v) {
		os << "(";
		for (unsigned i = 0; i < n; ++i) {
			os << v[i];
			if (i < n-1)
				os << ", ";
		}
		os << ")";
		return os;
	}

	template <typename T, unsigned n>
	std::ostream& operator <<(std::ostream& os, vec<T, n> v) {
		os << "(";
		for (unsigned i = 0; i < n; ++i) {
			os << v[i];
			if (i < n-1)
				os << ", ";
		}
		os << ")";
		return os;
	}

	template <typename U, typename V, unsigned n, template <typename U, unsigned n> class C1, template <typename V, unsigned n> class C2>
	vec<bt<U, V>, n> zip (const C1<U, n>& a, const C2<V, n>& b, bt<U, V> (*func)(const U&, const V&)) {
		vec<bt<U, V>, n> ret;
		for (unsigned i = 0; i < n; ++i) {
			ret[i] = func(a[i], b[i]);
		}
		return ret;
	}

	template <typename T, typename U, unsigned n, template <typename T, unsigned n> class C1, template <typename U, unsigned n> class C2>
	C1<T, n>& zip (C1<T, n>& a, const C2<U, n>& b, void (*func)(T&, const U&)) {
		for (unsigned i = 0; i < n; ++i) {
			func(a[i], b[i]);
		}
		return a;
	}

	template <typename U, typename V, unsigned n, template <typename T, unsigned n> class C>
	vec<bt<U, V>, n> map (const C<U, n>& a, const V& b, bt<U, V> (*func)(const U&, const V&)) {
		vec<bt<U, V>, n> ret;
		for (unsigned i = 0; i < n; ++i) {
			ret[i] = func(a[i], b);
		}
		return ret;
	}

	template <typename U, typename V, unsigned n, template <typename V, unsigned n> class C>
	vec<bt<U, V>, n> map (const U& a, const C<V, n>& b, bt<U, V> (*func)(const U&, const V&)) {
		vec<bt<U, V>, n> ret;
		for (unsigned i = 0; i < n; ++i) {
			ret[i] = func(a, b[i]);
		}
		return ret;
	}

	template <typename T, typename U, unsigned n, template <typename T, unsigned n> class C>
	C<T, n>& map (C<T, n>& a, const U& b, void (*func)(T&, const U&)) {
		for (unsigned i = 0; i < n; ++i) {
			func(a[i], b);
		}
		return a;
	}

	/* PLUS */

	template <typename T, typename U, typename V>
	T plus(const U& a, const V& b) {
		return a+b;
	}

	template <typename U, typename V, unsigned n, template <typename U, unsigned n> class C1, template <typename V, unsigned n> class C2>
	vec<bt<U, V>, n> operator + (const C1<U, n>& a, const C2<V, n>& b) {
		return zip(a, b, plus<bt<U, V>, U, V>);
	}

	template <typename U, typename V, unsigned n, template <typename U, unsigned n> class C>
	vec<bt<U, V>, n> operator + (const C<U, n>& a, const V& b) {
		return map(a, b, plus<bt<U, V>, U, V>);
	}

	template <typename U, typename V, unsigned n, template <typename V, unsigned n> class C>
	vec<bt<U, V>, n> operator + (const U& a, const C<V, n>& b) {
		return map(a, b, plus<bt<U, V>, U, V>);
	}

	template <typename T, typename U>
	void plus_assign(T& a, const U& b) {
		a += b;
	}

	template <typename T, typename U, unsigned n, template <typename T, unsigned n> class C>
	vec<T, n>& operator += (vec<T, n>& a, const C<U, n>& b) {
		return zip(a, b, plus_assign<T, U>);
	}

	template <typename T, typename U, unsigned n, template <typename T, unsigned n> class C>
	rvec<T, n> operator += (rvec<T, n> a, const C<U, n>& b) {
		return zip(a, b, plus_assign<T, U>);
	}

	template <typename T, typename U, unsigned n>
	vec<T, n>& operator += (vec<T, n>& a, const U& b) {
		return map(a, b, plus_assign<T, U>);
	}

	template <typename T, typename U, unsigned n>
	rvec<T, n> operator += (rvec<T, n> a, const U& b) {
		return map(a, b, plus_assign<T, U>);
	}

	/* MINUS */

	template <typename T, typename U, typename V>
	T minus(const U& a, const V& b) {
		return a-b;
	}

	template <typename U, typename V, unsigned n, template <typename U, unsigned n> class C1, template <typename V, unsigned n> class C2>
	vec<bt<U, V>, n> operator - (const C1<U, n>& a, const C2<V, n>& b) {
		return zip(a, b, minus<bt<U, V>, U, V>);
	}

	template <typename U, typename V, unsigned n, template <typename U, unsigned n> class C>
	vec<bt<U, V>, n> operator - (const C<U, n>& a, const V& b) {
		return map(a, b, minus<bt<U, V>, U, V>);
	}

	template <typename U, typename V, unsigned n, template <typename V, unsigned n> class C>
	vec<bt<U, V>, n> operator - (const U& a, const C<V, n>& b) {
		return map(a, b, minus<bt<U, V>, U, V>);
	}

	template <typename T, typename U>
	void minus_assign(T& a, const U& b) {
		a -= b;
	}

	template <typename T, typename U, unsigned n, template <typename T, unsigned n> class C>
	vec<T, n>& operator -= (vec<T, n>& a, const C<U, n>& b) {
		return zip(a, b, minus_assign<T, U>);
	}

	template <typename T, typename U, unsigned n, template <typename T, unsigned n> class C>
	rvec<T, n> operator -= (rvec<T, n> a, const C<U, n>& b) {
		return zip(a, b, minus_assign<T, U>);
	}

	template <typename T, typename U, unsigned n>
	vec<T, n>& operator -= (vec<T, n>& a, const U& b) {
		return map(a, b, minus_assign<T, U>);
	}

	template <typename T, typename U, unsigned n>
	rvec<T, n> operator -= (rvec<T, n> a, const U& b) {
		return map(a, b, minus_assign<T, U>);
	}

	/* MULTIPLICATION */

	template <typename T, typename U, typename V>
	T mult(const U& a, const V& b) {
		return a*b;
	}

	template <typename U, typename V, unsigned n, template <typename U, unsigned n> class C1, template <typename V, unsigned n> class C2>
	vec<bt<U, V>, n> operator * (const C1<U, n>& a, const C2<V, n>& b) {
		return zip(a, b, mult<bt<U, V>, U, V>);
	}

	template <typename U, typename V, unsigned n, template <typename U, unsigned n> class C>
	vec<bt<U, V>, n> operator * (const C<U, n>& a, const V& b) {
		return map(a, b, mult<bt<U, V>, U, V>);
	}

	template <typename U, typename V, unsigned n, template <typename V, unsigned n> class C>
	vec<bt<U, V>, n> operator * (const U& a, const C<V, n>& b) {
		return map(a, b, mult<bt<U, V>, U, V>);
	}

	template <typename T, typename U>
	void mult_assign(T& a, const U& b) {
		a *= b;
	}

	template <typename T, typename U, unsigned n, template <typename T, unsigned n> class C>
	vec<T, n>& operator *= (vec<T, n>& a, const C<U, n>& b) {
		return zip(a, b, mult_assign<T, U>);
	}

	template <typename T, typename U, unsigned n, template <typename T, unsigned n> class C>
	rvec<T, n> operator *= (rvec<T, n> a, const C<U, n>& b) {
		return zip(a, b, mult_assign<T, U>);
	}

	template <typename T, typename U, unsigned n>
	vec<T, n>& operator *= (vec<T, n>& a, const U& b) {
		return map(a, b, mult_assign<T, U>);
	}

	template <typename T, typename U, unsigned n>
	rvec<T, n> operator *= (rvec<T, n> a, const U& b) {
		return map(a, b, mult_assign<T, U>);
	}

	/* DIVISION */

	template <typename T, typename U, typename V>
	T div(const U& a, const V& b) {
		return a/b;
	}

	template <typename U, typename V, unsigned n, template <typename U, unsigned n> class C1, template <typename V, unsigned n> class C2>
	vec<bt<U, V>, n> operator / (const C1<U, n>& a, const C2<V, n>& b) {
		return zip(a, b, div<bt<U, V>, U, V>);
	}

	template <typename U, typename V, unsigned n, template <typename U, unsigned n> class C>
	vec<bt<U, V>, n> operator / (const C<U, n>& a, const V& b) {
		return map(a, b, div<bt<U, V>, U, V>);
	}

	template <typename U, typename V, unsigned n, template <typename V, unsigned n> class C>
	vec<bt<U, V>, n> operator / (const U& a, const C<V, n>& b) {
		return map(a, b, div<bt<U, V>, U, V>);
	}

	template <typename T, typename U>
	void div_assign(T& a, const U& b) {
		a /= b;
	}

	template <typename T, typename U, unsigned n, template <typename T, unsigned n> class C>
	vec<T, n>& operator /= (vec<T, n>& a, const C<U, n>& b) {
		return zip(a, b, div_assign<T, U>);
	}

	template <typename T, typename U, unsigned n, template <typename T, unsigned n> class C>
	rvec<T, n> operator /= (rvec<T, n> a, const C<U, n>& b) {
		return zip(a, b, div_assign<T, U>);
	}

	template <typename T, typename U, unsigned n>
	vec<T, n>& operator /= (vec<T, n>& a, const U& b) {
		return map(a, b, div_assign<T, U>);
	}

	template <typename T, typename U, unsigned n>
	rvec<T, n> operator /= (rvec<T, n> a, const U& b) {
		return map(a, b, div_assign<T, U>);
	}

	/* EQUAL */

	template <typename U, typename V, unsigned n, template <typename U, unsigned n> class C1, template <typename V, unsigned n> class C2>
	bool operator == (const C1<U, n>& a, const C2<V, n>& b) {
		for (unsigned i = 0; i < n; ++i) {
			if (a[i] != b[i])
				return false;
		}
		return true;
	}

	// TODO: Other operators

	template <typename T>
	bool above (const T& a, const T& b) {
		return a>b;
	}

	template <typename T, unsigned n, template <typename T, unsigned n> class C1, template <typename T, unsigned n> class C2>
	vec<bool, n> operator > (const C1<T, n>& a, const C2<T, n>& b) {
		return zip(a, b, above<T>);
	}

	/*template <typename T, unsigned n, template <typename T, unsigned n> class C1, template <typename T, unsigned n> class C2>
	vec<T, n> operator > (C1<T, n> v1, C2<T, n> v2) {
		T ret = std::numeric_limits<T>::min();
		for (unsigned i = 0; i < n; ++i) {
			ret = max(ret, v[i]);
		}
		return ret;
	}*/

	template <typename T, unsigned n, unsigned m>
	class mat
	{
		vec<T, n> data[m];
		T tmp[n*m];

	public:
		template <typename U>
		mat(U v) {
			for (unsigned i = 0; i < n; ++i) {
				for (unsigned j = 0; j < m; ++j) {
					data[j][i] = j == i ? v : 0;
				}
			}
		}

		template <typename U>
		mat(const vec<U, n>& a) : data{a} {}
		template <typename U, typename V>
		mat(const vec<U, n>& a, const vec<V, n>& b) : data{a,b} {}
		template <typename U, typename V, typename X>
		mat(const vec<U, n>& a, const vec<V, n>& b, const vec<X, n>& c) : data{a,b,c} {}
		template <typename U, typename V, typename X, typename Y>
		mat(const vec<U, n>& a, const vec<V, n>& b, const vec<X, n>& c, const vec<Y, n>& d) : data{a,b,c,d} {}

		mat() {}

		template <class... Args>
		mat(Args... args) : tmp{args...} {
			unsigned i = 0, j = 0;
			for (unsigned k = 0; k < sizeof...(Args); ++k) {
				data[j][i] = tmp[k];
				i++;
				if (i == n) {
					j++;
					i = 0;
				}
			}
		}

		vec<T, n>& operator [] (const int index) {
			return data[index];
		}

		const vec<T, n>& operator [] (const int index) const {
			return data[index];
		}
	};

	template <typename T, unsigned n, unsigned m>
	mat<T, n, m> zip (const mat<T, n, m>& a, const mat<T, n, m>& b, T (*func)(const T&, const T&)) {
		mat<T, n, m> ret;
		for (unsigned i = 0; i < n; ++i) {
			for (unsigned j = 0; j < m; ++j) {
				ret[i][j] = func(a[i][j], b[i][j]);
			}
		}
		return ret;
	}

	template <typename T, unsigned n, unsigned m>
	mat<T, n, m> operator + (const mat<T, n, m>& a, const mat<T, n, m>& b) {
		return zip(a, b, plus<T>);
	}

	template <typename T, unsigned n, unsigned m>
	mat<T, n, m>& operator += (mat<T, n, m>& a, const mat<T, n, m>& b) {
		return a = a + b;
	}

	template <typename T, unsigned n, unsigned m>
	mat<T, n, m> operator - (const mat<T, n, m>& a, const mat<T, n, m>& b) {
		return zip(a, b, minus<T>);
	}

	template <typename T, unsigned n, unsigned m>
	mat<T, n, m>& operator -= (mat<T, n, m>& a, const mat<T, n, m>& b) {
		return a = a - b;
	}

	template <typename T, unsigned n, unsigned m, unsigned o>
	mat<T, n, o> operator * (mat<T, n, m> mt1, mat<T, m, o> mt2) {
		mat<T, n, o> ret;
		for (unsigned i = 0; i < n; ++i) {
			for (unsigned j = 0; j < o; ++j) {
				for (unsigned k = 0; k < m; ++k) {
					ret[j][i] += mt1[k][i] * mt2[j][k];
				}
			}
		}
		return ret;
	}

	template <typename T, unsigned m, unsigned o>
	vec<T, o> operator * (vec<T, m> mt1, mat<T, m, o> mt2) {
		vec<T, o> ret;
		for (unsigned j = 0; j < o; ++j) {
			for (unsigned k = 0; k < m; ++k) {
				ret[j] += mt1[k] * mt2[j][k];
			}
		}
		return ret;
	}

	template <typename T, unsigned n, unsigned m>
	vec<T, n> operator * (mat<T, n, m> mt1, vec<T, m> mt2) {
		vec<T, n> ret;
		for (unsigned i = 0; i < n; ++i) {
			for (unsigned k = 0; k < m; ++k) {
				ret[i] += mt1[k][i] * mt2[k];
			}
		}
		return ret;
	}

	template <typename T, unsigned n, unsigned m>
	mat<T, n, m> operator *= (mat<T, n, m>& mt1, const mat<T, m, m>& mt2) {
		return mt1 = mt1 * mt2;
	}

	template <typename T, unsigned m>
	vec<T, m>& operator *= (vec<T, m>& mt1, const mat<T, m, m>& mt2) {
		return mt1 = mt2 * mt2;
	}

	typedef vec<float, 2> vec2;
	typedef vec<float, 3> vec3;
	typedef vec<float, 4> vec4;
	typedef vec<bool, 2> bvec2;
	typedef vec<bool, 3> bvec3;
	typedef vec<bool, 4> bvec4;
	typedef vec<double, 2> dvec2;
	typedef vec<double, 3> dvec3;
	typedef vec<double, 4> dvec4;
	typedef mat<float, 2, 2> mat2;
	typedef mat<float, 3, 3> mat3;
	typedef mat<float, 4, 4> mat4;
	typedef mat<float, 2, 2> mat2x2;
	typedef mat<float, 3, 2> mat2x3;
	typedef mat<float, 4, 2> mat2x4;
	typedef mat<float, 2, 3> mat3x2;
	typedef mat<float, 3, 3> mat3x3;
	typedef mat<float, 4, 3> mat3x4;
	typedef mat<float, 2, 4> mat4x2;
	typedef mat<float, 3, 4> mat4x3;
	typedef mat<float, 4, 4> mat4x4;
	typedef unsigned uint;

	template <typename U, typename V, template <typename U, unsigned n> class C1, template <typename V, unsigned n> class C2>
	vec<bt<U, V>, 3> cross(C1<U, 3> v1, C2<V, 3> v2) {
		return vec<bt<U, V>, 3>(v1[1]*v2[2] - v1[2]*v2[1], v1[2]*v2[0] - v1[0]*v2[2], v1[0]*v2[1] - v1[1]*v2[0]);
	}

	template <typename U, typename V, unsigned n, template <typename U, unsigned n> class C1, template <typename V, unsigned n> class C2>
	bt<U, V> dot(C1<U, n> v1, C2<V, n> v2) {
		bt<U, V> acc = 0.0f;
		for (unsigned i = 0; i < n; ++i) {
			acc += v1[i]*v2[i];
		}
		return acc;
	}

	template <typename T, unsigned n, template <typename T, unsigned n> class C>
	T length(C<T, n> v) {
		return sqrt(dot(v,v));
	}

	template <typename U, typename V, unsigned n, template <typename U, unsigned n> class C1, template <typename V, unsigned n> class C2>
	bt<U, V> distance(C1<U, n> v1, C2<V, n> v2) {
		return length(v1-v2);
	}

	template <typename T>
	T normalize(T n) {
		return n/length(n);
	}

	template <typename U, typename V>
	bt<U, V> max(U v1, V v2) {
		return v1 > v2 ? v1 : v2;
	}

	template <typename U, typename V, unsigned n, template <typename U, unsigned n> class C1, template <typename V, unsigned n> class C2>
	vec<bt<U, V>, n> max(C1<U, n> v1, C2<V, n> v2) {
		vec<bt<U, V>, n> ret;
		for (unsigned i = 0; i < n; ++i) {
			ret[i] = max(v1[i], v2[i]);
		}
		return ret;
	}

	template <typename T, unsigned n, template <typename T, unsigned n> class C>
	T max(C<T, n> v) {
		T ret = std::numeric_limits<T>::min();
		for (unsigned i = 0; i < n; ++i) {
			ret = max(ret, v[i]);
		}
		return ret;
	}

	template <typename U, typename V>
	bt<U, V> min(U v1, V v2) {
		return v1 < v2 ? v1 : v2;
	}

	template <typename T, unsigned n, template <typename T, unsigned n> class C>
	T min(C<T, n> v) {
		T ret = std::numeric_limits<T>::max();
		for (unsigned i = 0; i < n; ++i) {
			ret = min(ret, v[i]);
		}
		return ret;
	}

	template <typename U>
	U abs(U v1) {
		return std::abs(v1);
	}

	template <typename T, unsigned n, template <typename T, unsigned n> class C>
	vec<T, n> abs(C<T, n> v) {
		vec<T, n> ret;
		for (unsigned i = 0; i < n; ++i) {
			ret[i] = abs(v[i]);
		}
		return ret;
	}

	template <typename U, typename V, unsigned n, template <typename U, unsigned n> class C1, template <typename V, unsigned n> class C2>
	vec<bt<U, V>, n> reflect(C1<U, n> I, C2<V, n> N) {
		return I - 2.0f * dot(N, I) * N;
	}

	template <typename U, typename V, unsigned n, template <typename U, unsigned n> class C1, template <typename V, unsigned n> class C2>
	vec<bool, n> greaterThan(C1<U, n> v1, C2<V, n> v2) {
		vec<bool, n> ret;
		for (unsigned i = 0; i < n; ++i) {
			ret[i] = v1[i] > v2[i];
		}
		return ret;
	}

	template <typename U, typename V, unsigned n, unsigned m, template <typename U, unsigned n> class C1, template <typename V, unsigned m> class C2>
	mat<bt<U, V>, n, m> outerProduct(C1<U, n> v1, C2<V, m> v2) {
		mat<bt<U, V>, n, m> ret;
		for (unsigned i = 0; i < n; ++i) {
			for (unsigned j = 0; j < m; ++j) {
				ret[j][i] = v1[i] * v2[j];
			}
		}
		return ret;
	}

	template <typename T, unsigned n, unsigned m>
	mat<T, m, n> transpose(mat<T, n, m> mt) {
		mat<T, m, n> ret;
		for (unsigned i = 0; i < n; ++i) {
			for (unsigned j = 0; j < m; ++j) {
				ret[i][j] = mt[j][i];
			}
		}
		return ret;
	}

	template <typename U>
	U cos(U v1) {
		return std::cos(v1);
	}

	template <typename T, unsigned n, template <typename T, unsigned n> class C>
	vec<T, n> cos(C<T, n> v) {
		vec<T, n> ret;
		for (unsigned i = 0; i < n; ++i) {
			ret[i] = cos(v[i]);
		}
		return ret;
	}

	template <typename U>
	U sin(U v1) {
		return std::sin(v1);
	}

	template <typename T, unsigned n, template <typename T, unsigned n> class C>
	vec<T, n> sin(C<T, n> v) {
		vec<T, n> ret;
		for (unsigned i = 0; i < n; ++i) {
			ret[i] = sin(v[i]);
		}
		return ret;
	}

	template <typename U, typename V>
	bt<U, V> pow(U v1, V v2) {
		return std::pow(v1, v2);
	}

	template <typename U, typename V, unsigned n, template <typename U, unsigned n> class C1, template <typename V, unsigned n> class C2>
	vec<bt<U, V>, n> pow(C1<U, n> v1, C2<V, n> v2) {
		vec<bt<U, V>, n> ret;
		for (unsigned i = 0; i < n; ++i) {
			ret[i] = pow(v1[i], v2[i]);
		}
		return ret;
	}

	template <typename U, typename V, typename X>
	bt<U, V> clamp(U v1, V v2, X v3) {
		return v1 < v2 ? v2 : v1 > v3 ? v3 : v1;
	}

	/*template <typename U, typename V, unsigned n, template <typename U, unsigned n> class C1, template <typename V, unsigned n> class C2>
	vec<bt<U, V>, n> pow(C1<U, n> v1, C2<V, n> v2) {
		vec<bt<U, V>, n> ret;
		for (unsigned i = 0; i < n; ++i) {
			ret[i] = pow(v1[i], v2[i]);
		}
		return ret;
	}*/

	#define uniform 
	#define varying 
	#define inout 

	vec4 gl_TexCoord[8];
	vec4 gl_FragCoord;
	vec4 gl_FragColor;

	// TODO:
	// Dividing by zero - what is correct behavior?

}
