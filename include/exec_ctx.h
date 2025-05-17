#ifndef _EXEC_CTX_H_
#define _EXEC_CTX_H_

#include <pthread.h>

#include "event_loop.h"

/*
 * Execution context (thread) specific information.
 * event_loop, shared memory etc.
 */

class exec_ctx
{
    public:
        exec_ctx(pthread_t tid) : _exec_id(tid), _evt_loop(NULL) {}
        ~exec_ctx();

        bool   init_exec_ctx();
        bool   deinit_exec_ctx();
        bool   enter_run();

        bool   add_fd_for_monitoring(int fd, file_descriptor *fd_handler);
        bool   remove_fd_from_monitoring(int fd);

    private:
        pthread_t       _exec_id;
        event_loop*     _evt_loop;
};

#endif /* _EXEC_CTX_H_ */
