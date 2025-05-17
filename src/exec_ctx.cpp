
#include "exec_ctx.h"

exec_ctx::~exec_ctx()
{
    _exec_id = 0;
    if(_evt_loop != NULL)   deinit_exec_ctx();
}

bool  exec_ctx::init_exec_ctx()
{
    _evt_loop = new event_loop();

    return _evt_loop->event_loop_init();
}

bool  exec_ctx::deinit_exec_ctx()
{
    _evt_loop->event_loop_deinit();
    delete _evt_loop;
    _evt_loop = NULL;

    return true;
}

bool   exec_ctx::enter_run()
{
    _evt_loop->event_loop_run();

    return true;
}

bool   exec_ctx::add_fd_for_monitoring(int fd, file_descriptor *fd_handler )
{
    return _evt_loop->event_loop_add_fd( fd, fd_handler );
}

bool   exec_ctx::remove_fd_from_monitoring(int fd)
{
    return _evt_loop->event_loop_remove_fd( fd );
}

