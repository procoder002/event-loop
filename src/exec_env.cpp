
#include "exec_env.h"

exec_env* exec_env::_instance = NULL;
pthread_mutex_t _instance_lock = PTHREAD_MUTEX_INITIALIZER;

exec_env::exec_env()
{
    _lock = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_init(&_lock, NULL);
}

exec_env::~exec_env()
{
    pthread_mutex_destroy( &_lock );
}

exec_env *  exec_env::instance()
{
    pthread_mutex_lock( &_instance_lock );
    if( NULL == _instance )
    {
        _instance = new exec_env;
    }
    pthread_mutex_unlock( &_instance_lock );

    return _instance;
}

bool  exec_env::create_exec_env()
{
    pthread_t   tid = pthread_self();
    std::cout << "exec_env::create_exec_env thread_id: " << tid << std::endl;

    pthread_mutex_lock( &_lock );

    exec_ctx  *ctx = new exec_ctx(tid);

    if( true != ctx->init_exec_ctx() )
    {
        std::cout << "create_exec_env: Failed to initialize execution ctx." << std::endl;
        pthread_mutex_unlock( &_lock );
        return false;
    }


    _exec_ctx_map[ tid ] = ctx;

    pthread_mutex_unlock( &_lock );

    return true;
}

bool  exec_env::enter_run()
{
    pthread_t   tid = pthread_self();
    std::cout << "enter_run:thread_id:" << tid << std::endl;

    pthread_mutex_lock( &_lock );

    exec_ctx * ctx = _exec_ctx_map[ tid ];

    pthread_mutex_unlock( &_lock );

    if( NULL == ctx )
    {
        std::cout << "enter_run: Failed to retrieve exec ctx. " << tid << std::endl;
        return false;
    }

    return ctx->enter_run();
}

bool  exec_env::add_fd_for_monitoring(int fd, file_descriptor *file_descriptor)
{
    if( (-1 == fd) || (NULL == file_descriptor) )
    {
        std::cout << "add_fd_for_monitoring: Invalid input." << std::endl;
        return false;
    }

    pthread_t   tid = pthread_self();

    pthread_mutex_lock( &_lock );

    exec_ctx * ctx = _exec_ctx_map[ tid ];

    pthread_mutex_unlock( &_lock );

    if( NULL == ctx )
    {
        std::cout << "add_fd_for_monitoring: Failed to retrieve exec ctx. " << tid << std::endl;
        return false;
    }

    return ctx->add_fd_for_monitoring( fd, file_descriptor );
}

bool  exec_env::remove_fd_from_monitoring(int fd)
{
    if( -1 == fd )
    {
        std::cout << "remove_fd_from_monitoring: Invalid input" << std::endl;
        return false;
    }

    pthread_t   tid = pthread_self();

    pthread_mutex_lock( &_lock );

    exec_ctx * ctx = _exec_ctx_map[ tid ];

    pthread_mutex_unlock( &_lock );

    if( NULL == ctx )
    {
        std::cout << "remove_fd_from_monitoring: Failed to retrieve exec ctx. " << tid  << std::endl;
        return false;
    }

    return ctx->remove_fd_from_monitoring( fd );
}

bool  exec_env::destroy_exec_env()
{
    pthread_t  tid = pthread_self();

    pthread_mutex_lock( &_lock );

    exec_ctx * ctx = _exec_ctx_map[ tid ];

    _exec_ctx_map.erase( tid );
    pthread_mutex_unlock( &_lock );

    if( NULL == ctx )
    {
        std::cout << "destroy_exec_env: Failed to retrieve execution ctx. " << tid << std::endl;
        return false;
    }

    ctx->deinit_exec_ctx();

    return true;
}

