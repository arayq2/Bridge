/** ======================================================================+
 + Copyright @2023-2025 Arjun Ray
 + Released under MIT License
 + see https://mit-license.org
 +========================================================================*/
#pragma once
#ifndef UTILITY_WAITQUEUE_H
#define UTILITY_WAITQUEUE_H

#include <future>
#include <deque>
#include <thread>
#include <utility>
#include <stdexcept>
#include <mutex>

namespace Utility
{
    /**
     * WaitQueue.
     * A re-entrant mutex with enqueued clients (waiters).
     * The promise at the front is fulfilled for acquire(), and 
     * remains on the queue as a placeholder until release(), at 
     * which point it is dismissed and the successor is fulfilled.
     */
    class WaitQueue
    {
    public:
        WaitQueue() = default;
        // promises still on the queue will set_exception() on their futures
        ~WaitQueue() noexcept = default;
    
        void lock()   { append_( std::this_thread::get_id() ).wait(); }
        void unlock() { remove_( std::this_thread::get_id() ); }

        void acquire() { lock(); }
        void release() { unlock(); }

    private:
        using ThdId   = std::thread::id;
        using Promise = std::promise<void>; // producer of ready signal
        using Future  = std::future<void>;  // consumer of ready signal
        using Pair    = std::pair<ThdId, Promise>;
        using Queue   = std::deque<Pair>; // acquisition order
        using Mutex   = std::mutex;
        using Guard   = std::lock_guard<std::mutex>;

        Mutex       mx_;
        Queue       queue_;
        ThdId       curr_; // current holder of "lock"
        Promise     rpt_;   // non-queue placeholder for re-entrancy
        unsigned    count_; // re-entrancy count
        
        // enable front of queue and mark its id
        void fulfill_( Pair& pr )
        {
            curr_  = pr.first;
            count_ = 1;
            pr.second.set_value();
        }
        
        // re-entrancy is not cheap!
        Future re_enter_()
        {
            rpt_ = Promise();
            ++count_;
            rpt_.set_value();
            return rpt_.get_future();      
        }
        
        // new promise, fulfilled immediately if the only one
        Future append_( ThdId id )
        {
            //SpinLock    _lock(flag_);
            Guard       _guard(mx_);
            
            if ( curr_ == id ) { return re_enter_(); }
            
            queue_.emplace_back( std::make_pair( id, Promise() ) );
            // check empty -> non-empty transition
            if ( queue_.size() == 1 ) { fulfill_( queue_.front() ); }
            return queue_.back().second.get_future();
        }
        
        // unblock placeholder, fulfill next promise if present
        void remove_( ThdId id )
        {
            //SpinLock    _lock(flag_);
            Guard       _guard(mx_);
            
            if ( curr_ != id ) { throw std::runtime_error("release by wrong thread"); }
            
            if ( --count_ == 0 )
            {
                queue_.pop_front();
                if ( queue_.empty() ) { curr_ = ThdId(); }
                else                  { fulfill_( queue_.front() ); }
            }
        }
        
        WaitQueue(WaitQueue const&) = delete;
        WaitQueue& operator=( WaitQueue ) = delete;
    };
    
    class Ticket
    {
        WaitQueue&  wq_;
    public:
        explicit Ticket(WaitQueue& wq) : wq_(wq) { wq.lock(); }
        ~Ticket() noexcept { wq_.unlock(); }
    };
    
} //  namespace Utility   
#endif // UTILITY_WAITQUEUE_H
