#include "event_loop.h"

#include <sys/epoll.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#define EVT_LOOP_BACKOFF_TIME_USEC 2000

#define EXPECT_LIKELY(x)      __builtin_expect(!!(x), 1)

event_loop::event_loop()
{
    _epoll_fd = -1;
    _terminate_loop = false;
}


event_loop::~event_loop()
{
    _epoll_fd = -1;
    _terminate_loop = true;
}


bool  event_loop::event_loop_init()
{
    if( _epoll_fd < 0 )
    {
        _epoll_fd = epoll_create(32);
        std::cout << "In evt_loop_init _epoll_fd: " << _epoll_fd << std::endl;

        if( _epoll_fd < 0 )
        {
            std::cout << "epoll_create failed. Errorno: " << errno << std::endl;
            return false;
        }
        std::cout << "epoll_create successfull. epoll_fd:" << _epoll_fd << " terminate_loop:" << _terminate_loop << std::endl;

        return true;
    }
    else
    {
        std::cout << "evt_loop_init epoll already created" << std::endl;
        return false;
    }
}


bool  event_loop::event_loop_deinit()
{
    _terminate_loop = true;
    std::cout << "Closing epoll_fd:" << _epoll_fd << " terminate_loop:" << _terminate_loop << std::endl;
    close(_epoll_fd);
    _epoll_fd = -1;

    return true;
}


bool event_loop::add_fd_to_read_write_watch(int fd, bool _poll_out = false)
{
    struct epoll_event ev;

    memset(&ev, 0, sizeof(struct epoll_event));
    if(_poll_out)
    {
        ev.events = EPOLLIN | EPOLLERR | EPOLLHUP | EPOLLRDHUP | EPOLLOUT;
    }
    else
    {
        ev.events = EPOLLIN | EPOLLERR | EPOLLHUP | EPOLLRDHUP ;
    }
    ev.data.fd = fd;

    if(_epoll_fd < 0)
    {
        std::cout << " Calling evt_loop_init since _epoll_fd (" << _epoll_fd << ") is not initialized" << std::endl;
        event_loop_init();
    }

    int ret = epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, fd, &ev);
    if(ret != 0)
    {
        std::cout << "[epoll_fd: " << _epoll_fd << " ] Could not add fd" << fd << " because of error " << strerror(errno) << std::endl;
        return false;
    }
    std::cout<< "[epoll_fd: " << _epoll_fd << " ] event_loop fd added to watch " << fd << std::endl;
    return true;
}


bool event_loop::remove_fd_from_read_write_watch(int fd, bool _poll_out = false )
{
    struct epoll_event ev;

    memset(&ev, 0, sizeof(struct epoll_event));
    if(_poll_out)
    {
        ev.events = EPOLLIN | EPOLLERR | EPOLLHUP | EPOLLRDHUP | EPOLLOUT;
    }
    else
    {
	    ev.events = EPOLLIN | EPOLLERR | EPOLLHUP | EPOLLRDHUP ;
    }
    ev.data.fd = fd;

    int ret = epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, fd, &ev);
    if(ret != 0)
    {
        std::cout << " Could not remove fd " << fd <<" because of error " << strerror(errno) << std::endl;
        return false;
    }

    std::cout<< "event_loop fd removed from watch " << fd << std::endl;
    return true;
}

bool  event_loop::event_loop_add_fd(int fd, file_descriptor *fd_handler)
{
    std::map<int, file_descriptor*>::iterator mit;

    mit = _fds_map.find(fd);
    if(mit != _fds_map.end())
    {
        std::cout << "duplicate add_fd request, fd_handler already registered for fd: " << fd << std::endl;
        return false;
    }

    auto status = add_fd_to_read_write_watch(fd, fd_handler->get_poll_out());
    if(true == status)
    {
        _fds_map[ fd ] = fd_handler;
        std::cout<< "evt_loop_add_fd: fd added to map " << fd << std::endl;
    }
    else
    {
        std::cout<< "evt_loop_add_fd: fd add failed:" << fd << "with error code:" << status << std::endl;
        return false;
    }

    return true;
}


bool  event_loop::event_loop_remove_fd(int fd)
{
    std::map<int, file_descriptor*>::iterator mit;

    mit = _fds_map.find(fd);
    if(mit == _fds_map.end())
    {
        std::cout<< "evt_loop_remove_fd: handler not registered for fd: " << fd << std::endl;
        return false;
    }

    if(true == remove_fd_from_read_write_watch(fd, mit->second->get_poll_out() ))
    {
        //_fds_map.erase( mit );
        std::cout<< "evt_loop_remove_fd: fd removed from map " << fd << std::endl;
    }
    _fds_map.erase( mit );

    return true;
}


bool  event_loop::event_loop_run()
{
    struct epoll_event *events;
    const  int maxevents = 10;
    int    nfds;
    std::map<int, file_descriptor*>::iterator fd_mit;

    events = (struct epoll_event *) calloc(maxevents, sizeof(struct epoll_event));

    if (events == NULL) {
        std::cout << "Failed to allocate memory for events" << std::endl;
        return false;
    }

    while( !should_terminate() )
    {
        nfds = epoll_wait( _epoll_fd, events, maxevents, -1);
        if (EXPECT_LIKELY(-1 != nfds))
        {
            for(int n=0; n < nfds; ++n)
            {
                fd_mit = _fds_map.find(events[n].data.fd);
                if( fd_mit != _fds_map.end() )
                {
                    fd_mit->second->receive_fd_event( events[n].data.fd, events[n].events );
                }
                else
                {
                    std::cout << "evt_loop_run: Unknown fd event. fd: " << events[n].data.fd << std::endl;
                }
            }
        }
        memset(events, 0, maxevents * sizeof(struct epoll_event));

        usleep(EVT_LOOP_BACKOFF_TIME_USEC);
    }

    if(events)  free(events);

    return true;
}

