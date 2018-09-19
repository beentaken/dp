#ifndef DEBUG_PRINT_H
#define DEBUG_PRINT_H
#include <sstream>
#include <fstream>
#include <mutex>
class Debug
{
public:
	void flush()
	{
		static std::ofstream file("output.txt");
		static std::mutex m;
	    std::lock_guard<std::mutex> guard(m);
	    file << oss.str() << std::endl;
		oss.clear();
		oss.str("");
	}
	std::ostringstream oss;
};	

template<typename T>
Debug& operator<<(Debug& debug, const T& rhs)
{
	debug.oss << rhs;
	return debug;
}

#endif