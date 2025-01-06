Used with **esp-idf**, **libcurl** needs some paches.

The problem with **Connection already in progress** is caused by the fact that the lwip sockets starts at `LWIP_SOCKET_OFFSET`.

`FD_SET`/`FD_GET` macros take this into account when operating on fd sets: `LWIP_SOCKET_OFFSET` is subtracted from fd number, and the resulting value as used as bit offset in fd set.

In curl (file lib/select.h), the macro `#define VALID_SOCK(s) (((s) >= 0) && ((s) < FD_SETSIZE))` checks the valid socket and fails with esp-idf lwip socket.

This macro has to be changed to:

`#define VALID_SOCK(s) (((s) - LWIP_SOCKET_OFFSET >= 0) && ((s) - LWIP_SOCKET_OFFSET < FD_SETSIZE))`

then everything works as expected.

Only directories **lib** and **include** from curl repository needs to be used.

