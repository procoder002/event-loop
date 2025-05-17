#ifndef _APP_BASE_H_
#define _APP_BASE_H_

#include <string>
#include <iostream>
#include <thread>
#include <functional>
#include <atomic>


class app_thread {
    public:
      // Constructor that takes a callable (function or lambda) and its arguments
      template <typename Callable, typename... Args>
      app_thread(Callable&& func, Args&&... args)
          : _callback_function(std::bind(std::forward<Callable>(func), std::forward<Args>(args)...)), _is_running(false) {}

      // Destructor to join the thread if it's still joinable
      ~app_thread() {
          if (_thread.joinable()) { _thread.join(); }
      }

      bool isRunning() const { return _is_running; }

      // Function to start the thread
      void start() {
          if (!_is_running) {
              _is_running = true;
              _thread = std::thread(&app_thread::run, this);
          }
      }

    private:
      std::thread            _thread;
      std::atomic<bool>      _is_running; // Flag to check if the thread is running
      std::function<void()>  _callback_function;


      // Function that runs the thread
      virtual void run() {
          //std::this_thread::sleep_for(std::chrono::seconds(1));

          _callback_function();

          _is_running = false;
      }
};

#endif // _APP_BASE_H_