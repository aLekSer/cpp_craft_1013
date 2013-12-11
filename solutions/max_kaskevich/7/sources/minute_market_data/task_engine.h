#pragma once
#include "thread_safe_queue.h"
#include <boost/thread.hpp>

namespace multicast_communication
{

    class task_engine
    {
        typedef std::function< void() > task_type;
        const size_t max_threads_;
        thread_safe_queue< task_type > queue_;
        bool need_stop_;
        bool force_stop_;
        boost::thread_group threads_;
        boost::mutex mtx_;

    public:
        explicit task_engine( const size_t max_threads = boost::thread::hardware_concurrency() );
        ~task_engine( void );
        void start();
        void stop( bool force_stop = true );

        bool add_task( task_type task );
    private:
        void thread_func();
    };

}
