#ifndef _TIMER_H_
#define _TIMER_H_

#include "file_fd.h"


 /* TODO: Need a timer manager to manage all timers. */

class timer: public file_descriptor
{
    public:
        timer(uint64_t timer_interval, void *timer_data);
        virtual ~timer();

        bool start_timer();
        bool stop_timer();
        bool reinit_timer(uint64_t timer_interval);

        //void *  get_timer_data() { return _timer_data; }

        void receive_fd_event(int fd, uint32_t events) override;
        virtual int timer_callback(void* data) = 0;

    private:
        void     *_timer_data;
        uint64_t  _timer_interval;
};

#endif /* _TIMER_H_ */
