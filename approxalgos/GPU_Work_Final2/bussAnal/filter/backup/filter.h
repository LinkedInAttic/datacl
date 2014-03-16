/*
//Functor
class opFunctor{
	private:
		string op;
	public:

		switch(op)
		{
			case "lt":
				template <class T>
				bool operator() (const T a, const T b)
				{
					return less<T> ()(a, b);
				};
				break;
			case "le":
				template <class T>
				bool operator() (const T a, const T b)
				{
					return less_equal<T> ()(a, b);
				};
				break;
			case "gt":
				template <class T>
				bool operator() (const T a, const T b)
				{
					return greater<T> ()(a, b);
				};
				break;
			case "ge":
				template <class T>
				bool operator() (const T a, const T b)
				{
					return greater_equal<T> ()(a, b);
				};
				break;
			case "ne":
				template <class T>
				bool operator() (const T a, const T b)
				{
					return not_equal<T> ()(a, b);
				};
				break;
			default:
			template <class T>
				bool operator() (const T a, const T b)
				{
					return equal<T>()(a, b);
				};
				break;
		}
}
*/

template <class T, class Compare>
void filter(T data[], size_t dataSize, const T val, char bitvec[], Compare comp)
{
	for(unsigned long long i=0; i<dataSize; i++)
	{
		if( comp(data[i], val) )
			bitvec[i] = '1';
		else
			bitvec[i] = '0';
	}
}
