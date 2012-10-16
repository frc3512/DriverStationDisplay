////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2012 Laurent Gomila (laurent.gom@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "ContHttp.hpp"
#include <cctype>
#include <algorithm>
#include <iterator>
#include <sstream>


namespace
{
    // Convert a string to lower case
    std::string toLower(std::string str)
    {
        for (std::string::iterator i = str.begin(); i != str.end(); ++i)
            *i = static_cast<char>(std::tolower(*i));
        return str;
    }
}


namespace sf
{
////////////////////////////////////////////////////////////
ContHttp::Request::Request(const std::string& uri, Method method, const std::string& body)
{
    setMethod(method);
    setUri(uri);
    setHttpVersion(1, 0);
    setBody(body);
}


////////////////////////////////////////////////////////////
void ContHttp::Request::setField(const std::string& field, const std::string& value)
{
    m_fields[toLower(field)] = value;
}


////////////////////////////////////////////////////////////
void ContHttp::Request::setMethod(ContHttp::Request::Method method)
{
    m_method = method;
}


////////////////////////////////////////////////////////////
void ContHttp::Request::setUri(const std::string& uri)
{
    m_uri = uri;

    // Make sure it starts with a '/'
    if (m_uri.empty() || (m_uri[0] != '/'))
        m_uri.insert(0, "/");
}


////////////////////////////////////////////////////////////
void ContHttp::Request::setHttpVersion(unsigned int major, unsigned int minor)
{
    m_majorVersion = major;
    m_minorVersion = minor;
}


////////////////////////////////////////////////////////////
void ContHttp::Request::setBody(const std::string& body)
{
    m_body = body;
}


////////////////////////////////////////////////////////////
std::string ContHttp::Request::prepare() const
{
    std::ostringstream out;

    // Convert the method to its string representation
    std::string method;
    switch (m_method)
    {
        default :
        case Get :  method = "GET";  break;
        case Post : method = "POST"; break;
        case Head : method = "HEAD"; break;
    }

    // Write the first line containing the request type
    out << method << " " << m_uri << " ";
    out << "HTTP/" << m_majorVersion << "." << m_minorVersion << "\r\n";

    // Write fields
    for (FieldTable::const_iterator i = m_fields.begin(); i != m_fields.end(); ++i)
    {
        out << i->first << ": " << i->second << "\r\n";
    }

    // Use an extra \r\n to separate the header from the body
    out << "\r\n";

    // Add the body
    out << m_body;

    return out.str();
}


////////////////////////////////////////////////////////////
bool ContHttp::Request::hasField(const std::string& field) const
{
    return m_fields.find(toLower(field)) != m_fields.end();
}


////////////////////////////////////////////////////////////
ContHttp::Response::Response() :
m_status      (ConnectionFailed),
m_majorVersion(0),
m_minorVersion(0)
{

}


////////////////////////////////////////////////////////////
const std::string& ContHttp::Response::getField(const std::string& field) const
{
    FieldTable::const_iterator it = m_fields.find(toLower(field));
    if (it != m_fields.end())
    {
        return it->second;
    }
    else
    {
        static const std::string empty = "";
        return empty;
    }
}


////////////////////////////////////////////////////////////
ContHttp::Response::Status ContHttp::Response::getStatus() const
{
    return m_status;
}


////////////////////////////////////////////////////////////
unsigned int ContHttp::Response::getMajorHttpVersion() const
{
    return m_majorVersion;
}


////////////////////////////////////////////////////////////
unsigned int ContHttp::Response::getMinorHttpVersion() const
{
    return m_minorVersion;
}


////////////////////////////////////////////////////////////
const std::string& ContHttp::Response::getBody() const
{
    return m_body;
}


////////////////////////////////////////////////////////////
void ContHttp::Response::parse(const std::string& data)
{
    std::istringstream in(data);

    // Extract the HTTP version from the first line
    std::string version;
    if (in >> version)
    {
        if ((version.size() >= 8) && (version[6] == '.') &&
            (toLower(version.substr(0, 5)) == "http/")   &&
             isdigit(version[5]) && isdigit(version[7]))
        {
            m_majorVersion = version[5] - '0';
            m_minorVersion = version[7] - '0';
        }
        else
        {
            // Invalid HTTP version
            m_status = InvalidResponse;
            return;
        }
    }

    // Extract the status code from the first line
    int status;
    if (in >> status)
    {
        m_status = static_cast<Status>(status);
    }
    else
    {
        // Invalid status code
        m_status = InvalidResponse;
        return;
    }

    // Ignore the end of the first line
    in.ignore(10000, '\n');

    // Parse the other lines, which contain fields, one by one
    std::string line;
    while (std::getline(in, line) && (line.size() > 2))
    {
        std::string::size_type pos = line.find(": ");
        if (pos != std::string::npos)
        {
            // Extract the field name and its value
            std::string field = line.substr(0, pos);
            std::string value = line.substr(pos + 2);

            // Remove any trailing \r
            if (!value.empty() && (*value.rbegin() == '\r'))
                value.erase(value.size() - 1);

            // Add the field
            m_fields[toLower(field)] = value;
        }
    }

    // Finally extract the body
    m_body.clear();
    std::copy(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>(), std::back_inserter(m_body));
}


////////////////////////////////////////////////////////////
ContHttp::ContHttp() :
m_host(),
m_port(0)
{
	m_connection.connect( m_host , m_port , sf::milliseconds( 50 ) );
}


////////////////////////////////////////////////////////////
ContHttp::ContHttp(const std::string& host, unsigned short port)
{
    setHost(host, port);
    m_connection.connect( m_host , m_port );
}

ContHttp::~ContHttp() {
	 m_connection.disconnect();
}


////////////////////////////////////////////////////////////
void ContHttp::setHost(const std::string& host, unsigned short port)
{
    // Detect the protocol used
    std::string protocol = toLower(host.substr(0, 8));
    if (protocol.substr(0, 7) == "http://")
    {
        // HTTP protocol
        m_hostName = host.substr(7);
        m_port     = (port != 0 ? port : 80);
    }
    else if (protocol == "https://")
    {
        // HTTPS protocol
        m_hostName = host.substr(8);
        m_port     = (port != 0 ? port : 443);
    }
    else
    {
        // Undefined protocol - use HTTP
        m_hostName = host;
        m_port     = (port != 0 ? port : 80);
    }

    // Remove any trailing '/' from the host name
    if (!m_hostName.empty() && (*m_hostName.rbegin() == '/'))
        m_hostName.erase(m_hostName.size() - 1);

    m_host = IpAddress(m_hostName);
}


////////////////////////////////////////////////////////////
ContHttp::Response ContHttp::startStream(const ContHttp::Request& request)
{
    // First make sure that the request is valid -- add missing mandatory fields
    Request toSend(request);
    if (!toSend.hasField("From"))
    {
        toSend.setField("From", "user@sfml-dev.org");
    }
    if (!toSend.hasField("User-Agent"))
    {
        toSend.setField("User-Agent", "libsfml-network/2.x");
    }
    if (!toSend.hasField("Host"))
    {
        toSend.setField("Host", m_hostName);
    }
    if (!toSend.hasField("Content-Length"))
    {
        std::ostringstream out;
        out << toSend.m_body.size();
        toSend.setField("Content-Length", out.str());
    }
    if ((toSend.m_method == Request::Post) && !toSend.hasField("Content-Type"))
    {
        toSend.setField("Content-Type", "application/x-www-form-urlencoded");
    }
    if ((toSend.m_majorVersion * 10 + toSend.m_minorVersion >= 11) && !toSend.hasField("Connection"))
    {
        toSend.setField("Connection", "close");
    }

    // Prepare the response
    Response received;

	// Convert the request to string and send it through the connected socket
	std::string requestStr = toSend.prepare();

	if (!requestStr.empty())
	{
		// Send it through the socket
		if (m_connection.send(requestStr.c_str(), requestStr.size()) == Socket::Done)
		{
			// Wait for the server's response
			std::string receivedStr;
			std::size_t size = 0;
			char buffer[1024];
			while (m_connection.receive(buffer, sizeof(buffer), size) == Socket::Done)
			{ // TODO split stream into jpegs
				receivedStr.append(buffer, buffer + size);

			}

			// Build the Response object from the received data
			received.parse(receivedStr);
		}
	}

    return received;
}

} // namespace sf
