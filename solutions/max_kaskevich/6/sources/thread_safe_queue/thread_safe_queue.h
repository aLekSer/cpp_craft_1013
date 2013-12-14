#ifndef _TASK5_6_THREAD_SAFE_QUEUE_H_
#define _TASK5_6_THREAD_SAFE_QUEUE_H_

#include <cstdlib>
#include <memory>
#include <boost/thread.hpp>
#include <queue>

namespace multicast_communication
{

    template< typename T >
    class thread_safe_queue
    {
        std::queue< T > queue_;
        mutable boost::mutex mtx_;
        bool waitable_;
        boost::condition_variable cond_;

        bool pop( T& result );
    public:
        explicit thread_safe_queue();
        ~thread_safe_queue();

        void push( const T& new_element );
        bool wait_and_pop( T& result );
        bool try_pop( T& result );
        
        void enable_wait();
        void disable_wait();

        bool empty() const;
        size_t size() const;
    };

    template< typename T >
    thread_safe_queue< T >::thread_safe_queue() :
        waitable_( true )
    {
    }

    template< typename T >
    thread_safe_queue< T >::~thread_safe_queue()
    {
        disable_wait();
    }

    template< typename T >
    void thread_safe_queue< T >::push( const T&  elem )
    {
        boost::mutex::scoped_lock lock( mtx_ );
        queue_.push( elem );
        lock.unlock();
        cond_.notify_one();
    }

    template< typename T >
    bool thread_safe_queue< T >::pop( T& elem )
    {
        if( queue_.empty() )
        {
            return false;
        }
        elem = queue_.front();
        queue_.pop();
        return true;
    }

    template< typename T >
    bool thread_safe_queue< T >::try_pop( T& elem )
    {
        boost::mutex::scoped_lock lock( mtx_ );
        if( queue_.empty() )
        {
            return false;
        }
        elem = queue_.front();
        queue_.pop();
        return true;
    }

    template< typename T >
    bool thread_safe_queue< T >::wait_and_pop( T& elem )
    {
        if ( !waitable_ )
        {
            return false;
        }
        boost::mutex::scoped_lock lock(mtx_);
        while ( queue_.empty() && waitable_ )
        {
            cond_.wait( lock );
        }
        return waitable_ && pop( elem );
    }

    template< typename T >
    void thread_safe_queue< T >::enable_wait()
    {
        boost::mutex::scoped_lock lock(mtx_);
        waitable_ = true;
        if ( !queue_.empty() )
        {
            lock.unlock();
            cond_.notify_all();
        }
    }

    template< typename T >
    void thread_safe_queue< T >::disable_wait()
    {
        boost::mutex::scoped_lock lock(mtx_);
        if ( waitable_ )
        {
            waitable_ = false;
            lock.unlock();
            cond_.notify_all();            
        }
    }

    template< typename T >
    bool thread_safe_queue< T >::empty() const
    {
        boost::mutex::scoped_lock lock( mtx_ );
        return queue_.empty();
    }

    template< typename T >
    size_t thread_safe_queue< T >::size() const
    {
        boost::mutex::scoped_lock lock( mtx_ );
        return queue_.size();
    }

}

#endif // _TASK5_6_THREAD_SAFE_QUEUE_H_
