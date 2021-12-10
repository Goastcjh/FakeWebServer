#include "Packet.h"

int Socket(int domian, int type, int protocol) {
    int n;
    if ((n = socket(domian, type, protocol)) < 0) {
        fprintf(stderr, "socket error\n");
    }
    return n;
}

int Bind(int sockfd, SA* myaddr, socklen_t addrlen) {
    int n;
    if ((n = bind(sockfd, myaddr, addrlen)) < 0) {
        fprintf(stderr, "bind error\n");
    }
    return n;
}

int Listen(int sockfd, int backlog) {
    int n;
    if ((n = listen(sockfd, backlog)) < 0) {
        fprintf(stderr, "listen error\n");
    }
    return n;
}

int Accept(int sockfd, SA* cliaddr, socklen_t* addrlen) {
    int n;
    if ((n = accept(sockfd, cliaddr, addrlen)) < 0) {
        fprintf(stderr, "accpet error\n");
    }
}

int Fcntl(int fd, int cmd, long arg) {
    int n;
    if ((n = fcntl(fd, cmd, arg)) < 0) {
        fprintf(stderr, "fcntl error\n");
    }
    return n;
}

int setnonblocking(int sockfd) {
    int val = Fcntl(sockfd, F_GETFL, 0);
    return Fcntl(sockfd, F_SETFL, val | O_NONBLOCK);
}

int Epoll_create(int size) {
    int n;
    if ((n = epoll_create(size)) < 0) {
        fprintf(stderr, "epoll create error\n");
    }
    return n;
}

int Epoll_ctl(int epfd, int op, int fd, SE* event) {
    int n;
    if ((n = epoll_ctl(epfd, op, fd, event)) < 0) {
        fprintf(stderr, "epoll ctl error\n");
    }
    return n;
}


int Epoll_wait(int epfd, SE* events, int maxevents, int timeout) {
    int n;
    if ((n = epoll_wait(epfd, events, maxevents, timeout)) < 0) {
        fprintf(stderr, "epoll wait error\n");
    }
    return n;
}

ssize_t readv(int sockfd, const struct iovec* iov, int iovcnt) {
    return readv(sockfd, iov, iovcnt);
}
//UNIX安全读写函数
ssize_t readn(int fd, string& inbuffer, bool& zero) {
    ssize_t nread;
    ssize_t readSum = 0;
    while (true) {
        char buff[MAXLINE];
        if ((nread = read(fd, buff, MAXLINE)) < 0) {
            if (EINTR == errno) {
                continue;
            }
            else if (EAGAIN == errno) {
                return readSum;
            }
            else {
                fprintf(stderr, "read error\n");
                return -1;
            }
        }
        else if (!nread) {
            zero = true;
            break;
        }
        readSum += nread;
        inbuffer += string(buff, buff + nread);
    }
    return readSum;
}
//UNIX安全读写函数
ssize_t writen(int fd, const void* vptr, size_t n) {
    ssize_t nleft;
    ssize_t nwritten;
    const char* ptr = (char *)vptr;
    nleft = n;
    while (nleft > 0) {
        if ((nwritten = write(fd, ptr, nleft)) < 0) {
            if (nwritten < 0 && EINTR == errno) {   //被信号中断重写 call_back()
                nwritten = 0;
            }
            else {
                return -1;
            }
        }
        nleft -= nwritten;
        ptr += nwritten;
    }
    return n;
}

int Open(int sockfd) {
    int n;
    if ((n = close(sockfd)) < 0) {
        fprintf(stderr, "close sockfd error\n");
    }
    return n;
}

int Setsockopt(int sockfd, int level, int optname, const void* optval, socklen_t optlen) {
    int n;
    if ((n = setsockopt(sockfd, level, optname, optval, optlen)) < 0) {
        fprintf(stderr, "set sockopt error\n");
    }
    return n;
}

