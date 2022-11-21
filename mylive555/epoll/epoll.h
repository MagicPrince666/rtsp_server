/**
 * @file epoll.h
 * @author 黄李全 (846863428@qq.com)
 * @brief 
 * @version 0.1
 * @date 2022-11-18
 * @copyright Copyright (c) {2021} 个人版权所有
 */
#pragma once

#include <sys/socket.h>
#ifndef __APPLE__
#include <sys/epoll.h>
#else
#include <sys/event.h>
#endif

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

#define UDP_BUF_SIZE 1472
#define TCPBUF_SIZE 1460
#define MAXEVENTS 20
#define EPOLL_FD_SETSIZE 1024

#define MY_EPOLL Epoll::Instance()

class Epoll
{
public:
    static Epoll& Instance() {
        static Epoll instance;
        return instance;
    }

    ~Epoll();

    int EpollAdd(int fd, std::function<void()> handler);
    int EpollDel(int fd);
    int EpollLoop();
    bool EpoolQuit();

private:
    Epoll();
#ifndef __APPLE__
    struct epoll_event ev_, events_[MAXEVENTS];
#else
    struct kevent ev_[MAXEVENTS];
    struct kevent activeEvs_[MAXEVENTS];
#endif
    int epfd_;
    bool epoll_loop_{true};
    static Epoll *instance_;
    std::unordered_map<int, std::function<void()>> listeners_;
};
