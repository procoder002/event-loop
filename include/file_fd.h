#ifndef _FILE_FD_H_
#define _FILE_FD_H_

#include <cstdint>

class file_descriptor
{
    public:
        //file_descriptor()
        virtual ~file_descriptor();

        bool   register_fd_with_epoll();
        bool   unregister_fd_with_epoll();

        void         set_fd_value(int fd) { _fd = fd; }
        int          get_fd_value() { return _fd; }

        void         set_poll_out(bool poll_out) { _poll_out = poll_out; }
        bool         get_poll_out() {return _poll_out; }

        virtual void receive_fd_event(int fd, uint32_t events ) = 0;

    private:
        int      _fd        = 1;
        bool     _poll_out  = false;
        bool     _registered_with_epoll = false;
};


#endif /* _FILE_FD_H_ */
