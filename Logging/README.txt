
0.  Overview.

This implements the two-part design of a generic front-end and a replaceable back-end. (See LoggingWithMacros.txt.)

    a.  Every back-end is a wrapper around an actual logging implementation (typically a library, such as log4cxx).

    b.  The wrapper will implement a generic internal interface, which is used by the front end to communicate logging directives. (Note: this is a C++ variation of the Dependency Inversion Principle.)


1.  The application interface.
(File: Logging.h)

User code should #include this file only. It #include-s the front-end interface definition (Log.h). See "Using the macros" below.

    a.  This file defines a number of macros. Using these macros is not mandatory, but highly recommended.

    b.  There are two styles of macros, corresponding to how the message portion of the log entry can be assembled:
        1.  Stream insertion style, using operator<<. The std::ostream object accumulating the insertions is encapsulated in the macros.
        2.  Printf style formatting, using a format string and arguments. The char buffer (of size 2048) is encapsulated in the macros.

    c.  The main purpose of using macros is to leverage short-circuiting, so that relatively expensive operations, such as message assembly and I/O, are not activated unless needed. (This is classic C++ philosophy: pay for only what you need/use.)

    d.  The macros wrap two calls into the front-end interface:
        1.  A call to check whether a severity level (priority) is enabled for logging. If not, nothing further happens. This call is "cheap", by design and intent.
        2.  A call to commit the log entry particulars to the back-end implementation. This call is "expensive", as it involves both the assembly of the message portion for an input argument, and the I/O in the back end.

    e.  These two calls can be invoked directly in special circumstances. A macro is provided for the case where repeated possible commits at the same severity level can share the result of a single check.


2.  The front-end interface.
(File: Log.h)

    a.  This declares two main classes and supporting helpers. The two main classes are
        1.  Logger: A "context" object to carry state for the back-end implementation. It is needed for the "check enabled" API call. User code is responsible for instantiating this object and maintaining its lifecycle. In contrast to the log4X family, heap allocation is not necessary.
        2   Log: A class aggregating a number of definitions used in common by the set of static functions implementing the front-end interface. No instantiation is needed (the class has no non-static members).

    b.  There is a "default" back-end implementation built in, as a "fail-soft" mechanism, that writes log entries to standard error. It is not ordinarily invoked.


3.  The back-end interface.
(File: LogImpl.h)

This file is relevant to back-end wrapper code only. This defines the interface that the front-end expects the back-end to implement.

    a.  Calls to initialize and finalize the back-end.

    b.  Calls to acquire and release back-end proxies for state information, which will be opaque pointers (void *).

    c.  Calls to determine whether a severity level is enabled for logging.

    d.  Calls to commit a log entry to the back-end.


4.  The front-end implementation.
(File: Logging.cpp)

This file has the (mainly administrative) details of translating between the front-end interface and the back-end interface. These are the implementations of items declared in Log.h:

    a.  The static functions of the Utility::Log class.
    b.  The constructors of the Utility::Logger class.


5.  Back-end implementations.
(File: LogImplStub.cpp, Log4CxxImpl.cpp, etc.)

These implement the interface declared in LogImpl.h:

    a.  Log4CxxImpl.cpp: This file implements the wrapper around the log4cxx library.

    b.  LogImplStub.cpp: This is a "no-op" implementation that causes the built-in implementation to be invoked. See "Default fail-safe logging" below.


6.  Using the system: logger setup.

The back-end implementation of a logger is represented in the front-end and user code by an opaque pointer (void *). The pointer will be acquired from the back-end by the constructor of the Utility::Logger object. This Utility::Logger object (or a pointer to it) is the mandatory first argument of the level-specific logging macros.

Thus, whereas using log4cxx directly the logger acquisition looked like this:

    static log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger("categoryName");

Now, it looks like this:

    static Utility::Logger  logger("categoryName");

These are effectively equivalent if log4cxx is used as the back-end.

Important note: These logger instances are not needed if only the fallback default logging implementation is used. In other words, no (explicit) setup is necessary in order to use the default logging implementation.


7.  Using the macros.

The macros come in two styles, depending on whether stream insertion or printf-style formatting is used to assemble the message text. There are separate macros for each severity level (which saves on having to provide the level explicitly as an argument), all of which expand to the same internal macro.

Using the insertion style as an example, a logging statement would look like this:

    LOG_STRM_INFO(logger, "string1" << object1 << "string2" << etc);

Internally, this expands to another macro

    LOG_STRM(logger, Utility::Log::Level::INFO, "string1" << object1 << "string2" << etc);

The LOG_STRM() macro expands to a sequence of statements. First, there is a check whether the level is "active". If not, the macro ends at once.

    Utility::Log::Token _token = Utility::Log::is_enabled( logger, level );

This _token object will be passed to the back-end if needed. It also implements operator bool(), and thus can be used directly in boolean test contexts. This is how the macro implements short-circuiting logic: only if the level is active (i.e. _token evaluates to true) will the message actually be assembled and passed to the back-end.

    if ( _token ) { /* front end logging implementation */
        std::ostringstream  _oss;
        _oss << "string1" << object1 << "string2" << etc;
        Utility::Log::commit( _token, LOCATION(), _oss.str().c_str() );
    }

The LOCATION() macro carries the details of the source code context (function name, file name, line number) from which the original macro LOG_STRM_INFO() was invoked. Note that this results in the actual source location being logged, precisely because only macros have been used up to this point. Had there been a function call anywhere in the expansion chain, the original location information would have been lost.

Note that a Utility::Log::Token object, if acquired explicitly, can be reused. A separate macro is provided for this case, and looks like this:

    Utility::Log::Token _debug(Utility::Log::is_enabled( logger, Utility::Log::Level::DEBUG ));
    // and later...
    LOG_STRM_IF(_debug, "string1" << object1 << "string2" << etc);
    // and later...
    LOG_STRM_IF(_debug, "strin31" << object2 << "string4" << etc);

This will probably be most useful for DEBUG level logging, which if enabled is likely to be invoked multiple times.


8.  Default fail-soft logging.

Passing nullptr instead of a Utility::Logger reference/pointer to the level-specific macros will log to a fallback destination (by default, standard error), bypassing the back-end entirely. This is the built-in fail-soft implementation. 

Note that using nullptr as the "logger reference" does not require a logger instance to be defined (or even to exist).


