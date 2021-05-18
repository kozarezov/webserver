#include "ProcessMethod.hpp"
#include <string>

ProcessMethod::ProcessMethod()
{
}

ProcessMethod::~ProcessMethod()
{
}

void	ProcessMethod::secretary_Request(Request &request, Response &respone, Setting * config, const std::string &method)
{
	_response = &respone;
	_request = &request;
	_config = config;
	_method = method;
	_stat_num = stat(_response->getPath().c_str(), &_stat);

	int i = _response->getLocationRespond();

	if (method == "GET")
	{
		if (i == -1)
			processGetRequest(-1);
		else if (_config->getLocationGet(i))
			processGetRequest(i);
		else
			_response->setCode(405);
	}
	if (method == "HEAD")
	{
		if (i == -1)
			processHeadRequest(-1);
		else if (_config->getLocationHead(i))
			processHeadRequest(i);
		else
			_response->setCode(405);
	}
	if (method == "POST")
	{
		if (i == -1)
			processPostRequest(-1);
		else if (_config->getLocationPost(i))
			processPostRequest(i);
		else
			_response->setCode(405);
	}
	if (method == "PUT")
	{
		if (i == -1)
			processPutRequest();
		else if (_config->getLocationPut(i))
			processPutRequest();
		else
			_response->setCode(405);
	}
}

void	ProcessMethod::processGetRequest(int i)
{
	bool autoindex = true;

	if (i != -1 && _config->getLocationAutoindex(i) == 0)
		autoindex = false;
	_response->setCode(200);
	if (S_ISLNK(_stat.st_mode) || S_ISREG(_stat.st_mode))
		_response->setBody(readPath(_response->getPath()));
	else if (S_ISDIR(_stat.st_mode) && autoindex == true)
		_response->setBody(generateAutoindex(_response->getPath()));
	else
		_response->setCode(404);
}

void	ProcessMethod::processHeadRequest(int i)
{
	bool autoindex = true;

	if (i != -1 && _config->getLocationAutoindex(i) == 0)
		autoindex = false;
	_response->setCode(200);
	if (S_ISDIR(_stat.st_mode) && autoindex == false)
		_response->setCode(404);
}

void	ProcessMethod::processPostRequest(int i)
{
    /* if (i == -1) */
    /* { */
		/* _response->setCode(404); */
    /*     return; */
    /* } */#include <sys/types.h>
#include <sys/wait.h>
    if (_request->getHeader()["Content-Length"] == "0")
    {
        _response->setCode(405);
        return;
    }
    if (1 == 1)
    {
        std::string exec_prog;
        for (int i = 0; i < _config->getCGISize(); ++i)
        {
            if (_config->getCGIType(i) == "php")
               exec_prog = _config->getCGIPath(i); 
        }
        if (exec_prog != "")
            _execCGI(exec_prog);
        else
            processGetRequest(i);
    }
    else
    {
        /* if didnt exist cgi, then processGetRequest */
        processGetRequest(i);
    }
}

void	ProcessMethod::processPutRequest()
{
	int fd = 0;
	size_t check = 0;
	std::string request_body(_request->getBody().begin(),_request->getBody().end());
	
    if (_request->getHeader()["Content-Length"] == "0")
    {
		_response->setCode(405);
        return;
    }
	if (S_ISDIR(_stat.st_mode))
		_response->setCode(404);
	if ((fd = open(_response->getPath().c_str(), O_RDWR | O_CREAT | O_TRUNC, 0666)) < 0)
		_response->setCode(500);
	else
	{
		check = write(fd, request_body.c_str(), request_body.length());
		if (check != request_body.length()) {
			_response->setCode(200);
			std::cout << "Write error" << std::endl;
			close(fd);
		}
		else if (_stat_num != -1)
			_response->setCode(200);
		else
			_response->setCode(201);
		close(fd);
	}
}

std::string ProcessMethod::readPath(std::string path)
{
	int 		fd = open(path.c_str(), O_RDONLY);;
	char		buf[100];
	std::string body;

	if (!fd)
		std::cerr << "File doesn`t open" << std::endl;
	bzero(buf, 100);
	int pos = 0;
	while ((pos = read(fd, &buf, 100)) > 0)
		body.append(buf, pos);
	close(fd);
	return (body);
}

std::string ProcessMethod::generateAutoindex(std::string path) 
{
	std::string 	autoindex;
	DIR*			directory = opendir(path.c_str());
	struct dirent*	entry = 0;

	if (directory)
	{
		autoindex.append("<html><head><title>Index of </title></head><body><h1>Index of </h1><br><hr><a href=\"../\">../</a><br>");
		while ((entry = readdir(directory)) != 0) 
			autoindex.append("<a href=\"" + std::string(entry->d_name) + "\">" + std::string(entry->d_name) + "</a><br>");
		autoindex.append("</body></html>");
	}
	return (autoindex);
}

