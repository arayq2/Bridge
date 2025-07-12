
STOMP (Streaming Text Oriented Messaging Protocol).

1.  An incomplete implementation of the STOMP v.1.1 protocol.

    a.  Heartbeats are not implemented.


2.  Basic usage.

    a.  Sending is trivial. See Sender.cpp. Note that sending is thread safe. Access to the I/O primitive is serialized.

    b.  Receiving can be either synchronous (not advised for anything beyond the simplest cases), or aysnchronous. See ReceiverSync.cpp and ReceiverAsync.cpp. Callback handlers in the asynchronous case should do absolute minimum work, and pass data to dedicated threads, as they are called in the (single) internal dispatching thread.
    

3.  Fine grained reception from multiple sources.

See Feeder.h. It uses a local queue to offload messages from the Stomp dispatch thread. Subscriptions are "tagged" with bespoke information that is echoed in the callback from the local queue.


4.  Servlet style interface.

See Servlet.h. This encapsulates the connection setup and calls the provided "servlet" callback synchronously along with an interface to the send side.
