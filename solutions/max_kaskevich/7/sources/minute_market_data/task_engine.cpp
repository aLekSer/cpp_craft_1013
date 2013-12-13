#include "task_engine.h"

multicast_communication::task_engine::task_engine( const size_t max_threads ) :
    max_threads_( ( max_threads > 0 ? max_threads : 1 ) ),
    need_stop_( true ),
    force_stop_( false )
{
    start();
}

multicast_communication::task_engine::~task_engine()
{
    stop();
}

void multicast_communication::task_engine::start()
{
    boost::mutex::scoped_lock lock( mtx_ );
    if ( !need_stop_ )
    {
        return;
    }
    need_stop_ = false;

    for( size_t i = 0; i < max_threads_; ++i )
    {
        threads_.create_thread( boost::bind( &multicast_communication::task_engine::thread_func, this ) );
    }
}

void multicast_communication::task_engine::stop( bool force_stop )
{
    boost::mutex::scoped_lock lock( mtx_ );
    force_stop_ = force_stop;
    need_stop_ = true;
    if( force_stop_ )
    {
        queue_.disable_wait();
    }
    else
    {
        while( !queue_.empty() ) // TODO without bad spin lock
        {
        }
    }

    threads_.join_all();
}

bool multicast_communication::task_engine::add_task( task_type task )
{
    boost::mutex::scoped_lock lock( mtx_ );
    if ( need_stop_ )
    {
        return false;
    }
    queue_.push( task );
    return true;        
}

void multicast_communication::task_engine::thread_func()
{
    task_type task;
    while ( queue_.wait_and_pop( task ) && !force_stop_ )
    {
        task();
    }
}
