#include "timer.h"

#include <stdint.h>

#include <sys/timerfd.h>
#include <error.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include <iostream>

#define UNUSED_VAR(arg)  ((void)arg)

/**
 * @brief Construct a new timer::timer object
 *
 * @param timer_interval : in miliseconds
 * @param data
 */
timer::timer(uint64_t timer_interval, void *data): _timer_interval(timer_interval), _timer_data(data)
{
    int flags = TFD_CLOEXEC;

    int timer_fd = timerfd_create(CLOCK_REALTIME, 0);

    if( timer_fd == -1 )
    {
        std::cout << "Creating timer failed. Error - " << strerror(errno) << std::endl;
    }
    else
    {
        flags = fcntl(timer_fd, F_GETFL, 0);
        if(flags < 0)
        {
            std::cout << "Unable to read timer fd flags " <<  strerror(errno) << std::endl;
        }
        else
        {
            flags |= O_NONBLOCK;
            if(fcntl(timer_fd, F_SETFL, flags) < 0)
            {
                std::cout << "Unable to set timer fd as non blocking " << strerror(errno) << std::endl;
            }

            /*
             * To support zero value timer, a small timeout is provided.
             * else, the timer is disarmed by API.
             */
            if( !_timer_interval )
            {
                _timer_interval = 1;
                std::cout << "Starting zero value timer" << std::endl;
            }

            set_fd_value( timer_fd );
        }
    }
}


timer::~timer()
{
    stop_timer();
    _timer_interval = 0;
    _timer_data = NULL;
}


bool timer::start_timer()
{
    if( get_fd_value() == -1)
    {
        std::cout << "Timer not valid." << std::endl;
        return false;
    }

    /* Start the timer */
    struct itimerspec   its;

    its.it_value.tv_sec = _timer_interval / 1000;
    its.it_value.tv_nsec = (_timer_interval % 1000) * 1000 * 1000;
    /* Periodic timer not used. */
    its.it_interval.tv_sec = 0;
    its.it_interval.tv_nsec = 0;

    if( -1 == timerfd_settime(get_fd_value(), 0, &its, 0) )
    {
        std::cout << "Setting timer failed. Error - " << strerror(errno) << std::endl;
        return false;
    }

    return register_fd_with_epoll();
}


bool  timer::stop_timer()
{
    return unregister_fd_with_epoll();
}


bool  timer::reinit_timer(uint64_t timer_interval)
{
    stop_timer();
    _timer_interval = timer_interval;
    if( !_timer_interval )
    {
        _timer_interval = 1;
        std::cout << "Starting zero value timer" << std::endl;
    }

    /* reset the values */
    return start_timer();
}

void timer::receive_fd_event(int fd, uint32_t events)
{
    UNUSED_VAR(fd);
    UNUSED_VAR(events);

    int re_arm = 0;
    struct itimerspec   its;

    /* FD event receieved on timer expiry. */
    re_arm = timer_callback( _timer_data );

    if( !re_arm )
    {
        memset( &its, 0 , sizeof(its));

        if( -1 == timerfd_settime(get_fd_value(), 0, &its, 0) )
        {
            std::cout << "Removing timer failed. Error - " << strerror(errno) << std::endl;
            return;
        }
    }
    else
    {
        /* Re-arm the timer, if non-zero value was received. */
        start_timer();
    }

    return;
}
