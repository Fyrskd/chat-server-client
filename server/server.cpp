#define _VERSION "0.1.2"
#define _PORT
#define _IPADDRESS
#define BACKLOG 10

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <cstring>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <sys/epoll.h>
#include <map>
#include <unordered_map>


const int N = 1 << 14;

epoll_event evs[N];
int clientCount;
std::unordered_map<std::string, int> usedname;

struct Client {
	int fd;
	std::string name;
	Client() = default;
	Client(int _fd, std::string _name) : fd(_fd), name(_name) {}
};

void MySend(int sockfd, std::string&& Msg) {
	uint32_t MsgLen = htonl(static_cast<uint32_t>(Msg.size()));
	send(sockfd, reinterpret_cast<const char*>(&MsgLen), sizeof(MsgLen), 0);
	
	int totalBytes = 0;
	while(totalBytes < Msg.size()) {
		int bytes = send(sockfd, Msg.data() + totalBytes, Msg.size() - totalBytes, 0);
		if(bytes <= 0) {
			perror("send");
			exit(-1);
		}
		totalBytes += bytes;
	}
}

int MyRecv(int sockfd, char* Msg) {
	uint32_t MsgLen;
	int totalBytes = 0;
	while(totalBytes < sizeof(MsgLen)) {
		int bytes = recv(sockfd, reinterpret_cast<char*>(&MsgLen) + totalBytes, sizeof(MsgLen) - totalBytes, 0);
		if(bytes <= 0) return bytes;
		totalBytes += bytes;
	}
	MsgLen = ntohl(MsgLen);
	
	totalBytes = 0;
	while(totalBytes < MsgLen) {
		int bytes = recv(sockfd, Msg + totalBytes, MsgLen - totalBytes, 0);
		if(bytes <= 0) return bytes;
		totalBytes += bytes;
	}
	return MsgLen;
}

int main() {
	usedname["system"] = 1;
	int epfd = epoll_create(1);
	if(epfd < 0) {
		perror("epoll");
		return -1;
	}
	
	int serverfd = socket(AF_INET, SOCK_STREAM, 0);
	if(serverfd == -1) {
		perror("socket");
		return -1;
	}
	
	sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(_PORT);
	
	int reuse = 1;
	if (setsockopt(serverfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
		perror("setsockopt(SO_REUSEADDR) failed");
		return -1;
	}
	
	if(bind(serverfd, reinterpret_cast<sockaddr*>(&serveraddr), sizeof(serveraddr)) == -1) {
		perror("bind");
		return -1;
	}
	if(listen(serverfd, SOMAXCONN) == -1) {
		perror("listen");
		return -1;
	}
	
	epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = serverfd;
	
	if(epoll_ctl(epfd, EPOLL_CTL_ADD, serverfd, &ev) < 0) {
		perror("epoll_ctl Server");
		return -1;
	}
	std::cerr << "Waiting for connect...\n";
	
	std::map<int, Client> clientsmap;
	while(true) {
		int count = epoll_wait(epfd, evs, N, -1);
		if(count < 0) {
			perror("epoll_wait");
			break;
		}
		
		for(int i = 0; i < count; ++ i) {
			int fd = evs[i].data.fd;
			if(fd == serverfd) {
				sockaddr_in clientAddr;
				memset(&clientAddr, 0, sizeof(clientAddr));
				socklen_t socklen = sizeof(clientAddr);
				int clientfd = accept(serverfd, reinterpret_cast<sockaddr*>(&clientAddr), &socklen);
				if(clientfd < 0) {
					perror("accept");
					break;
				}
				
				ev.events = EPOLLIN;
				ev.data.fd = clientfd;
				
				if(epoll_ctl(epfd, EPOLL_CTL_ADD, clientfd, &ev)) {
					perror("epoll_ctl Client");
					break;
				}
				char clientIP[INET_ADDRSTRLEN];
				if(inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, sizeof(clientIP)) != NULL) {
					std::cerr << clientIP << ":" << ntohs(clientAddr.sin_port) << " connecting ...\n";
				}
				else {
					perror("Interpret Client IP");
					break;
				}
				
				clientsmap.emplace(clientfd, Client(clientfd, ""));
			}
			else {
				char buffer[N];
				memset(buffer, 0, sizeof buffer);
				int status = MyRecv(fd, buffer);
				if(status < 0) {
					perror("recv");
					break;
				}
				if(status == 0) {
					std::cerr << "User " << clientsmap[fd].name << " disconnected\n";
					epoll_ctl(epfd, EPOLL_CTL_DEL, fd, 0);
					close(fd);
					if(clientsmap[fd].name != "") {
						clientCount --;
					}
					std::cerr << clientCount << " users online\n";
					std::string name = clientsmap[fd].name;
					usedname.erase(name);
					clientsmap.erase(fd);
					for(auto &[clientfd, client] : clientsmap) {
						if(client.name != "") {
							MySend(clientfd, "!" + std::to_string(clientCount));
							MySend(clientfd, "[system] :" + name + " Logout");
						}
					}
				}
				else {
					std::string message(buffer, status);
					message = message.substr(1);
					if(clientsmap[fd].name == "") {
						if(buffer[0] == '!') {
							std::cerr << "User " << message << " try Login\n";
							if(usedname.count(message)) {
								MySend(fd, "?0");
								continue;
							}
							usedname[message] = 1;
							MySend(fd, "?1");
							clientsmap[fd].name = message;
							std::cerr << "User " << message << " Login\n";
							clientCount ++;
							std::cerr << clientCount << " users online\n";
							for(auto &[clientfd, client] : clientsmap) {
								if(client.name != "") {
									MySend(clientfd, "!" + std::to_string(clientCount));
									MySend(clientfd, "[system] :" + message + " Login");
								}
							}
						}
					}
					else {
						if(buffer[0] == '?') {
							std::cerr << "Get Message from " << clientsmap[fd].name << ": " << buffer + 1 << "\n";
							for(auto& [clientfd, client] : clientsmap) 
								if(clientfd != fd && client.name != "") {
									MySend(clientfd, "[" + clientsmap[fd].name + "] : " + message);
								}
						} else if(buffer[0] == '@') {
							MySend(fd, "!" + std::to_string(clientCount));
						}
					}
				}
			}
		}
	}
	close(epfd);
	close(serverfd);
	return 0;
}
