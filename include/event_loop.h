#ifndef _EVT_LOOP_H_
#define _EVT_LOOP_H_

#include <map>
#include <iostream>

#include "file_fd.h"


class event_loop
{
    public:
        event_loop();
        ~event_loop();

        bool  event_loop_init();
        bool  event_loop_deinit();

        bool  event_loop_add_fd(int fd, file_descriptor *fd_handler);
        bool  event_loop_remove_fd(int fd);

        bool  event_loop_run();

    private:

        int    _epoll_fd;
        bool   _terminate_loop;

        std::map<int, file_descriptor*>  _fds_map;

        bool  add_fd_to_read_write_watch(int fd, bool _poll_out);
        bool  remove_fd_from_read_write_watch(int fd, bool _poll_out);
        bool  should_terminate() { return _terminate_loop; }
};


#endif /* _EVT_LOOP_H_ */
