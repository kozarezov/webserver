#include "Response.hpp"


Response::Response()
    : _code_list()
    , _code(0)
    , _start_line()
    , _header()
    , _buf()
    , _body()
    , _body_size(0)
    , _header_size(0)
    , _error_flag(false)
    , _real_path()
{
	if (this->_code_list.empty())
		initCodeList();
}

Response::~Response() { }

Response::Response(const Response & other) { *this = other; }

Response & Response::operator=(const Response & other) {
    _code_list = other._code_list;
    _code = other._code;
    _start_line = other._start_line;
    _header = other._header;
    _buf = other._buf;
    _body = other._body;
    _body_size = other._body_size;
    _header_size = other._header_size;
    _error_flag = other._error_flag;
    _real_path = other._real_path;
    return *this;
}

void Response::initCodeList()
{
    this->_code_list[0] = "CODE NOT SETTED IN RESPONSE.CPP !";
    this->_code_list[100] = "Continue";
    this->_code_list[101] = "Switching Protocols";
    this->_code_list[200] = "OK";
    this->_code_list[201] = "Created";
    this->_code_list[202] = "Accepted";
    this->_code_list[203] = "Non-Authoritative Information";
    this->_code_list[204] = "No Content";
    this->_code_list[205] = "Reset Content";
    this->_code_list[206] = "Partial Content";
    this->_code_list[300] = "Multiple Choices";
    this->_code_list[301] = "Moved Permanently";
    this->_code_list[302] = "Found";
    this->_code_list[303] = "See Other";
    this->_code_list[304] = "Not Modified";
    this->_code_list[305] = "Use Proxy";
    this->_code_list[307] = "Temporary Redirect";
    this->_code_list[400] = "Bad Request";
    this->_code_list[401] = "Unauthorized";
    this->_code_list[402] = "Payment Required";
    this->_code_list[403] = "Forbidden";
    this->_code_list[404] = "Not Found";
    this->_code_list[405] = "Method Not Allowed";
    this->_code_list[406] = "Not Acceptable";
    this->_code_list[407] = "Proxy Authentication Required";
    this->_code_list[408] = "Request Timeout";
    this->_code_list[409] = "Conflict";
    this->_code_list[410] = "Gone";
    this->_code_list[411] = "Length Required";
    this->_code_list[412] = "Precondition Failed";
    this->_code_list[413] = "Payload Too Large";
    this->_code_list[414] = "URI Too Long";
    this->_code_list[415] = "Unsupported Media Type";
    this->_code_list[416] = "Range Not Satisfiable";
    this->_code_list[417] = "Expectation Failed";
    this->_code_list[426] = "Upgrade Required";
    this->_code_list[500] = "Internal Server Error";
    this->_code_list[501] = "Not Implemented";
    this->_code_list[502] = "Bad Gateway";
    this->_code_list[503] = "Service Unavailable";
    this->_code_list[504] = "Gateway Timeout";
    this->_code_list[505] = "HTTP Version Not Supported";
}

std::string Response::getMessage(int code)
{
	return (_code_list.at(code));
}

t_response_start_line Response::getStartLine(void) const
{
	return _start_line;
}

std::vector<char> & Response::getBuf(void) 
{
	return _buf;
}

std::vector<char> Response::getBuf(void) const 
{
	return _buf;
}

std::string & Response::getPath(void) 
{
	return _real_path;
}

std::string Response::getPath(void) const 
{
	return _real_path;
}

std::string & Response::getBody(void) 
{
	return _body;
}

std::string Response::getBody(void) const 
{
	return _body;
}

std::map<std::string, std::string> Response::getHeader() const 
{
    return _header;
}

int	Response::getBodySize() const
{
	return _body_size;
}

int	Response::getHeaderSize() const
{
	return _header_size;
}

std::string	Response::toString(int nbr)
{
	char *ch_code = ft_itoa(nbr);
	std::string status_code(ch_code);
	delete ch_code;

	return (status_code);
}

void		Response::generateResponseMsg(Request &request)
{
	std::string error_msg;
	std::string headers;

    /* std::cout << "check_syntax" << std::endl; */
	check_syntax(request);
    if (_code == 0)
    {
        /* std::cout << "check_path" << std::endl; */
        check_path(request);
    }
    if (_code == 0)
    {
        /* std::cout << "check_method" << std::endl; */
        check_method(request);
    }

    /* std::cout << "generateErrorMsg" << std::endl; */
	error_msg = generateErrorMsg();

	//_body = "Hello world!"; /* Пока не понимаю из чего формируется боди, видимо нужна отдельная функция */
	//_body_size = _body.length(); /* Размер боди должен считаться когда формируется боди */

    /* std::cout << "check_error" << std::endl; */
	check_error(error_msg);
    /* std::cout << "addHeader" << std::endl; */
	addHeader(request, headers);
    /* std::cout << "append CRLF" << std::endl; */
	headers.append(CRLF);

	std::string::iterator beg = headers.begin();
	std::string::iterator end = headers.end();

	while (beg != end)
	{
		_buf.push_back(*beg);
		++beg;
	}
    /* std::cout << "addBody" << std::endl; */
	addBody(error_msg);
}

