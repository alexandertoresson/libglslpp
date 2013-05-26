
#include <cmath>
#include <cstdio>
#include <cstring>
#include <limits>
#include <iostream>

#include "glsl++-swizzle.h"

namespace glsl {

	template <bool b>
	struct tAssert
	{
	};

	template <>
	struct tAssert<true>
	{
		void operator () () {};
	};

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

		// TODO: Necessary?
		vec(const vec<T, n>& a) : x(data[0]), y(data[1]), z(data[2]), w(data[3]), r(x), g(y), b(z), a(w), s(x), t(y), p(z), q(w) {
			for (unsigned i = 0; i < n; ++i) {
				data[i] = a[i];
			}
		}

		template <typename U, unsigned m, template <typename V, unsigned o> class C>
		vec(const C<U, m>& a) : x(data[0]), y(data[1]), z(data[2]), w(data[3]), r(x), g(y), b(z), a(w), s(x), t(y), p(z), q(w) {
			tAssert<m <= n>()();
			unsigned i;
			for (i = 0; i < m; ++i) {
				data[i] = a[i];
			}
			for (; i < n; ++i) {
				data[i] = 0;
			}
		}

	private:
		template <typename U, unsigned m, template <typename V, unsigned o> class C, typename V>
		static void init(size_t offset, C<U, m>& a, V v) {
			a.data[offset] = v;
			for (++offset; offset < m; ++offset) {
				a.data[offset] = 0;
			}
		}

		template <typename W, unsigned p, template <typename V, unsigned o> class C1, typename U, unsigned m, template <typename V, unsigned o> class C2>
		static void init(size_t offset, C1<W, p>& a, const C2<U, m>& v) {
			for (unsigned i = 0; i < m; ++i) {
				a.data[i+offset] = v[i];
			}
			for (offset += m; offset < p; ++offset) {
				a.data[offset] = 0;
			}
		}

		template <typename U, unsigned m, template <typename V, unsigned o> class C, typename V, class... Args>
		static void init(size_t offset, C<U, m>& a, V v, Args... args) {
			a.data[offset] = v;
			init(offset+1, a, args...);
		}

