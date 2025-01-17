#include "TCP.h"

TCPServer::TCPServer(int sock, const char *clnt_ip, int clnt_port, int buffer_size, FILE *logfile)
{
	this->sock = sock;
	strcpy(this->clnt_ip, clnt_ip);
	this->clnt_port = clnt_port;
	this->buffer_size = buffer_size;
	this->read_buffer = new char[this->buffer_size];
	this->read_remaining = 0;
	this->write_buffer = new char[this->buffer_size];
	this->write_remaining = 0;
	this->logfile = logfile;
}

TCPServer::~TCPServer()
{
	close(this->sock);
	delete[] this->read_buffer;
	delete[] this->write_buffer;
}

string TCPServer::ReadLine()
{
	string ret = "";
	while (1)
	{
		int n = recv(this->sock, this->read_buffer + this->read_remaining,
					 this->buffer_size - this->read_remaining, MSG_DONTWAIT);
		if (n > 0)
			this->read_remaining += n;

		char *enter = strchr(read_buffer, '\n');
		if (enter)
		{
			enter += 1; // 把指针挪动到\n后面
			char *dst = new char[this->buffer_size];
			strncpy(dst, this->read_buffer, enter - this->read_buffer);
			ret = dst;
			delete dst;
			this->read_remaining -= enter - this->read_buffer;
			if (this->read_remaining > 0)
				memmove(this->read_buffer, enter, this->read_remaining);
			if (this->read_remaining == 0)
				bzero(this->read_buffer, this->buffer_size);
			break;
		}
	}
	return ret;
}

string TCPServer::ReadLine(timeval &ot)
{
	fd_set fds;
	int maxfdp = this->sock + 1;
	string ret = "";
	while (1)
	{
		if (strlen(this->read_buffer) == 0)
		{
			FD_ZERO(&fds);
			FD_SET(this->sock, &fds);
			int sel = select(maxfdp, &fds, NULL, NULL, &ot);
			if (sel <= 0) // TimeOut or other Errors
			{
				return "";
			}
		}

		int n = recv(this->sock, this->read_buffer + this->read_remaining,
					 this->buffer_size - this->read_remaining, MSG_DONTWAIT);
		if (n > 0)
			this->read_remaining += n;

		char *enter = strchr(read_buffer, '\n');
		if (enter)
		{
			enter += 1; // 把指针挪动到\n后面
			char *dst = new char[this->buffer_size];
			strncpy(dst, this->read_buffer, enter - this->read_buffer);
			ret = dst;
			delete dst;
			this->read_remaining -= enter - this->read_buffer;
			if (this->read_remaining > 0)
				memmove(this->read_buffer, enter, this->read_remaining);
			if (this->read_remaining == 0)
				bzero(this->read_buffer, this->buffer_size);
			break;
		}
	}
	return ret;
}

int TCPServer::WriteLine(string &s)
{
	bzero(this->write_buffer, this->buffer_size);
	strcat(this->write_buffer, s.c_str());
	strcat(this->write_buffer, "\r\n");
	int len = s.length() + 2;
	int n = 0;

	while (n < len)
	{
		n += send(this->sock, this->write_buffer + n, len - n, MSG_DONTWAIT);
	}
	bzero(this->write_buffer, this->buffer_size);
	return n;
}

map<string, string> TCPServer::ReadBlock()
{
	map<string, string> block;
	while (1)
	{
		string line(ReadLine());
		//清换行符
		int posr = line.find('\r');
		int posn = line.find('\n');
		line = line.substr(0, min(posr, posn));
		//分离键值
		int pos = line.find(" = ");
		string key(line.substr(0, pos));
		string value(line.substr(pos + 3));

		if (key == "Length")
			break;
		else
			block[key] = value;
	}
	return block;
}

