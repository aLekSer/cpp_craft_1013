#ifndef _TASK5_6_THREAD_SAFE_QUEUE_H_
#define _TASK5_6_THREAD_SAFE_QUEUE_H_

#include <cstdlib>
#include <memory>
#include <boost/thread.hpp>

namespace multicast_communication
{

    template< typename T >
    class thread_safe_queue
    {
        struct node
        {
            std::shared_ptr< node > next;
            const T data;
            node( const T& t ) :
                data( t )
            {

            }
        };

        std::shared_ptr< node > first_;
        std::shared_ptr< node > last_;
        size_t size_;
        boost::mutex mtx_;
        boost::mutex wait_not_empty_;
        bool disable_wait_;

    public:
        explicit thread_safe_queue();
        ~thread_safe_queue();

        void push( const T& new_element );
        bool pop( T& result );
        bool wait_pop( T& result );
        void enable_wait();
        void disable_wait();

        bool empty() const;
        size_t size() const;
    };

    template< typename T >
    thread_safe_queue< T >::thread_safe_queue() :
        first_( nullptr ),
        last_( nullptr ),
        size_( 0 ),
        disable_wait_(false)
    {
         wait_not_empty_.lock();
    }

    template< typename T >
    thread_safe_queue< T >::~thread_safe_queue()
    {
        disable_wait();
    }

    template< typename T >
    void thread_safe_queue< T >::push( const T&  elem )
    {
        boost::mutex::scoped_lock lock(mtx_);
        if ( empty() )
        {
            wait_not_empty_.unlock();
        }
        std::shared_ptr< node > push_node( new node( elem ) );
        if ( last_ )
        {
            last_->next = push_node;
        }
        else
        {
            first_ = push_node;
        }
        last_ = push_node;
        ++size_;   

    }

    template< typename T >
    bool thread_safe_queue< T >::pop( T& elem )
    {
        boost::mutex::scoped_lock lock(mtx_);
        if( first_ )
        {
            elem = first_->data;
            first_ = first_->next;
            if( !first_ )
            {
                last_.reset();
            }
            --size_;

            wait_not_empty_.try_lock();            
            if ( !empty() )
            {
                wait_not_empty_.unlock();
            }
            return true;
        }
        return false;
    }

    template< typename T >
    bool thread_safe_queue< T >::wait_pop( T& elem )
    {
        if( disable_wait_  )
        {
            return false;
        }
        wait_not_empty_.lock();
        return !disable_wait_ && pop( elem );
    }

    template< typename T >
    void thread_safe_queue< T >::enable_wait()
    {
        disable_wait_ = false;
        boost::mutex::scoped_lock lock(mtx_);
        if ( empty() )
        {
            wait_not_empty_.try_lock();
        }
    }

    template< typename T >
    void thread_safe_queue< T >::disable_wait()
    {
        if ( !disable_wait_ )
        {
            disable_wait_ = true;
            wait_not_empty_.unlock();
        }
    }

    template< typename T >
    bool thread_safe_queue< T >::empty() const
    {
        return !size_;
    }

    template< typename T >
    size_t thread_safe_queue< T >::size() const
    {
        return size_;
    }

}

#endif // _TASK5_6_THREAD_SAFE_QUEUE_H_
