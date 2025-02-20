#pragma once

#include <string>
#include <vector>
#include <map>

typedef std::map<std::string, std::string> PropertyMap;
typedef std::map<std::string, PropertyMap> ValueMap;
typedef std::pair<std::string, ValueMap> HeaderEntry;
typedef std::vector<HeaderEntry> HeaderList;

class HttpRequest;
class HttpResponse;

class HttpHeader {
	private:

		HttpHeader();
		HttpHeader(const HttpHeader& src);
		HttpHeader& operator=(const HttpHeader& src);
		~HttpHeader();

	public:

		static void			addEntry(HttpRequest& request, std::string headerLine);
		static HeaderEntry& findEntry(HttpRequest& request, std::string name);
		static ValueMap&	findValue(HeaderEntry& entry, std::string name);
		static PropertyMap&	findProperty(ValueMap& values, std::string name);
		static std::string&	findPropertyValue(PropertyMap& property, std::string name);
		static void			appendHeaders(std::stringstream& buffer, HttpResponse& response);
};