//support IPV6
/*
struct addrinfo
{
  int ai_flags;			/* Input flags.  
  int ai_family;		/* Protocol family for socket.  
  int ai_socktype;		/* Socket type.  
  int ai_protocol;		/* Protocol for socket.  
  socklen_t ai_addrlen;		/* Length of socket address.  
  struct sockaddr *ai_addr;	/* Socket address for socket.  
  char *ai_canonname;		/* Canonical name for service location. 
  struct addrinfo *ai_next;	/* Pointer to next in list.  
};

ai_flags:Value Meaning
AI_PASSIVE 套接字地址将用于调用bind 函数
AI_CANONNAME 返回规范名称
AI_NUMERICHOST 传递给getaddrinfo函数的nodename参数必须是数字字符串。
AI_ALL If this bit is set, a request is made for IPv6 addresses and IPv4 addresses with AI_V4MAPPED.
AI_ADDRCONFIG 只有配置了全局地址后，getaddrinfo才会解析。 IPv6和IPv4环回地址不被认为是有效的全局地址。
AI_V4MAPPED 如果对IPv6地址的getaddrinfo请求失败，则对IPv4地址进行名称服务请求，这些地址将转换为IPv4映射IPv6地址格式。
AI_NON_AUTHORITATIVE 地址信息可以来自非授权命名空间提供商
AI_SECURE 地址信息来自安全信道。
AI_RETURN_PREFERRED_NAMES 地址信息是用于用户的优选名称。
AI_FQDN getaddrinfo将返回名称最终解析为的完全限定域名。 完全限定域名在ai_canonname成员中返回。
这与AI_CANONNAME位标记不同，后者返回在DNS中注册的规范名称，该名称可能与平面名称解析为的完全限定域名不同。
只能设置AI_FQDN和AI_CANONNAME位中的一个。 如果EAI_BADFLAGS同时存在这两个标志，getaddrinfo函数将失败。
AI_FILESERVER 命名空间提供程序提示正在查询的主机名正在文件共享方案中使用。 命名空间提供程序可以忽略此提示。

ai_family:
AF_UNSPEC 地址系列未指定。
AF_INET IPv4 address family.
AF_NETBIOS NetBIOS地址系列。
AF_INET6 IPv6 address family.
AF_IRDA The Infrared Data Association address family.
AF_BTH Bluetooth address family.

ai_protocol:
IPPROTO_TCP 传输控制协议（TCP）。 当ai_family成员为AF_INET或AF_INET6且ai_socktype成员为SOCK_STREAM时，这是一个可能的值
IPPROTO_UDP 用户数据报协议（UDP）。 当ai_family成员为AF_INET或AF_INET6且类型参数为SOCK_DGRAM时，这是一个可能的值。
IPPROTO_RM PGM协议用于可靠的组播。 当ai_family成员为AF_INET且ai_socktype成员为SOCK_RDM时，这是一个可能的值。 在为Windows Vista及更高版本发布的Windows SDK上，此值也称为IPPROTO_PGM。
可能的选项特定于指定的地址系列和套接字类型。
如果为ai_protocol指定了值0，则调用者不希望指定协议，服务提供者将选择要使用的ai_protocol。 对于IPv4和IPv6之外的协议，将ai_protocol设置为零。
下表列出了ai_protocol成员的通用值，尽管其他许多值也是可能的。

ai_socktype:
SOCK_STREAM 使用OOB数据传输机制提供顺序，可靠，双向，基于连接的字节流。使用Internet地址系列（AF_INET或AF_INET6）的传输控制协议（TCP）。如果ai_family成员是AF_IRDA，则SOCK_STREAM是唯一支持的套接字类型。
SOCK_DGRAM 支持数据报，它是无连接的，不可靠的固定（通常小）最大长度的缓冲区。对Internet地址系列（AF_INET或AF_INET6）使用用户数据报协议（UDP）。
SOCK_RAW 提供一个原始套接字，允许应用程序处理下一个上层协议头。要操作IPv4标头，必须在套接字上设置IP_HDRINCL套接字选项。要操作IPv6头，必须在套接字上设置IPV6_HDRINCL套接字选项。
SOCK_RDM 提供可靠的消息数据报。这种类型的示例是在Windows中的实用通用多播（PGM）多播协议实现，通常被称为可靠多播节目。
SOCK_SEQPACKET 基于数据报提供伪流包。
*/
int tcp_listen(const char* host, const char* serv, socklen_t* addrlen) {
    int listenfd, n;
    const int on = 1;
    struct addrinfo hints;
    struct addrinfo* res, *ressave;
    bzero(&hints, sizeof(struct addrinfo));
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    if (n = getaddrinfo(host, serv, &hints, &res) != 0) {
        fprintf(stderr, "tcp_listen error\n");
    }
    ressave = res;
    do {
        listenfd = Socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (listenfd < 0) {
            continue;
        }
        Setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
        if (!Bind(listenfd, res->ai_addr, res->ai_addrlen)) {
            break;
        }
        Close(listenfd);
    }while (res = res->ai_next);
    if (!res) {
        fprintf(stderr, "tcp_listen error\n");
    }

    Listen(listenfd, getconf().getlistenq());
    if (addrlen) {
        *addrlen = res->ai_addrlen;
    }

    freeaddrinfo(ressave);
    return listenfd;
}

int Eventfd(unsigned int initval, int flags) {
    int n;
    if ((n = eventfd(initval, flags)) < 0) {
        fprintf(stderr, "eventfd error\n");
    }
    return n;
}

