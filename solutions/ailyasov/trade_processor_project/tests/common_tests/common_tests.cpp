#include <vector>
#include <string>
#include <set>

#include <boost/thread.hpp>
#include <boost/bind.hpp>

#include "test_registrator.h"

#include "trade_message.h"
#include "quote_message.h"

#include "thread_safe_queue.h"

namespace common
{
    namespace tests_
    { 
        void push_thread(thread_safe_queue<long long int>& q, long long int j)
        {
            long long int start = 10000 * j;
            for(long long int i = start; i < start + 10000; ++i)
            {
                q.push(i);
            }
        }
        void pop_thread(thread_safe_queue<long long int>& q)
        {
            for(int i = 0; i < 10000; ++i)
            {
                long long int a;
                q.wait_and_pop(a);
            }
        }
        void common_tests()
        {
            {
                thread_safe_queue<long long int> q;
                q.push(1L);
                BOOST_CHECK_EQUAL(1, q.size());
            }

            {
                thread_safe_queue<long long int> q;
                q.push(1L);
                q.push(2L);
                q.push(3L);
                long long int a;
                q.pop(a);
                BOOST_CHECK_EQUAL(2, q.size());
            }
            {
                using Set = std::set<long long int>;
                using Iterator = Set::iterator;
                using Result = std::pair<Iterator, bool>;
                thread_safe_queue<long long int> q;
                boost::thread_group t_group;
                for(long long int i = 0; i < 100; ++i)
                {
                    t_group.create_thread(boost::bind(&push_thread, boost::ref(q), i));
                }
                t_group.join_all();
                BOOST_CHECK_EQUAL(1000000, q.size());
                Set s;
                while(!q.empty())
                {
                    long long int a;
                    q.pop(a);
                    Result r = s.insert(a); 
                    BOOST_CHECK_EQUAL(true, r.second);
                }
                BOOST_CHECK_EQUAL(0, q.size());
            }
            {
                thread_safe_queue<long long int> q;
                boost::thread_group t_group;
                for(long long int i = 0; i < 100; ++i)
                {
                    t_group.create_thread(boost::bind(&push_thread, boost::ref(q), i));
                    t_group.create_thread(boost::bind(&pop_thread, boost::ref(q)));
                }
                t_group.join_all();
                BOOST_CHECK_EQUAL(0, q.size());
            }
        }
    }
}
