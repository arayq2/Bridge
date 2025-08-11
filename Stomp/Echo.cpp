/** ======================================================================+
 + Copyright @2023-2025 Arjun Ray
 + Released under MIT License
 + see https://mit-license.org
 +========================================================================*/
#include "Servlet.h"
#include "Notice.h"

#include <iostream>

    struct DoNothing
    {
        void on_init( Stomp::Publisher& ) {}

        void on_start( Stomp::Publisher& ) {}

        void on_error( std::string&, std::string const&, Stomp::Publisher& ) {}

    };

    class Receiver
    : public DoNothing
    {
    public:
        ~Receiver() noexcept = default;
        explicit
        Receiver(Utility::Notice& notice)
        : notice_(notice)
        {}

        void on_message( std::string const& message, Stomp::EndPoint const& endpoint, Stomp::Publisher& pub )
        {
            std::cout << "[(" << (endpoint.isQ_ ? "Q)" : "T)") 
                      << endpoint.dest_ << "]\n"
                      << message << std::endl;            
            pub.publish( message );
        }

        void stop() { notice_.deliver(); }

    private:
        Utility::Notice&        notice_;
    };

    int main( int ac, char* av[] )
    {
        if ( ac < 4 )
        {
            std::cerr << "Usage: " << av[0] << " t|q <source> <target>" << std::endl;
            return 1;            
        }

        bool                        _isq(av[1][0] == 'q');

        Stomp::StompAgent           _agent(true);
        Utility::Notice             _notice;
        Receiver                    _receiver(_notice);
        Stomp::Servlet<Receiver>    _servlet(_agent, _receiver, {av[3], _isq});

        _servlet.subscribe( {av[2], _isq} );
        _notice.wait();
        return 0;
    }
