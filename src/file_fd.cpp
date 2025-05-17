
#include "file_fd.h"
#include <unistd.h>
#include <iostream>
#include "exec_env.h"

file_descriptor::~file_descriptor()
{
    if(_fd > 0) {
        close(_fd);
    }

    _fd = -1;
    _poll_out = false;
}

bool  file_descriptor::register_fd_with_epoll()
{
    if(_fd < 0) {
        std::cout << "Failed to register fd with epoll" << std::endl;
        return  false;
    }

    bool status = false;
    if(!_registered_with_epoll) {
        _registered_with_epoll = true;
        status = exec_env::instance()->add_fd_for_monitoring(_fd, this);
    }

	return status;
}

bool  file_descriptor::unregister_fd_with_epoll()
{
    if(_fd < 0) {
        std::cout << "Failed to unregister fd with epoll" << std::endl;
        return  false;
    }

    bool status = false;
    if(_registered_with_epoll) {

        _registered_with_epoll = false;
        status = exec_env::instance()->remove_fd_from_monitoring(_fd);
    }

    return status;
}

