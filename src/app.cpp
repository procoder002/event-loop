#include "app_base.h"
#include "timer.h"
#include "exec_env.h"

class app_timer : public timer {
  public:
    app_timer(uint64_t timer_interval, void *timer_data): timer(timer_interval, timer_data) {}

    int timer_callback(void* data) override {
        std::cout << " NON blocking app_timer callback called ! " << std::endl;
        return 1;
    }
};

 /*
    do {
        std::cout << " app_run function called ! " << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(2));

    } while(true);
*/

void app_run(int argc, char **argv)
{
    std::cout << "app_run() called for " << std::endl;

    exec_env::instance()->create_exec_env();

    app_timer one_shot_timer(2000, NULL);

    one_shot_timer.start_timer();

    exec_env::instance()->enter_run();

    exec_env::instance()->destroy_exec_env();
}

int main(int argc, char **argv)
{
    app_thread sample_app(app_run, argc, argv);

    sample_app.start();
}