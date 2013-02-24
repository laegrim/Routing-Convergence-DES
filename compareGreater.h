#ifndef COMPAREGREATER_H
#define COMPAREGREATER_H

template <typename T>
class compareGreater
{
private: 
	bool order;
public:
	compareGreater();
	bool operator()(const T rhs, const T lhs) const;
};

template<typename T>
compareGreater<T>::compareGreater()
{
	order = true;
}

template<typename T>
bool compareGreater<T>::operator()(const T lhs, const T rhs) const
{
	if (order == true)
		return (*lhs > *rhs);
	else
		return (*rhs < *lhs);
}

#endif