#pragma once

#include <string>
#include <vector>

class HeaderProperty {
	private:
		void*	operator new(size_t size);
		void	operator delete(void* ptr);
	public:
		std::string	name;
		std::string	value;

		HeaderProperty();
		HeaderProperty(const HeaderProperty& src);
		HeaderProperty& operator=(const HeaderProperty& src);
		~HeaderProperty();
};