map<string, string> TCPServer::ReadBlock(int overtime)
{
	timeval ot = {overtime, 0};
	map<string, string> block;
	while (1)
	{
		string line(ReadLine(ot));
		if (ot.tv_sec < 0 || (ot.tv_sec == 0 && ot.tv_usec == 0))
		{
			map<string, string> errorblock;
			errorblock["Content"] = "GameTimeout";
			return errorblock;
		}
		fprintf(logfile, "(%s)[Server]: 从[%s:%d]接接收数据:%s\n", Tools::Get_Time().c_str(), clnt_ip, clnt_port, line.c_str());
		fflush(logfile);
		//清换行符
		int posr = line.find('\r');
		int posn = line.find('\n');
		line = line.substr(0, min(posr, posn));
		//分离键值
		int pos = line.find(" = ");
		string key(line.substr(0, pos));
		string value(line.substr(pos + 3));

		if (key == "Length")
			break;
		else
			block[key] = value;
	}
	return block;
}

int TCPServer::WriteBlock(map<string, string> &map, list<string> keylist)
{
	int len = 0;
	list<string>::iterator iter;
	iter = keylist.begin();
	while (iter != keylist.end())
	{
		string line((*iter) + " = " + map[*iter]);
		WriteLine(line);
		fprintf(logfile, "(%s)[Server]: 向[%s:%d]发送数据:%s\n", Tools::Get_Time().c_str(), clnt_ip, clnt_port, line.c_str());
		fflush(logfile);
		len += line.length() + 2;
		iter++;
	}
	string line("Length = ");
	len += line.length() + 2;
	int newlen = len + to_string(len).length();
	if (to_string(len).length() != to_string(newlen).length())
		newlen++;
	line += to_string(newlen);
	WriteLine(line);
	fprintf(logfile, "(%s)[Server]: 向[%s:%d]发送数据:%s\n", Tools::Get_Time().c_str(), clnt_ip, clnt_port, line.c_str());
	fflush(logfile);

	return newlen;
}

map<string, int> TCPServer::login(DatabaseAccess *db)
{
	map<string, string> block_out;
	block_out["Type"] = "SecurityString";
	srand((unsigned)time(0));
	string randstr = "";
	char c[10];
	for (int i = 0; i < 5; i++)
	{
		sprintf(c, "%08x", rand());
		randstr += c;
	}
	block_out["Content"] = randstr;
	list<string> AuthKeylist{"Type", "Content"};
	WriteBlock(block_out, AuthKeylist);

	map<string, string> block_in = ReadBlock();

	if (block_in["Type"] == "ParameterAuthenticate")
	{
		string auth_str = "";
		for (int i = 0; i < 40; i++)
		{
			char a, b, c;
			c = stoi(block_in["MD5"].substr(i * 2, 2), 0, 16);
			b = block_out["Content"][i];
			a = c ^ b;
			auth_str += a;
		}
		int ret = UserAuthenticate(auth_str, db);
		if (ret == -1) //登录失败
		{
			map<string, int> gameinfo;
			gameinfo["GameType"] = -1;
			gameinfo["StuNo"] = stoi(auth_str.substr(0, 7));
			return gameinfo;
		}
		else
		{
			map<string, int> gameinfo;
			timeval now_time;
			gettimeofday(&now_time, nullptr);
			gameinfo["GameStartTime_sec"] = now_time.tv_sec;
			gameinfo["GameStartTime_usec"] = now_time.tv_usec;
			gameinfo["StuNo"] = stoi(auth_str.substr(0, 7));
			if (ret == 0) //base模式
				gameinfo["GameType"] = 0;
			if (ret == 1) //competiiton模式
				gameinfo["GameType"] = 1;
			gameinfo["Row"] = stoi(block_in["Row"]);
			gameinfo["Col"] = stoi(block_in["Col"]);
			gameinfo["GameID"] = stoi(block_in["GameID"]);
			gameinfo["Delay"] = stoi(block_in["Delay"]) / 1000;

			return gameinfo;
		}
	}
	else
	{
		map<string, int> gameinfo;
		gameinfo["GameType"] = -1;
		return gameinfo;
	}
}

int TCPServer::UserAuthenticate(string auth_str, DatabaseAccess *db)
{
	//printf("auth_str=%s\n",auth_str.c_str());
	return db->Login(auth_str);
}