void ProcessMethod::_execCGI(const std::string & exec_prog)
{
    /* https://stackoverflow.com/a/47716623 */
    std::vector<char*> envVector;
    envVector.push_back(const_cast<char*>("AUTH_TYPE="));
    std::string content_length = "CONTENT_LENGTH=" + _request->getHeader()["Content-Length"];
    envVector.push_back(const_cast<char*>(content_length.c_str()));
    envVector.push_back(const_cast<char*>("CONTENT_TYPE=application/x-www-form-urlencoded"));
    envVector.push_back(const_cast<char*>("GATEWAY_INTERFACE=CGI/1.1"));
    /* envVector.push_back(const_cast<char*>("PATH_INFO=")); */
    /* envVector.push_back(const_cast<char*>("PATH_TRANSLATED=")); */
    /* envVector.push_back(const_cast<char*>("QUERY_STRING=")); */
    std::string remote_addr = "REMOTE_ADDR=" + std::string(_config->getHost());
    envVector.push_back(const_cast<char*>(remote_addr.c_str()));
    /* envVector.push_back(const_cast<char*>("REMOTE_IDENT=")); */
    /* envVector.push_back(const_cast<char*>("REMOTE_USER=")); */
    std::string request_method = "REQUEST_METHOD=" + _request->getStartLine().method;
    envVector.push_back(const_cast<char*>(request_method.c_str()));
    /* envVector.push_back(const_cast<char*>("REQUEST_URI=")); */
    envVector.push_back(const_cast<char*>("SCRIPT_NAME=test.php"));
    envVector.push_back(const_cast<char*>("REDIRECT_STATUS=200"));
    std::string server_name = "SERVER_NAME=" + _config->getServerName();
    envVector.push_back(const_cast<char*>(server_name.c_str()));
    char *port = ft_itoa(_config->getPort());
    std::string server_port = "SERVER_PORT=" + std::string(port);
    free(port);
    envVector.push_back(const_cast<char*>(server_port.c_str()));
    envVector.push_back(const_cast<char*>("SERVER_PROTOCOL=HTTP/1.1"));
    envVector.push_back(const_cast<char*>("SERVER_SOFTWARE=1.0"));
    std::string script_filename = "SCRIPT_FILENAME=" + _response->getPath(); 
    envVector.push_back(const_cast<char*>(script_filename.c_str()));
    envVector.push_back(0);
	if (_config->getDebugLevel() > 1)
    {
        std::cout << utils::GRA << "------" << " CGI envirements start " << "------" << utils::RES << std::endl;
        for (size_t i = 0; envVector[i] != 0; ++i)
            std::cout << "  " << envVector[i] << std::endl;
        std::cout << utils::GRA << "------" << " CGI envirements end " << "------" << utils::RES << std::endl;
    }
    char **env = envVector.data();

    /* https://stackoverflow.com/a/39395978 */
    int child_to_parent[2];
    int parent_to_child[2];

	
    if (pipe(child_to_parent) == -1 || pipe(parent_to_child) == -1) {
		std::cout << "Error: pipe error" << std::endl;
		exit(EXIT_FAILURE);
	}

    pid_t pid = fork();

    if (pid == 0) {
        /* In child process */
        dup2(child_to_parent[1], STDOUT_FILENO);
        close(child_to_parent[0]);

        dup2(parent_to_child[0], STDIN_FILENO);
        close(parent_to_child[1]);

        char *arg[] = { const_cast<char*>(exec_prog.c_str()), 0 };
        execve(exec_prog.c_str(), &arg[0], &env[0]);
    } else {
        /* In parent process */
        close(child_to_parent[1]);
        close(parent_to_child[0]);

        std::vector<char> body = _request->getBody();
        /* std::cout << "body: " << &body[0] << std::endl; */
        if (write(parent_to_child[1], &body[0], body.size()) < 0)
            throw std::runtime_error(std::string("execCGI write: ") + strerror(errno));
        int status;
        if (waitpid(pid, &status, 0) < 0)
            throw std::runtime_error(std::string("execCGI waitpid") + strerror(errno));
        char                line[5];
        int                 m;
        std::vector<char>   buf;
        while ((m = read(child_to_parent[0], line, 5)) > 0)
        {
            if (m == -1)
                throw std::runtime_error(std::string("write: ") + strerror(errno));
            buf.insert(buf.end(), line, line + m);
        }
        if (Request::bufContains(&buf, "\r\n\r\n"))
        {
            std::vector<char> headers_buf_from_cgi;
            headers_buf_from_cgi = Request::deleteHeaderInBuf(&buf);
            std::map<std::string, std::string> header_map;
            header_map = utils::parseBufToHeaderMap(_response->getHeader(),
                    headers_buf_from_cgi);
            _response->setHeader(header_map);
        }
        _response->setBody(std::string(buf.begin(), buf.end()));
        _response->setCode(200);
    }
}
