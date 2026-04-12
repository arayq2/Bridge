/** ======================================================================+
 + Copyright @2019 Arjun Ray
 + Released under MIT License
 + see https://mit-license.org
 +========================================================================*/
 
#include "utility/Lockable.h"
#include "utility/Scheduler.h"

#include <iostream>

Utility::Event  stop;

    class Test
    {
        struct Handler
        {
            using item_type = unsigned int;
            Test*   tp_;
            ~Handler() noexcept = default;
            Handler(Test* tp) : tp_(tp) {}
            void operator()( item_type& item ) { tp_->process_item( item ); }
        };
        using Timer   = Utility::Scheduler<Handler>;
        using TimeOut = typename Timer::TimeOut;

    public:
        ~Test() noexcept = default;
        Test(long time1, long time2);

        void set_all();
        void process_item( unsigned int& );
        void publish( unsigned int const& );
        void time_left();
    private:
        Timer       timer_;
        TimeOut     t1_;
        TimeOut     t2_;
    };

    Test::Test(long time1, long time2)
    : timer_(Handler(this))
    , t1_(timer_, time1)
    , t2_(timer_, time2)
    {}

    void Test::set_all()
    {
        unsigned int    _t1(1);
        unsigned int    _t2(2);
        t1_.set( _t1 );
        t2_.set( _t2 );
    }

    void Test::process_item( unsigned int& item )
    {
        switch ( item )
        {
        case 1 : publish( item ); time_left(); break;
        case 2 : publish( item ); stop.signal();
        default: break;
        }
    }

    void Test::time_left()
    {
        std::cout << "Time left on t2: " << t2_.timeleft() << std::endl;
    }

    void Test::publish( unsigned int const& item )
    {
        std::cout << "Received item [" << item << "]" << std::endl;
    }

int main()
{
    Test    _test(2000L, 4000L);
    _test.set_all();
    stop.wait();

    return 0;
}
