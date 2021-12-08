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