std::string	Response::generateErrorMsg()
{
	std::string error;
	std::string	error_path = "./files/error.html";

	if (_error_flag == true)
	{
		int fd = open(error_path.c_str(), O_RDONLY);
		char buf[100];
		bzero(buf, 100);
		int pos = 0;
		while ((pos = read(fd, &buf, 100)) > 0)
			error.append(buf, pos);
		close(fd);
	}
	return (error);

}

void		Response::check_path(Request &request)
{
    (void)(request);
    /* method getArgument set as static */
	/* t_start_line temp = request.getStartLine(); */
	/* setPath(Parser::getArgument(temp.request_target, ft_strchr(temp.request_target, '/'))); */
	

	setPath("./files/index.html");


	/* Проверка пути
	путь получаем так: request.getStartLine().request_target
	нужно подумать от куда брать инфу о редиректах, возможно сюда нужно передавать setting или config
	
	после проверки записать путь setPath(std::string path);

	если не найден путь
	
	setCode(404);
	setErrorFlag(true);*/
}

void		Response::check_error(const std::string &error_msg)
{
	if (_code >= 400)
		_body_size = error_msg.length();
}

void		Response::check_syntax(Request &request)
{
    if (!request.isMethodValid())
    {
        setErrorFlag(true);
        setCode(400); 
        return;
    }
    if (request.getStartLine().request_target.size() > 8000)
    {
        setErrorFlag(true);
        setCode(501); 
        return;
    }
    if (!request.isRequestTargetValid())
    {
        setErrorFlag(true);
        setCode(400); 
        return;
    }
    if (!request.isHttpVersionValid())
    {
        setErrorFlag(true);
        setCode(505); 
        return;
    }
}

void		Response::check_method(Request &request)
{
    std::string method = request.getStartLine().method;
	ProcessMethod process;

	process.secretary_Request(request, *this, method);
	/* Проверка на вызываемый метод отсюда мы должны отправить наш итоговый запрос c названием метода в класс
	ProcessMethod(request, *this, std::string method(например "GET"));
	В итоге этот класс должен заполнить 
	- Response->_body
	- Response->_body_size
	- Response->setCode()
	
	если он деактивирован (методы GET HEAD не могут быть деактивированы)
	
	setCode(405);
	setErrorFlag(true); */
}

void		Response::check_authentication(Request &request)
{
    (void)(request);
	/* Проверка на аутентификацию, если нужна
	
	setCode(401);
	setErrorFlag(true); 
	
	P.S надо понять от куда в этом случае будет браться body для сообщения (скорее всего после этого должен измениться путь)*/
}

void		Response::setCode(int code)
{
	_code = code;
}

void		Response::setErrorFlag(bool flag)
{
	_error_flag = flag;
}

void	Response::setBody(const std::string &body)
{
	_body = body;
	setBodySize(body.length());
}

void	Response::setBodySize(size_t len)
{
	_body_size = len;
}

void	Response::setPath(std::string path)
{
	_real_path = path;
}

std::string Response::get_time() 
{
	/* Написать функцию даты и времени */

	return ("Fri, 17 Jul 2010 16:09:18 GMT+2");
}

void		Response::addHeader(Request &request, std::string &headers)
{
    (void)(request);
	/* std::map < std::string, std::string >::const_iterator it = request.getHeader().find("Connection"); */

    /* std::cout << "_code" << _code << std::endl; */

	_start_line.http_version = HTTP;
	_start_line.code = toString(_code);
	_start_line.message = getMessage(_code);
	_header["Date"] = get_time();
	_header["Server"] = "ServerCeccentr"; /* Из конфига? */
	/* _header[it->first] = it->second; /1* Connection где-то есть где-то нет? *1/ */
	/* _header["Accept-Ranges"] = "bytes"; /1* Всегда bytes? *1/ */
	_header["Content-Length"] = toString(_body_size);
	_header["Content-Type"] = "text/html"; /* Всегда text/html? */

	std::map < std::string, std::string >::iterator beg = _header.begin();
	std::map < std::string, std::string >::iterator end = _header.end();

	headers.append(
			_start_line.http_version + SPACE
			+ _start_line.code + SPACE
			+ _start_line.message + CRLF);

	while (beg != end)
	{
		headers.append(beg->first + ":" + SPACE);
		headers.append(beg->second + CRLF);
		++beg;
	}
}

void		Response::addBody(const std::string &error_msg)
{	
	if (_code < 400)
	{
		/* std::cout << "проверка!" << _body << std::endl; */

		std::string::iterator beg = _body.begin();
		std::string::iterator end = _body.end();
		
		while (beg != end)
		{
			_buf.push_back(*beg);
			++beg;
		}
	}
	else
	{
		std::string::const_iterator beg = error_msg.begin();
		std::string::const_iterator end = error_msg.end();

		while (beg != end)
		{
			_buf.push_back(*beg);
			++beg;
		}
	}
}

void        Response::clear()
{
    _code = 0;
    _start_line.code = "";
    _start_line.message = "";
    _start_line.http_version = "";
    _header.clear();
    _buf.clear();
	_body = "";
	_body_size = 0;
	_header_size = 0;
	_error_flag = false;
    _real_path = "";
}
