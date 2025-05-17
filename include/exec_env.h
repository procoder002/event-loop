
#ifndef _EXEC_ENV_H_
#define _EXEC_ENV_H_

#include <pthread.h>

#include <map>
#include <iostream>

#include "exec_ctx.h"

/*
 * Defines Execution environment of a independently runnable process.
 *
 * exec_ctx -> Represents one execution context (thread).
 * exec_env -> Singleton class to manage all exec_ctx (Per process).
 */

class exec_env
{
    public:
        ~exec_env();

        static exec_env *  instance();

        bool  create_exec_env();
        bool  destroy_exec_env();

        bool  enter_run();
        bool  add_fd_for_monitoring(int fd, file_descriptor *file_descriptor);
        bool  remove_fd_from_monitoring(int fd);

    private:
        exec_env();
        static  exec_env  *_instance;
        pthread_mutex_t       _lock;
        std::map<pthread_t, exec_ctx *>  _exec_ctx_map;
};


#endif /* _EXEC_ENV_H_ */