		template <typename W, unsigned p, template <typename V, unsigned o> class C1, typename U, unsigned m, template <typename V, unsigned o> class C2, class... Args>
		static void init(size_t offset, C1<W, p>& a, const C2<U, m>& v, Args... args) {
			for (unsigned i = 0; i < m; ++i) {
				a.data[i+offset] = v[i];
			}
			init(offset+m, a, args...);
		}
	public:
		template <class... Args>
		vec(Args... args) : x(data[0]), y(data[1]), z(data[2]), w(data[3]), r(x), g(y), b(z), a(w), s(x), t(y), p(z), q(w) {
			init(0, *this, args...);
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

	template <typename U, typename V, unsigned n, template <typename U, unsigned n> class C1, template <typename V, unsigned n> class C2, typename F>
	auto zip (const C1<U, n>& a, const C2<V, n>& b, F func) -> vec<decltype(func(U(), V())), n> {
		vec<decltype(func(U(), V())), n> ret;
		for (unsigned i = 0; i < n; ++i) {
			ret[i] = func(a[i], b[i]);
		}
		return ret;
	}

	template <typename T, typename U, unsigned n, template <typename T, unsigned n> class C1, template <typename U, unsigned n> class C2, typename F>
	C1<T, n>& zip (C1<T, n>& a, const C2<U, n>& b, F func) {
		for (unsigned i = 0; i < n; ++i) {
			func(a[i], b[i]);
		}
		return a;
	}

	template <typename U, unsigned n, template <typename T, unsigned n> class C, typename F>
	auto map (const C<U, n>& a, F func) -> vec<decltype(func(U())), n> {
		vec<decltype(func(U())), n> ret;
		for (unsigned i = 0; i < n; ++i) {
			ret[i] = func(a[i]);
		}
		return ret;
	}

	template <typename T, unsigned n, template <typename T, unsigned n> class C, typename F>
	C<T, n>& map (C<T, n>& a, F func) {
		for (unsigned i = 0; i < n; ++i) {
			func(a[i]);
		}
		return a;
	}

	template <typename T, unsigned n, template <typename T, unsigned n> class C, typename F>
	T foldl (const C<T, n>& a, T def, F func) {
		for (unsigned i = 0; i < n; ++i) {
			def = func(def, a[i]);
		}
		return def;
	}

	/* PLUS */

	template <typename U, typename V, unsigned n, template <typename U, unsigned n> class C1, template <typename V, unsigned n> class C2>
	vec<decltype(U()+V()), n>operator + (const C1<U, n>& a, const C2<V, n>& b) {
		return zip(a, b, [](U a, V b){ return a+b; });
	}

	template <typename U, typename V, unsigned n, template <typename U, unsigned n> class C>
	vec<decltype(U()+V()), n> operator + (const C<U, n>& a, const V& b) {
		return map(a, [=](U a){ return a+b; });
	}

	template <typename U, typename V, unsigned n, template <typename V, unsigned n> class C>
	vec<decltype(U()+V()), n> operator + (const U& a, const C<V, n>& b) {
		return map(b, [=](V b){ return a+b; });
	}

	// NOTE: vec is passed by ref, rvec is passed by value
	template <typename T, typename U, unsigned n, template <typename T, unsigned n> class C>
	vec<T, n>& operator += (vec<T, n>& a, const C<U, n>& b) {
		return zip(a, b, [](T& a, U b){ return a+= b; });
	}

	template <typename T, typename U, unsigned n, template <typename T, unsigned n> class C>
	rvec<T, n> operator += (rvec<T, n> a, const C<U, n>& b) {
		return zip(a, b, [](T& a, U b){ return a+= b; });
	}

	template <typename T, typename U, unsigned n>
	vec<T, n>& operator += (vec<T, n>& a, const U& b) {
		return map(a, [=](T& a){ return a+= b; });
	}

	template <typename T, typename U, unsigned n>
	rvec<T, n> operator += (rvec<T, n> a, const U& b) {
		return map(a, [=](T& a){ return a+= b; });
	}

	/* MINUS */

	template <typename U, typename V, unsigned n, template <typename U, unsigned n> class C1, template <typename V, unsigned n> class C2>
	vec<decltype(U()-V()), n>operator - (const C1<U, n>& a, const C2<V, n>& b) {
		return zip(a, b, [](U a, V b){ return a-b; });
	}

	template <typename U, typename V, unsigned n, template <typename U, unsigned n> class C>
	vec<decltype(U()-V()), n> operator - (const C<U, n>& a, const V& b) {
		return map(a, [=](U a){ return a-b; });
	}

	template <typename U, typename V, unsigned n, template <typename V, unsigned n> class C>
	vec<decltype(U()-V()), n> operator - (const U& a, const C<V, n>& b) {
		return map(b, [=](V b){ return a-b; });
	}

	// NOTE: vec is passed by ref, rvec is passed by value
	template <typename T, typename U, unsigned n, template <typename T, unsigned n> class C>
	vec<T, n>& operator -= (vec<T, n>& a, const C<U, n>& b) {
		return zip(a, b, [](T& a, U b){ return a-= b; });
	}

	template <typename T, typename U, unsigned n, template <typename T, unsigned n> class C>
	rvec<T, n> operator -= (rvec<T, n> a, const C<U, n>& b) {
		return zip(a, b, [](T& a, U b){ return a-= b; });
	}

	template <typename T, typename U, unsigned n>
	vec<T, n>& operator -= (vec<T, n>& a, const U& b) {
		return map(a, [=](T& a){ return a-= b; });
	}

	template <typename T, typename U, unsigned n>
	rvec<T, n> operator -= (rvec<T, n> a, const U& b) {
		return map(a, [=](T& a){ return a-= b; });
	}

	/* MULTIPLICATION */

	template <typename U, typename V, unsigned n, template <typename U, unsigned n> class C1, template <typename V, unsigned n> class C2>
	vec<decltype(U()*V()), n>operator * (const C1<U, n>& a, const C2<V, n>& b) {
		return zip(a, b, [](U a, V b){ return a*b; });
	}

	template <typename U, typename V, unsigned n, template <typename U, unsigned n> class C>
	vec<decltype(U()*V()), n> operator * (const C<U, n>& a, const V& b) {
		return map(a, [=](U a){ return a*b; });
	}

	template <typename U, typename V, unsigned n, template <typename V, unsigned n> class C>
	vec<decltype(U()*V()), n> operator * (const U& a, const C<V, n>& b) {
		return map(b, [=](V b){ return a*b; });
	}

	// NOTE: vec is passed by ref, rvec is passed by value
	template <typename T, typename U, unsigned n, template <typename T, unsigned n> class C>
	vec<T, n>& operator *= (vec<T, n>& a, const C<U, n>& b) {
		return zip(a, b, [](T& a, U b){ return a*= b; });
	}

	template <typename T, typename U, unsigned n, template <typename T, unsigned n> class C>
	rvec<T, n> operator *= (rvec<T, n> a, const C<U, n>& b) {
		return zip(a, b, [](T& a, U b){ return a*= b; });
	}

	template <typename T, typename U, unsigned n>
	vec<T, n>& operator *= (vec<T, n>& a, const U& b) {
		return map(a, [=](T& a){ return a*= b; });
	}

	template <typename T, typename U, unsigned n>
	rvec<T, n> operator *= (rvec<T, n> a, const U& b) {
		return map(a, [=](T& a){ return a*= b; });
	}

	/* DIVISION */

	template <typename U, typename V, unsigned n, template <typename U, unsigned n> class C1, template <typename V, unsigned n> class C2>
	vec<decltype(U()/V()), n>operator / (const C1<U, n>& a, const C2<V, n>& b) {
		return zip(a, b, [](U a, V b){ return a/b; });
	}

	template <typename U, typename V, unsigned n, template <typename U, unsigned n> class C>
	vec<decltype(U()/V()), n> operator / (const C<U, n>& a, const V& b) {
		return map(a, [=](U a){ return a/b; });
	}

	template <typename U, typename V, unsigned n, template <typename V, unsigned n> class C>
	vec<decltype(U()/V()), n> operator / (const U& a, const C<V, n>& b) {
		return map(b, [=](V b){ return a/b; });
	}

	// NOTE: vec is passed by ref, rvec is passed by value
	template <typename T, typename U, unsigned n, template <typename T, unsigned n> class C>
	vec<T, n>& operator /= (vec<T, n>& a, const C<U, n>& b) {
		return zip(a, b, [](T& a, U b){ return a/= b; });
	}

	template <typename T, typename U, unsigned n, template <typename T, unsigned n> class C>
	rvec<T, n> operator /= (rvec<T, n> a, const C<U, n>& b) {
		return zip(a, b, [](T& a, U b){ return a/= b; });
	}

	template <typename T, typename U, unsigned n>
	vec<T, n>& operator /= (vec<T, n>& a, const U& b) {
		return map(a, [=](T& a){ return a/= b; });
	}

	template <typename T, typename U, unsigned n>
	rvec<T, n> operator /= (rvec<T, n> a, const U& b) {
		return map(a, [=](T& a){ return a/= b; });
	}

	/* EQUAL */

	template <typename U, typename V, unsigned n, template <typename U, unsigned n> class C1, template <typename V, unsigned n> class C2>
	bool operator == (const C1<U, n>& a, const C2<V, n>& b) {
		return foldl(zip(a, b, [](U a, V b){ return a==b; }), true, [](bool a, bool b){ return a&&b; });
	}

	/* NOT EQUAL */

	template <typename U, typename V, unsigned n, template <typename U, unsigned n> class C1, template <typename V, unsigned n> class C2>
	bool operator != (const C1<U, n>& a, const C2<V, n>& b) {
		return !(a == b);
	}

	// TODO: Other operators

	template <typename T, typename U, unsigned n, template <typename T, unsigned n> class C1, template <typename U, unsigned n> class C2>
	vec<bool, n> operator > (const C1<T, n>& a, const C2<U, n>& b) {
		return zip(a, b, [](T a, U b){ return a>b; });
	}

	/*template <typename T, unsigned n, template <typename T, unsigned n> class C1, template <typename T, unsigned n> class C2>
	vec<T, n> operator > (C1<T, n> v1, C2<T, n> v2) {
		T ret = std::numeric_limits<T>::min();
		for (unsigned i = 0; i < n; ++i) {
			ret = max(ret, v[i]);
		}
		return ret;
	}*/

	/* NEGATION */
	
	template <typename T, unsigned n, template <typename U, unsigned n> class C >
	vec<T, n> operator - (const C<T, n>& a) {
		return map(a, [](T a){ return -a; });
	}

	template <typename T, unsigned n, unsigned m>
	class mat
	{
		vec<T, n> data[m];
		T tmp[n*m];

	public:
		template <class U, unsigned o, unsigned p>
		friend std::ostream& operator<<(std::ostream& os, mat<U, o, p> v);

		template <typename U>
		mat(mat<U, n, m> v) {
			for (unsigned i = 0; i < n; ++i) {
				for (unsigned j = 0; j < m; ++j) {
					data[j][i] = v[j][i];
				}
			}
		}

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
	std::ostream& operator <<(std::ostream& os, mat<T, n, m> v) {
		os << "(";
		for (unsigned i = 0; i < n; ++i) {
			for (unsigned j = 0; j < m; ++j) {
				os << v[j][i];
				if (j < m-1)
					os << ", ";
				else if (i < n-1)
					os << "; ";
			}
		}
		os << ")";
		return os;
	}

	template <typename U, typename V, unsigned n, unsigned m, typename F>
	auto zip (const mat<U, n, m>& a, const mat<V, n, m>& b, F func) -> mat<decltype(func(U(), V())), n, m> {
		mat<decltype(func(U(), V())), n, m> ret;
		for (unsigned i = 0; i < n; ++i) {
			for (unsigned j = 0; j < m; ++j) {
				ret[i][j] = func(a[i][j], b[i][j]);
			}
		}
		return ret;
	}

	template <typename U, typename V, unsigned n, unsigned m, typename F>
	mat<U, n, m>& zip (mat<U, n, m>& a, const mat<V, n, m>& b, F func) {
		for (unsigned i = 0; i < n; ++i) {
			for (unsigned j = 0; j < m; ++j) {
				func(a[i][j], b[i][j]);
			}
		}
		return a;
	}

	template <typename U, unsigned n, unsigned m, typename F>
	auto map (const mat<U, n, m>& a, F func) -> mat<decltype(func(U())), n, m> {
		mat<decltype(func(U())), n, m> ret;
		for (unsigned i = 0; i < n; ++i) {
			for (unsigned j = 0; j < m; ++j) {
				ret[i][j] = func(a[i][j]);
			}
		}
		return ret;
	}

	template <typename U, unsigned n, unsigned m, typename F>
	mat<U, n, m>& map (mat<U, n, m>& a, F func) {
		for (unsigned i = 0; i < n; ++i) {
			for (unsigned j = 0; j < m; ++j) {
				func(a[i][j]);
			}
		}
		return a;
	}

	template <typename U, typename V, unsigned n, unsigned m>
	mat<decltype(U()+V()), n, m> operator + (const mat<U, n, m>& a, const mat<V, n, m>& b) {
		return zip(a, b, [](U a, V b){ return a+b; });
	}

	template <typename U, typename V, unsigned n, unsigned m>
	mat<decltype(U()+V()), n, m> operator + (const mat<U, n, m>& a, const V& b) {
		return map(a, [=](U a){ return a+b; });
	}

	template <typename U, typename V, unsigned n, unsigned m>
	mat<decltype(U()+V()), n, m> operator + (const U& a, const mat<V, n, m>& b) {
		return map(b, [=](V b){ return a+b; });
	}

	template <typename U, typename V, unsigned n, unsigned m>
	mat<U, n, m>& operator += (mat<U, n, m>& a, const mat<V, n, m>& b) {
		return zip(a, b, [](U& a, V b){ a+=b; });
	}

	template <typename U, typename V, unsigned n, unsigned m>
	mat<U, n, m>& operator += (mat<U, n, m>& a, const V& b) {
		return map(a, [=](U& a){ a+=b; });
	}

	template <typename U, typename V, unsigned n, unsigned m>
	mat<decltype(U()-V()), n, m> operator - (const mat<U, n, m>& a, const mat<V, n, m>& b) {
		return zip(a, b, [](U a, V b){ return a-b; });
	}

	template <typename U, typename V, unsigned n, unsigned m>
	mat<decltype(U()+V()), n, m> operator - (const mat<U, n, m>& a, const V& b) {
		return map(a, [=](U a){ return a-b; });
	}

	template <typename U, typename V, unsigned n, unsigned m>
	mat<decltype(U()+V()), n, m> operator - (const U& a, const mat<V, n, m>& b) {
		return map(b, [=](V b){ return a-b; });
	}

	template <typename U, typename V, unsigned n, unsigned m>
	mat<U, n, m>& operator -= (mat<U, n, m>& a, const mat<V, n, m>& b) {
		return zip(a, b, [](U& a, V b){ a-=b; });
	}

	template <typename U, typename V, unsigned n, unsigned m>
	mat<U, n, m>& operator -= (mat<U, n, m>& a, const V& b) {
		return map(a, [=](U& a){ a-=b; });
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
	vec<T, m> operator *= (vec<T, m>& mt1, const mat<T, m, m>& mt2) {
		return mt1 = mt1 * mt2;
	}

	template <typename U, typename V, unsigned n, unsigned m>
	bool operator == (const mat<U, n, m>& a, const mat<V, n, m>& b) {
		for (unsigned i = 0; i < m; ++i) {
			if (a[i] != b[i])
				return false;
		}
		return true;
	}

	template <typename U, typename V, unsigned n, unsigned m>
	bool operator != (const mat<U, n, m>& a, const mat<V, n, m>& b) {
		return !(a == b);
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
	vec<decltype(U()*V()), 3> cross(C1<U, 3> v1, C2<V, 3> v2) {
		return vec<decltype(U()*V()), 3>(v1[1]*v2[2] - v1[2]*v2[1], v1[2]*v2[0] - v1[0]*v2[2], v1[0]*v2[1] - v1[1]*v2[0]);
	}

	template <typename U, typename V, unsigned n, template <typename U, unsigned n> class C1, template <typename V, unsigned n> class C2>
	decltype(U()*V()) dot(C1<U, n> v1, C2<V, n> v2) {
		return foldl(v1*v2, decltype(U()*V())(), [](decltype(U()*V()) a, decltype(U()*V()) b){ return a+b;});
	}

	template <typename T, unsigned n, template <typename T, unsigned n> class C>
	T length(C<T, n> v) {
		return sqrt(dot(v,v));
	}

	template <typename U, typename V, unsigned n, template <typename U, unsigned n> class C1, template <typename V, unsigned n> class C2>
	decltype(U()-V()) distance(C1<U, n> v1, C2<V, n> v2) {
		return length(v1-v2);
	}

	template <typename T>
	T normalize(T n) {
		return n/length(n);
	}

	template <typename U, typename V>
	decltype(false ? U() : V()) max(U v1, V v2) {
		return v1 > v2 ? v1 : v2;
	}

	template <typename U, typename V, unsigned n, template <typename U, unsigned n> class C1, template <typename V, unsigned n> class C2>
	vec<decltype(max(U(), V())), n> max(const C1<U, n>& v1, const C2<V, n>& v2) {
		return zip(v1, v2, max<U, V>);
	}

	template <typename U, typename V>
	decltype(false ? U() : V()) min(U v1, V v2) {
		return v1 < v2 ? v1 : v2;
	}

	template <typename U, typename V, unsigned n, template <typename U, unsigned n> class C1, template <typename V, unsigned n> class C2>
	vec<decltype(min(U(), V())), n> min(const C1<U, n>& v1, const C2<V, n>& v2) {
		return zip(v1, v2, min<U, V>);
	}

	template <typename U>
	U abs(U v1) {
		return std::abs(v1);
	}

	template <typename T, unsigned n, template <typename T, unsigned n> class C>
	vec<T, n> abs(const C<T, n>& v) {
		return map(v, abs<T>);
	}

	template <typename U>
	U floor(U v1) {
		return std::floor(v1);
	}

	template <typename T, unsigned n, template <typename T, unsigned n> class C>
	vec<T, n> floor(const C<T, n>& v) {
		return map(v, floor<T>);
	}

	template <typename U>
	U fract(U v1) {
		return v1 - std::floor(v1);
	}

	template <typename T, unsigned n, template <typename T, unsigned n> class C>
	vec<T, n> fract(const C<T, n>& v) {
		return map(v, fract<T>);
	}

	template <typename U>
	U sign(U v1) {
		return (U(0) < v1) - (v1 < U(0));;
	}

	template <typename T, unsigned n, template <typename T, unsigned n> class C>
	vec<T, n> sign(const C<T, n>& v) {
		return map(v, sign<T>);
	}

	template <typename U, typename V, unsigned n, template <typename U, unsigned n> class C1, template <typename V, unsigned n> class C2>
	vec<decltype(U()*V()), n> reflect(C1<U, n> I, C2<V, n> N) {
		return I - 2.0f * dot(N, I) * N;
	}

	template <typename U, typename V, unsigned n, template <typename U, unsigned n> class C1, template <typename V, unsigned n> class C2>
	vec<bool, n> greaterThan(C1<U, n> v1, C2<V, n> v2) {
		return zip(v1, v2, [](U a, V b){ return a>b; });
	}

	template <typename U, typename V, unsigned n, unsigned m, template <typename U, unsigned n> class C1, template <typename V, unsigned m> class C2>
	mat<decltype(U()*V()), n, m> outerProduct(C1<U, n> v1, C2<V, m> v2) {
		mat<decltype(U()*V()), n, m> ret;
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
	vec<T, n> cos(const C<T, n>& v) {
		return map(v, cos<T>);
	}

	template <typename U>
	U sin(U v1) {
		return std::sin(v1);
	}

	template <typename T, unsigned n, template <typename T, unsigned n> class C>
	vec<T, n> sin(const C<T, n>& v) {
		return map(v, sin<T>);
	}

	template <typename T, typename U>
	T atan(T v1, U v2) {
		return std::atan2(v1, v2);
	}

	template <typename T, typename U, unsigned n, template <typename T, unsigned n> class C1, template <typename U, unsigned n> class C2 >
	vec<T, n> atan(const C1<T, n>& v1, const C2<U, n>& v2) {
		return zip(v1, v2, atan<T, U>);
	}

	template <typename U, typename V>
	decltype(std::pow(U(), V())) pow(U v1, V v2) {
		return std::pow(v1, v2);
	}

	template <typename U, typename V, unsigned n, template <typename U, unsigned n> class C1, template <typename V, unsigned n> class C2>
	vec<decltype(std::pow(U(), V())), n> pow(const C1<U, n>& v1, const C2<V, n>& v2) {
		return zip(v1, v2, pow<U, V>);
	}

	template <typename U, typename V, typename X>
	decltype(min(max(U(), V()), X())) clamp(U v1, V v2, X v3) {
		return min(max(v1, v2), v3);
	}

	template <typename U, typename V, typename X, unsigned n, template <typename U, unsigned n> class C>
	vec<decltype(clamp(U(), V(), X())), n> clamp(const C<U, n>& v1, V v2, X v3) {
		// TODO: Can this be achieved with std::bind2nd?
		return map(v1, [=](U v1){ return clamp(v1, v2, v3); });
	}

	template <typename U, typename V, typename X, unsigned n, template <typename U, unsigned n> class C1, template <typename V, unsigned n> class C2, template <typename X, unsigned n> class C3>
	vec<decltype(clamp(U(), V(), X())), n> clamp(const C1<U, n>& v1, const C2<V, n>& v2, const C3<X, n>& v3) {
		// TODO: zip3?
		return zip(zip(v1, v2, max<U, V>), v3, min<decltype(max(U(), V())), X>);
	}

	template <typename U, typename V, typename X>
	decltype(U() + V() + X()) mix(U v1, V v2, X v3) {
		return v1 * (1 - v3) + v2 * v3;
	}

	template <typename U, typename V, typename X, unsigned n, template <typename U, unsigned n> class C1, template <typename V, unsigned n> class C2>
	vec<decltype(mix(U(), V(), X())), n> mix(const C1<U, n>& v1, const C2<V, n>& v2, X v3) {
		return zip(v1, v2, [=](U v1, V v2){ return mix(v1, v2, v3); });
	}

	// TODO: mix(vec, vec, vec)

	template <typename U, typename V, typename X>
	decltype(U() + V() + X()) smoothstep(U edge0, V edge1, X x) {
		X t = clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);
		return t * t * (3.0 - 2.0 * t);
	}

	template <typename U, typename V, typename X, unsigned n, template <typename U, unsigned n> class C1, template <typename V, unsigned n> class C2>
	vec<decltype(smoothstep(U(), V(), X())), n> smoothstep(const C1<U, n>& v1, const C2<V, n>& v2, X v3) {
		return zip(v1, v2, [=](U v1, V v2){ return smoothstep(v1, v2, v3); });
	}

	// TODO: smoothstep(vec, vec, vec)

	#define uniform 
	#define varying 
	#define inout 
	#define out 

	// TODO:
	// Dividing by zero - what is correct behavior?

}
