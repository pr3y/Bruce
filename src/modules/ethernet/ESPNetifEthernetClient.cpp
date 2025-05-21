#include "ESPNetifEthernetClient.h"
#include "Arduino.h"
#include "esp_netif.h"
#include "lwip/err.h"
#include "lwip/sockets.h"

ESPNetifEthernetClient::ESPNetifEthernetClient() {}

ESPNetifEthernetClient::~ESPNetifEthernetClient() {}

#define log_e printf
#define log_i printf

int ESPNetifEthernetClient::connect(IPAddress ip, uint16_t port, int32_t timeout_ms) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        // log_e("socket: %d", errno);
        return -1;
    }
    fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFL, 0) | O_NONBLOCK);
    uint32_t ip_addr = ip;
    struct sockaddr_in serveraddr;
    memset((char *)&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    memcpy((void *)&serveraddr.sin_addr.s_addr, (const void *)(&ip_addr), 4);
    serveraddr.sin_port = htons(port);
    fd_set fdset;
    struct timeval tv;
    FD_ZERO(&fdset);
    FD_SET(sockfd, &fdset);
    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;

    int res = lwip_connect(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));

    if (res < 0 && errno != EINPROGRESS) {
        log_e("connect on fd %d, errno: %d, \"%s\"", sockfd, errno, strerror(errno));
        close(sockfd);
        return -1; // Error
    }

    res = select(sockfd + 1, nullptr, &fdset, nullptr, timeout_ms < 0 ? nullptr : &tv);
    if (res < 0) {
        log_e("select on fd %d, errno: %d, \"%s\"", sockfd, errno, strerror(errno));
        close(sockfd);
        return -1;
    } else if (res == 0) {
        log_i("select returned due to timeout %d ms for fd %d", timeout_ms, sockfd);
        close(sockfd);
        return -1;
    } else {
        int sockerr;
        socklen_t len = (socklen_t)sizeof(int);
        res = getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &sockerr, &len);

        if (res < 0) {
            log_e("getsockopt on fd %d, errno: %d, \"%s\"", sockfd, errno, strerror(errno));
            close(sockfd);
            return -1;
        }

        if (sockerr != 0) {
            log_e("socket error on fd %d, errno: %d, \"%s\"", sockfd, sockerr, strerror(sockerr));
            close(sockfd);
            return -1;
        }
    }

    // #define ROE_WIFICLIENT(x, msg)                                                                               \
//     {                                                                                                        \
//         if (((x) < 0)) {                                                                                     \
//             log_e(                                                                                           \
//                 "Setsockopt '" msg "'' on fd %d failed. errno: %d, \"%s\"", sockfd, errno, strerror(errno)   \
//             );                                                                                               \
//             return 0;                                                                                        \
//         }                                                                                                    \
//     }
    // ROE_WIFICLIENT(setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv)), "SO_SNDTIMEO");
    // ROE_WIFICLIENT(setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)), "SO_RCVTIMEO");

    // // These are also set in WiFiClientSecure, should be set here too?
    // // ROE_WIFICLIENT(setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, &enable, sizeof(enable)),"TCP_NODELAY");
    // // ROE_WIFICLIENT (setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &enable,
    // sizeof(enable)),"SO_KEEPALIVE");

    fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFL, 0) & (~O_NONBLOCK));
    // clientSocketHandle.reset(new WiFiClientSocketHandle(sockfd));
    // _rxBuffer.reset(new WiFiClientRxBuffer(sockfd));

    // _connected = true;

    return sockfd;
}

void ESPNetifEthernetClient::client_close(int sockfd) {
    if (sockfd >= 0) {
        shutdown(sockfd, SHUT_RDWR);
        close(sockfd);
    }
}

bool ESPNetifEthernetClient::connected(int sockfd) {
    bool _connected = false;

    uint8_t dummy;
    int res = recv(sockfd, &dummy, 0, MSG_DONTWAIT);

    // recv only sets errno if res is <= 0
    if (res <= 0) {
        switch (errno) {
            case EWOULDBLOCK:
            case ENOENT: // caused by vfs
                _connected = true;
                break;
            case ENOTCONN:
            case EPIPE:
            case ECONNRESET:
            case ECONNREFUSED:
            case ECONNABORTED:
                _connected = false;
                log_d("Disconnected: RES: %d, ERR: %d", res, errno);
                break;
            default:
                log_i("Unexpected: RES: %d, ERR: %d", res, errno);
                _connected = true;
                break;
        }
    } else {
        _connected = true;
    }

    return _connected;
}
