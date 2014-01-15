#ifndef __8a21da4a_542d_457f_8e90_583653fb71cb
#define __8a21da4a_542d_457f_8e90_583653fb71cb

#include <ostream>

template<class T = std::ostream>
class sticky_ostream
{
public:
	typedef T::char_type char_type;
	typedef T::traits_type traits_type;
	typedef std::basic_ios<char_type, traits_type> basic_ios;

	sticky_ostream(T& out) : out_(out) {}

	char_type fill(char_type fillch)
	{
		return out_.fill(fillch);
	}

	std::streamsize width(std::streamsize w)
	{
		return width_ = w;
	}

	sticky_ostream& operator<<(T& (*manip)(T&))
	{	// call basic_ostream manipulator
		return (*manip)(out_);
	}

	sticky_ostream& operator<<(basic_ios& (*manip)(basic_ios&))
	{	// call basic_ios manipulator
		(*manip)(out_);
		return *this;
	}

	sticky_ostream& operator<<(std::ios_base& (*manip)(std::ios_base&))
	{	// call ios_base manipulator
		(*manip)(out_);
		return (*this);
	}

	sticky_ostream& operator<<(char_type value)
	{
		out_ << value;
		return *this;
	}

	sticky_ostream& operator<<(const char_type* value)
	{
		out_ << value;
		return *this;
	}

	template<typename T>
	sticky_ostream& operator<<(const T& value)
	{
		out_.width(width_);
		out_ << value;
		return *this;
	}

private:
	T& out_;
	std::streamsize width_;
};

template<class T>
struct CSetFill
{
	explicit CSetFill(T ch) : fillch(ch) {}
	T fillch;
};

template<class T>
inline CSetFill<T> setfill(T fillch)
{
	return CSetFill<T>(fillch);
}

template<class T>
inline sticky_ostream<T>& operator<<(sticky_ostream<T>& out,
	const CSetFill<T::char_type>& manip)
{
	out.fill(manip.fillch);
	return out;
}

struct CSetW
{
	explicit CSetW(std::streamsize w) : width(w) {}
	std::streamsize width;
};

inline CSetW setw(std::streamsize width)
{
	return CSetW(width);
}

template<class T>
inline sticky_ostream<T>& operator<<(sticky_ostream<T>& out,
	const CSetW& manip)
{
	out.width(manip.width);
	return out;
}

template< class Elem, class Ostr = sticky_ostream<std::ostream> >
class ostream_iterator
{
public:
	typedef Ostr::char_type char_type;
	typedef Ostr::traits_type traits_type;
	typedef Ostr ostream_type;

	ostream_iterator(ostream_type& ostr, const char_type* delim = 0)
		: ostr_(&ostr), delim_(delim) {}

	ostream_iterator& operator=(const Elem& value)
	{
		*ostr_ << value;
		if( delim_ )
			*ostr_ << delim_;
		return *this;
	}

	ostream_iterator& operator*()
	{
		return *this;
	}

	ostream_iterator& operator++()
	{
		return *this;
	}

	ostream_iterator& operator++(int)
	{
		return (*this);
	}

protected:
	const char_type* delim_;
	ostream_type* ostr_;
};

#endif //__8a21da4a_542d_457f_8e90_583653fb71cb
