/* lib/curl_config.h.  Generated from curl_config.h.in by configure.  */
/* lib/curl_config.h.in.  Generated from configure.ac by autoheader.  */

/* Location of default ca bundle */
#define CURL_CA_BUNDLE "/etc/ssl/certs/ca-certificates.crt"

/* define "1" to use built in CA store of SSL library */
/* #undef CURL_CA_FALLBACK */

/* Location of default ca path */
/* #undef CURL_CA_PATH */

/* to disable cookies support */
/* #undef CURL_DISABLE_COOKIES */

/* to disable cryptographic authentication */
/* #undef CURL_DISABLE_CRYPTO_AUTH */

/* to disable DICT */
/* #undef CURL_DISABLE_DICT */

/* to disable FILE */
/* #undef CURL_DISABLE_FILE */

/* to disable FTP */
/* #undef CURL_DISABLE_FTP */

/* to disable Gopher */
/* #undef CURL_DISABLE_GOPHER */

/* to disable HTTP */
/* #undef CURL_DISABLE_HTTP */

/* to disable IMAP */
/* #undef CURL_DISABLE_IMAP */

/* to disable LDAP */
#define CURL_DISABLE_LDAP 1

/* to disable LDAPS */
#define CURL_DISABLE_LDAPS 1

/* to disable NTLM */
#define CURL_DISABLE_NTLM 1

/* to disable --libcurl C code generation option */
/* #undef CURL_DISABLE_LIBCURL_OPTION */

/* to disable POP3 */
/* #undef CURL_DISABLE_POP3 */

/* to disable proxies */
/* #undef CURL_DISABLE_PROXY */

/* to disable RTSP */
/* #undef CURL_DISABLE_RTSP */

/* to disable SMB/CIFS */
/* #undef CURL_DISABLE_SMB */

/* to disable SMTP */
/* #undef CURL_DISABLE_SMTP */

/* to disable TELNET */
/* #undef CURL_DISABLE_TELNET */

/* to disable TFTP */
/* #undef CURL_DISABLE_TFTP */

/* to disable TLS-SRP authentication */
/* #undef CURL_DISABLE_TLS_SRP */

/* to disable verbose strings */
/* #undef CURL_DISABLE_VERBOSE_STRINGS */

/* Definition to make a library symbol externally visible. */
#define CURL_EXTERN_SYMBOL __attribute__ ((__visibility__ ("default")))

/* your Entropy Gathering Daemon socket pathname */
/* #undef EGD_SOCKET */

/* Define if you want to enable IPv6 support */
#define ENABLE_IPV6 1

/* Define to the type of arg 2 for gethostname. */
#define GETHOSTNAME_TYPE_ARG2 size_t

/* Define to the type qualifier of arg 1 for getnameinfo. */
#define GETNAMEINFO_QUAL_ARG1 const

/* Define to the type of arg 1 for getnameinfo. */
#define GETNAMEINFO_TYPE_ARG1 struct sockaddr *

/* Define to the type of arg 2 for getnameinfo. */
#define GETNAMEINFO_TYPE_ARG2 socklen_t

/* Define to the type of args 4 and 6 for getnameinfo. */
#define GETNAMEINFO_TYPE_ARG46 socklen_t

/* Define to the type of arg 7 for getnameinfo. */
#define GETNAMEINFO_TYPE_ARG7 int

/* Specifies the number of arguments to getservbyport_r */
#define GETSERVBYPORT_R_ARGS 6

/* Specifies the size of the buffer to pass to getservbyport_r */
#define GETSERVBYPORT_R_BUFSIZE 4096

/* Define to 1 if you have the alarm function. */
#define HAVE_ALARM 1

/* Define to 1 if you have the <alloca.h> header file. */
#define HAVE_ALLOCA_H 1

/* Define to 1 if you have the <arpa/inet.h> header file. */
#define HAVE_ARPA_INET_H 1

/* Define to 1 if you have the <arpa/tftp.h> header file. */
#define HAVE_ARPA_TFTP_H 1

/* Define to 1 if you have the <assert.h> header file. */
#define HAVE_ASSERT_H 1

/* Define to 1 if you have the basename function. */
//#define HAVE_BASENAME 1

/* Define to 1 if bool is an available type. */
#define HAVE_BOOL_T 1

/* Define to 1 if using BoringSSL. */
/* #undef HAVE_BORINGSSL */

/* Define to 1 if you have the clock_gettime function and monotonic timer. */
//#define HAVE_CLOCK_GETTIME_MONOTONIC 1

/* Define to 1 if you have the closesocket function. */
/* #undef HAVE_CLOSESOCKET */

/* Define to 1 if you have the CloseSocket camel case function. */
/* #undef HAVE_CLOSESOCKET_CAMEL */

/* Define to 1 if you have the connect function. */
#define HAVE_CONNECT 1

/* Define to 1 if you have the <crypto.h> header file. */
/* #undef HAVE_CRYPTO_H */

/* Define to 1 if you have the `CyaSSL_CTX_UseSupportedCurve' function. */
/* #undef HAVE_CYASSL_CTX_USESUPPORTEDCURVE */

/* Define to 1 if you have the <cyassl/error-ssl.h> header file. */
/* #undef HAVE_CYASSL_ERROR_SSL_H */

/* Define to 1 if you have the `CyaSSL_get_peer_certificate' function. */
/* #undef HAVE_CYASSL_GET_PEER_CERTIFICATE */

/* Define to 1 if you have the <cyassl/options.h> header file. */
/* #undef HAVE_CYASSL_OPTIONS_H */

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Define to 1 if you have the `ENGINE_cleanup' function. */
#define HAVE_ENGINE_CLEANUP 1

/* Define to 1 if you have the `ENGINE_load_builtin_engines' function. */
#define HAVE_ENGINE_LOAD_BUILTIN_ENGINES 1

/* Define to 1 if you have the <errno.h> header file. */
#define HAVE_ERRNO_H 1

/* Define to 1 if you have the <err.h> header file. */
/* #undef HAVE_ERR_H */

/* Define to 1 if you have the fcntl function. */
#define HAVE_FCNTL 1

/* Define to 1 if you have the <fcntl.h> header file. */
#define HAVE_FCNTL_H 1

/* Define to 1 if you have a working fcntl O_NONBLOCK function. */
#define HAVE_FCNTL_O_NONBLOCK 1

/* Define to 1 if you have the fdopen function. */
#define HAVE_FDOPEN 1

/* Define to 1 if you have the `fork' function. */
#define HAVE_FORK 1

/* Define to 1 if you have the freeaddrinfo function. */
#define HAVE_FREEADDRINFO 1

/* Define to 1 if you have the freeifaddrs function. */
#define HAVE_FREEIFADDRS 1

/* Define to 1 if you have the fsetxattr function. */
#define HAVE_FSETXATTR 1

/* fsetxattr() takes 5 args */
#define HAVE_FSETXATTR_5 1

/* fsetxattr() takes 6 args */
/* #undef HAVE_FSETXATTR_6 */

/* Define to 1 if you have the ftruncate function. */
#define HAVE_FTRUNCATE 1

/* Define to 1 if you have the gai_strerror function. */
#define HAVE_GAI_STRERROR 1

/* Define to 1 if you have a working getaddrinfo function. */
#define HAVE_GETADDRINFO 1

/* Define to 1 if the getaddrinfo function is threadsafe. */
#define HAVE_GETADDRINFO_THREADSAFE 1

/* Define to 1 if you have the `geteuid' function. */
//#define HAVE_GETEUID 1

/* Define to 1 if you have the gethostbyaddr function. */
#define HAVE_GETHOSTBYADDR 1

/* Define to 1 if you have the gethostbyaddr_r function. */
#define HAVE_GETHOSTBYADDR_R 1

/* gethostbyaddr_r() takes 5 args */
/* #undef HAVE_GETHOSTBYADDR_R_5 */

/* gethostbyaddr_r() takes 7 args */
/* #undef HAVE_GETHOSTBYADDR_R_7 */

/* gethostbyaddr_r() takes 8 args */
#define HAVE_GETHOSTBYADDR_R_8 1

/* Define to 1 if you have the gethostbyname function. */
#define HAVE_GETHOSTBYNAME 1

/* Define to 1 if you have the gethostbyname_r function. */
#define HAVE_GETHOSTBYNAME_R 1

/* gethostbyname_r() takes 3 args */
/* #undef HAVE_GETHOSTBYNAME_R_3 */

/* gethostbyname_r() takes 5 args */
/* #undef HAVE_GETHOSTBYNAME_R_5 */

/* gethostbyname_r() takes 6 args */
#define HAVE_GETHOSTBYNAME_R_6 1

/* Define to 1 if you have the gethostname function. */
//#define HAVE_GETHOSTNAME 1

/* Define to 1 if you have a working getifaddrs function. */
//#define HAVE_GETIFADDRS 1

/* Define to 1 if you have the getnameinfo function. */
#define HAVE_GETNAMEINFO 1

/* Define to 1 if you have the `getpass_r' function. */
/* #undef HAVE_GETPASS_R */

/* Define to 1 if you have the `getppid' function. */
#define HAVE_GETPPID 1

/* Define to 1 if you have the `getprotobyname' function. */
#define HAVE_GETPROTOBYNAME 1

/* Define to 1 if you have the `getpwuid' function. */
#define HAVE_GETPWUID 1

/* Define to 1 if you have the `getpwuid_r' function. */
#define HAVE_GETPWUID_R 1

/* Define to 1 if you have the `getrlimit' function. */
#define HAVE_GETRLIMIT 1

/* Define to 1 if you have the getservbyport_r function. */
#define HAVE_GETSERVBYPORT_R 1

/* Define to 1 if you have the `gettimeofday' function. */
#define HAVE_GETTIMEOFDAY 1

/* Define to 1 if you have a working glibc-style strerror_r function. */
/* #undef HAVE_GLIBC_STRERROR_R */

/* Define to 1 if you have a working gmtime_r function. */
#define HAVE_GMTIME_R 1

/* Define to 1 if you have the `gnutls_alpn_set_protocols' function. */
/* #undef HAVE_GNUTLS_ALPN_SET_PROTOCOLS */

/* Define to 1 if you have the `gnutls_certificate_set_x509_key_file2'
   function. */
/* #undef HAVE_GNUTLS_CERTIFICATE_SET_X509_KEY_FILE2 */

/* Define to 1 if you have the `gnutls_ocsp_req_init' function. */
/* #undef HAVE_GNUTLS_OCSP_REQ_INIT */

/* if you have the function gnutls_srp_verifier */
/* #undef HAVE_GNUTLS_SRP */

/* if you have GSS-API libraries */
/* #undef HAVE_GSSAPI */

/* Define to 1 if you have the <gssapi/gssapi_generic.h> header file. */
/* #undef HAVE_GSSAPI_GSSAPI_GENERIC_H */

/* Define to 1 if you have the <gssapi/gssapi.h> header file. */
/* #undef HAVE_GSSAPI_GSSAPI_H */

/* Define to 1 if you have the <gssapi/gssapi_krb5.h> header file. */
/* #undef HAVE_GSSAPI_GSSAPI_KRB5_H */

/* if you have GNU GSS */
/* #undef HAVE_GSSGNU */

/* if you have Heimdal */
/* #undef HAVE_GSSHEIMDAL */

/* if you have MIT Kerberos */
/* #undef HAVE_GSSMIT */

/* Define to 1 if you have the <idn2.h> header file. */
/* #undef HAVE_IDN2_H */

/* Define to 1 if you have the <ifaddrs.h> header file. */
//#define HAVE_IFADDRS_H 1

/* Define to 1 if you have the `if_nametoindex' function. */
#define HAVE_IF_NAMETOINDEX 1

/* Define to 1 if you have the `inet_addr' function. */
#define HAVE_INET_ADDR 1

/* Define to 1 if you have the inet_ntoa_r function. */
/* #undef HAVE_INET_NTOA_R */

/* inet_ntoa_r() takes 2 args */
/* #undef HAVE_INET_NTOA_R_2 */

/* inet_ntoa_r() takes 3 args */
/* #undef HAVE_INET_NTOA_R_3 */

/* Define to 1 if you have a IPv6 capable working inet_ntop function. */
#define HAVE_INET_NTOP 1

/* Define to 1 if you have a IPv6 capable working inet_pton function. */
#define HAVE_INET_PTON 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the ioctl function. */
#define HAVE_IOCTL 1

/* Define to 1 if you have the ioctlsocket function. */
/* #undef HAVE_IOCTLSOCKET */

/* Define to 1 if you have the IoctlSocket camel case function. */
/* #undef HAVE_IOCTLSOCKET_CAMEL */

/* Define to 1 if you have a working IoctlSocket camel case FIONBIO function.
   */
/* #undef HAVE_IOCTLSOCKET_CAMEL_FIONBIO */

/* Define to 1 if you have a working ioctlsocket FIONBIO function. */
/* #undef HAVE_IOCTLSOCKET_FIONBIO */

/* Define to 1 if you have a working ioctl FIONBIO function. */
#define HAVE_IOCTL_FIONBIO 1

/* Define to 1 if you have a working ioctl SIOCGIFADDR function. */
//#define HAVE_IOCTL_SIOCGIFADDR 1

/* Define to 1 if you have the <io.h> header file. */
/* #undef HAVE_IO_H */

/* Define to 1 if you have the lber.h header file. */
/* #undef HAVE_LBER_H */

/* Define to 1 if you have the ldapssl.h header file. */
/* #undef HAVE_LDAPSSL_H */

/* Define to 1 if you have the ldap.h header file. */
/* #undef HAVE_LDAP_H */

/* Define to 1 if you have the `ldap_init_fd' function. */
/* #undef HAVE_LDAP_INIT_FD */

/* Use LDAPS implementation */
#define HAVE_LDAP_SSL 1

/* Define to 1 if you have the ldap_ssl.h header file. */
/* #undef HAVE_LDAP_SSL_H */

/* Define to 1 if you have the `ldap_url_parse' function. */
/* #undef HAVE_LDAP_URL_PARSE */

/* Define to 1 if you have the <libgen.h> header file. */
#define HAVE_LIBGEN_H 1

/* Define to 1 if you have the `idn2' library (-lidn2). */
/* #undef HAVE_LIBIDN2 */

/* Define to 1 if using libressl. */
/* #undef HAVE_LIBRESSL */

/* Define to 1 if you have the <librtmp/rtmp.h> header file. */
/* #undef HAVE_LIBRTMP_RTMP_H */

/* Define to 1 if you have the `ssh2' library (-lssh2). */
/* #undef HAVE_LIBSSH2 */

/* Define to 1 if you have the <libssh2.h> header file. */
/* #undef HAVE_LIBSSH2_H */

/* Define to 1 if you have the `ssl' library (-lssl). */
//#define HAVE_LIBSSL 1

/* if zlib is available */
//#define HAVE_LIBZ 1

/* Define to 1 if you have the <limits.h> header file. */
#define HAVE_LIMITS_H 1

/* if your compiler supports LL */
#define HAVE_LL 1

/* Define to 1 if you have the <locale.h> header file. */
#define HAVE_LOCALE_H 1

/* Define to 1 if you have a working localtime_r function. */
#define HAVE_LOCALTIME_R 1

/* Define to 1 if the compiler supports the 'long long' data type. */
#define HAVE_LONGLONG 1

/* Define to 1 if you have the malloc.h header file. */
#define HAVE_MALLOC_H 1

/* Define to 1 if you have the memory.h header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the memrchr function or macro. */
/* #undef HAVE_MEMRCHR */

/* Define to 1 if you have the MSG_NOSIGNAL flag. */
//#define HAVE_MSG_NOSIGNAL 1

/* Define to 1 if you have the <netdb.h> header file. */
#define HAVE_NETDB_H 1

/* Define to 1 if you have the <netinet/in.h> header file. */
#define HAVE_NETINET_IN_H 1

/* Define to 1 if you have the <netinet/tcp.h> header file. */
//#define HAVE_NETINET_TCP_H 1

/* Define to 1 if you have the <net/if.h> header file. */
//#define HAVE_NET_IF_H 1

/* Define to 1 if you have the <nghttp2/nghttp2.h> header file. */
/* #undef HAVE_NGHTTP2_NGHTTP2_H */

/* Define to 1 if NI_WITHSCOPEID exists and works. */
/* #undef HAVE_NI_WITHSCOPEID */

/* if you have an old MIT Kerberos version, lacking GSS_C_NT_HOSTBASED_SERVICE
   */
/* #undef HAVE_OLD_GSSMIT */

/* Define to 1 if you have the <openssl/crypto.h> header file. */
#define HAVE_OPENSSL_CRYPTO_H 1

/* Define to 1 if you have the <openssl/engine.h> header file. */
#define HAVE_OPENSSL_ENGINE_H 1

/* Define to 1 if you have the <openssl/err.h> header file. */
#define HAVE_OPENSSL_ERR_H 1

/* Define to 1 if you have the <openssl/pem.h> header file. */
#define HAVE_OPENSSL_PEM_H 1

/* Define to 1 if you have the <openssl/pkcs12.h> header file. */
#define HAVE_OPENSSL_PKCS12_H 1

/* Define to 1 if you have the <openssl/rsa.h> header file. */
#define HAVE_OPENSSL_RSA_H 1

/* if you have the function SRP_Calc_client_key */
#define HAVE_OPENSSL_SRP 1

/* Define to 1 if you have the <openssl/ssl.h> header file. */
#define HAVE_OPENSSL_SSL_H 1

/* Define to 1 if you have the <openssl/x509.h> header file. */
#define HAVE_OPENSSL_X509_H 1

/* Define to 1 if you have the <pem.h> header file. */
/* #undef HAVE_PEM_H */

/* Define to 1 if you have the `perror' function. */
#define HAVE_PERROR 1

/* Define to 1 if you have the `pipe' function. */
#define HAVE_PIPE 1

/* Define to 1 if you have a working poll function. */
//#define HAVE_POLL 1

/* If you have a fine poll */
//#define HAVE_POLL_FINE 1

/* Define to 1 if you have the <poll.h> header file. */
//#define HAVE_POLL_H 1

/* Define to 1 if you have a working POSIX-style strerror_r function. */
#define HAVE_POSIX_STRERROR_R 1

/* if you have <pthread.h> */
/* #undef HAVE_PTHREAD_H */

/* Define to 1 if you have the <pwd.h> header file. */
#define HAVE_PWD_H 1

/* Define to 1 if you have the `RAND_egd' function. */
#define HAVE_RAND_EGD 1

/* Define to 1 if you have the recv function. */
#define HAVE_RECV 1

/* Define to 1 if you have the <rsa.h> header file. */
/* #undef HAVE_RSA_H */

/* Define to 1 if you have the select function. */
#define HAVE_SELECT 1

/* Define to 1 if you have the send function. */
#define HAVE_SEND 1

/* Define to 1 if you have the <setjmp.h> header file. */
#define HAVE_SETJMP_H 1

/* Define to 1 if you have the `setlocale' function. */
#define HAVE_SETLOCALE 1

/* Define to 1 if you have the `setmode' function. */
/* #undef HAVE_SETMODE */

/* Define to 1 if you have the `setrlimit' function. */
#define HAVE_SETRLIMIT 1

/* Define to 1 if you have the setsockopt function. */
#define HAVE_SETSOCKOPT 1

/* Define to 1 if you have a working setsockopt SO_NONBLOCK function. */
/* #undef HAVE_SETSOCKOPT_SO_NONBLOCK */

/* Define to 1 if you have the <sgtty.h> header file. */
#define HAVE_SGTTY_H 1

/* Define to 1 if you have the sigaction function. */
#define HAVE_SIGACTION 1

/* Define to 1 if you have the siginterrupt function. */
#define HAVE_SIGINTERRUPT 1

/* Define to 1 if you have the signal function. */
//#define HAVE_SIGNAL 1

/* Define to 1 if you have the <signal.h> header file. */
#define HAVE_SIGNAL_H 1

/* Define to 1 if you have the sigsetjmp function or macro. */
//#define HAVE_SIGSETJMP 1

/* Define to 1 if sig_atomic_t is an available typedef. */
#define HAVE_SIG_ATOMIC_T 1

/* Define to 1 if sig_atomic_t is already defined as volatile. */
/* #undef HAVE_SIG_ATOMIC_T_VOLATILE */

/* Define to 1 if struct sockaddr_in6 has the sin6_scope_id member */
#define HAVE_SOCKADDR_IN6_SIN6_SCOPE_ID 1

/* Define to 1 if you have the socket function. */
#define HAVE_SOCKET 1

/* Define to 1 if you have the socketpair function. */
#define HAVE_SOCKETPAIR 1

/* Define to 1 if you have the <socket.h> header file. */
/* #undef HAVE_SOCKET_H */

/* Define to 1 if you have the `SSLv2_client_method' function. */
/* #undef HAVE_SSLV2_CLIENT_METHOD */

/* Define to 1 if you have the `SSL_get_shutdown' function. */
#define HAVE_SSL_GET_SHUTDOWN 1

/* Define to 1 if you have the <ssl.h> header file. */
/* #undef HAVE_SSL_H */

/* Define to 1 if you have the <stdbool.h> header file. */
#define HAVE_STDBOOL_H 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdio.h> header file. */
#define HAVE_STDIO_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the strcasecmp function. */
#define HAVE_STRCASECMP 1

/* Define to 1 if you have the strcmpi function. */
/* #undef HAVE_STRCMPI */

/* Define to 1 if you have the strdup function. */
#define HAVE_STRDUP 1

/* Define to 1 if you have the strerror_r function. */
#define HAVE_STRERROR_R 1

/* Define to 1 if you have the stricmp function. */
/* #undef HAVE_STRICMP */

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the strncasecmp function. */
#define HAVE_STRNCASECMP 1

/* Define to 1 if you have the strncmpi function. */
/* #undef HAVE_STRNCMPI */

/* Define to 1 if you have the strnicmp function. */
/* #undef HAVE_STRNICMP */

/* Define to 1 if you have the <stropts.h> header file. */
//#define HAVE_STROPTS_H 1

/* Define to 1 if you have the strstr function. */
#define HAVE_STRSTR 1

/* Define to 1 if you have the strtok_r function. */
#define HAVE_STRTOK_R 1

/* Define to 1 if you have the strtoll function. */
#define HAVE_STRTOLL 1

/* if struct sockaddr_storage is defined */
#define HAVE_STRUCT_SOCKADDR_STORAGE 1

/* Define to 1 if you have the timeval struct. */
#define HAVE_STRUCT_TIMEVAL 1

/* Define to 1 if you have the <sys/filio.h> header file. */
/* #undef HAVE_SYS_FILIO_H */

/* Define to 1 if you have the <sys/ioctl.h> header file. */
//#define HAVE_SYS_IOCTL_H 1

/* Define to 1 if you have the <sys/param.h> header file. */
#define HAVE_SYS_PARAM_H 1

/* Define to 1 if you have the <sys/poll.h> header file. */
//#define HAVE_SYS_POLL_H 1

/* Define to 1 if you have the <sys/resource.h> header file. */
#define HAVE_SYS_RESOURCE_H 1

/* Define to 1 if you have the <sys/select.h> header file. */
//#define HAVE_SYS_SELECT_H 1

/* Define to 1 if you have the <sys/socket.h> header file. */
#define HAVE_SYS_SOCKET_H 1

/* Define to 1 if you have the <sys/sockio.h> header file. */
/* #undef HAVE_SYS_SOCKIO_H */

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/time.h> header file. */
#define HAVE_SYS_TIME_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <sys/uio.h> header file. */
#define HAVE_SYS_UIO_H 1

/* Define to 1 if you have the <sys/un.h> header file. */
//#define HAVE_SYS_UN_H 1

/* Define to 1 if you have the <sys/utime.h> header file. */
/* #undef HAVE_SYS_UTIME_H */

/* Define to 1 if you have the <sys/wait.h> header file. */
#define HAVE_SYS_WAIT_H 1

/* Define to 1 if you have the <sys/xattr.h> header file. */
#define HAVE_SYS_XATTR_H 1

/* Define to 1 if you have the <termios.h> header file. */
#define HAVE_TERMIOS_H 1

/* Define to 1 if you have the <termio.h> header file. */
#define HAVE_TERMIO_H 1

/* Define to 1 if you have the <time.h> header file. */
#define HAVE_TIME_H 1

/* Define to 1 if you have the `uname' function. */
#define HAVE_UNAME 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to 1 if you have the `utime' function. */
#define HAVE_UTIME 1

/* Define to 1 if you have the <utime.h> header file. */
#define HAVE_UTIME_H 1

/* Define to 1 if compiler supports C99 variadic macro style. */
#define HAVE_VARIADIC_MACROS_C99 1

/* Define to 1 if compiler supports old gcc variadic macro style. */
#define HAVE_VARIADIC_MACROS_GCC 1

/* Define to 1 if you have the winber.h header file. */
/* #undef HAVE_WINBER_H */

/* Define to 1 if you have the windows.h header file. */
/* #undef HAVE_WINDOWS_H */

/* Define to 1 if you have the winldap.h header file. */
/* #undef HAVE_WINLDAP_H */

/* Define to 1 if you have the winsock2.h header file. */
/* #undef HAVE_WINSOCK2_H */

/* Define to 1 if you have the winsock.h header file. */
/* #undef HAVE_WINSOCK_H */

/* Define to 1 if you have the `wolfSSLv3_client_method' function. */
/* #undef HAVE_WOLFSSLV3_CLIENT_METHOD */

/* Define to 1 if you have the `wolfSSL_CTX_UseSupportedCurve' function. */
/* #undef HAVE_WOLFSSL_CTX_USESUPPORTEDCURVE */

/* Define to 1 if you have the `wolfSSL_get_peer_certificate' function. */
/* #undef HAVE_WOLFSSL_GET_PEER_CERTIFICATE */

/* Define to 1 if you have the `wolfSSL_UseALPN' function. */
/* #undef HAVE_WOLFSSL_USEALPN */

/* Define this symbol if your OS supports changing the contents of argv */
#define HAVE_WRITABLE_ARGV 1

/* Define to 1 if you have the writev function. */
#define HAVE_WRITEV 1

/* Define to 1 if you have the ws2tcpip.h header file. */
/* #undef HAVE_WS2TCPIP_H */

/* Define to 1 if you have the <x509.h> header file. */
/* #undef HAVE_X509_H */

/* if you have the zlib.h header file */
//#define HAVE_ZLIB_H 1

/* Define to the sub-directory where libtool stores uninstalled libraries. */
#define LT_OBJDIR ".libs/"

/* Define to 1 if you need the lber.h header file even with ldap.h */
/* #undef NEED_LBER_H */

/* Define to 1 if you need the malloc.h header file even with stdlib.h */
/* #undef NEED_MALLOC_H */

/* Define to 1 if you need the memory.h header file even with stdlib.h */
/* #undef NEED_MEMORY_H */

/* Define to 1 if _REENTRANT preprocessor symbol must be defined. */
/* #undef NEED_REENTRANT */

/* Define to 1 if _THREAD_SAFE preprocessor symbol must be defined. */
/* #undef NEED_THREAD_SAFE */

/* Define to enable NTLM delegation to winbind's ntlm_auth helper. */
#define NTLM_WB_ENABLED 1

/* Define absolute filename for winbind's ntlm_auth helper. */
#define NTLM_WB_FILE "/usr/bin/ntlm_auth"

/* cpu-machine-OS */
#define OS "x86_64-pc-linux-gnu"

/* Name of package */
#define PACKAGE "curl"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "a suitable curl mailing list: https://curl.haxx.se/mail/"

/* Define to the full name of this package. */
#define PACKAGE_NAME "curl"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "curl -"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "curl"

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION "-"

/* a suitable file to read random data from */
#define RANDOM_FILE "/dev/urandom"

/* Define to the type of arg 1 for recv. */
#define RECV_TYPE_ARG1 int

/* Define to the type of arg 2 for recv. */
#define RECV_TYPE_ARG2 void *

/* Define to the type of arg 3 for recv. */
#define RECV_TYPE_ARG3 size_t

/* Define to the type of arg 4 for recv. */
#define RECV_TYPE_ARG4 int

/* Define to the function return type for recv. */
#define RECV_TYPE_RETV ssize_t

/* Define as the return type of signal handlers (`int' or `void'). */
#define RETSIGTYPE void

/* Define to the type qualifier of arg 5 for select. */
#define SELECT_QUAL_ARG5 

/* Define to the type of arg 1 for select. */
#define SELECT_TYPE_ARG1 int

/* Define to the type of args 2, 3 and 4 for select. */
#define SELECT_TYPE_ARG234 fd_set *

/* Define to the type of arg 5 for select. */
#define SELECT_TYPE_ARG5 struct timeval *

/* Define to the function return type for select. */
#define SELECT_TYPE_RETV int

/* Define to the type qualifier of arg 2 for send. */
#define SEND_QUAL_ARG2 const

/* Define to the type of arg 1 for send. */
#define SEND_TYPE_ARG1 int

/* Define to the type of arg 2 for send. */
#define SEND_TYPE_ARG2 void *

/* Define to the type of arg 3 for send. */
#define SEND_TYPE_ARG3 size_t

/* Define to the type of arg 4 for send. */
#define SEND_TYPE_ARG4 int

/* Define to the function return type for send. */
#define SEND_TYPE_RETV ssize_t

/* The size of `int', as computed by sizeof. */
#define SIZEOF_INT 4

/* The size of `long', as computed by sizeof. */
#define SIZEOF_LONG 4

/* The size of `long long', as computed by sizeof. */
/* #undef SIZEOF_LONG_LONG */

/* The size of `off_t', as computed by sizeof. */
#define SIZEOF_OFF_T 4

/* The size of `short', as computed by sizeof. */
#define SIZEOF_SHORT 2

/* The size of `size_t', as computed by sizeof. */
#define SIZEOF_SIZE_T 4

/* The size of `time_t', as computed by sizeof. */
#define SIZEOF_TIME_T 4

/* The size of `void*', as computed by sizeof. */
#define SIZEOF_VOIDP 4

#define SIZEOF_CURL_OFF_T 8

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Define to the type of arg 3 for strerror_r. */
#define STRERROR_R_TYPE_ARG3 size_t

/* Define to 1 if you can safely include both <sys/time.h> and <time.h>. */
#define TIME_WITH_SYS_TIME 1

/* Define to enable c-ares support */
/* #undef USE_ARES */

/* if axTLS is enabled */
/* #undef USE_AXTLS */

/* if CyaSSL/WolfSSL is enabled */
/* #undef USE_CYASSL */

/* to enable Apple OS native SSL/TLS support */
/* #undef USE_DARWINSSL */

/* if GnuTLS is enabled */
/* #undef USE_GNUTLS */

/* if GnuTLS uses nettle as crypto backend */
/* #undef USE_GNUTLS_NETTLE */

/* PSL support enabled */
/* #undef USE_LIBPSL */

/* if librtmp is in use */
/* #undef USE_LIBRTMP */

/* if libSSH2 is in use */
/* #undef USE_LIBSSH2 */

/* If you want to build curl with the built-in manual */
#define USE_MANUAL 1

/* if mbedTLS is enabled */
#undef USE_MBEDTLS
//#define USE_MBEDTLS 1

/* Define to enable metalink support */
/* #undef USE_METALINK */

/* if nghttp2 is in use */
/* #undef USE_NGHTTP2 */

/* if NSS is enabled */
/* #undef USE_NSS */

/* Use OpenLDAP-specific code */
/* #undef USE_OPENLDAP */

/* if OpenSSL is in use */
//#define USE_OPENSSL 1

/* if PolarSSL is enabled */
/* #undef USE_POLARSSL */

/* to enable Windows native SSL/TLS support */
/* #undef USE_SCHANNEL */

/* if you want POSIX threaded DNS lookup */
/* #undef USE_THREADS_POSIX */

/* if you want Win32 threaded DNS lookup */
/* #undef USE_THREADS_WIN32 */

/* Use TLS-SRP authentication */
#define USE_TLS_SRP 1

/* Use Unix domain sockets */
//#define USE_UNIX_SOCKETS 1

/* Define to 1 if you have the `normaliz' (WinIDN) library (-lnormaliz). */
/* #undef USE_WIN32_IDN */

/* Define to 1 if you are building a Windows target with large file support.
   */
/* #undef USE_WIN32_LARGE_FILES */

/* Use Windows LDAP implementation */
/* #undef USE_WIN32_LDAP */

/* Define to 1 if you are building a Windows target without large file
   support. */
/* #undef USE_WIN32_SMALL_FILES */

/* to enable SSPI support */
/* #undef USE_WINDOWS_SSPI */

/* Version number of package */
#define VERSION "-"

/* Define to 1 to provide own prototypes. */
/* #undef WANT_IDN_PROTOTYPES */

/* Define to avoid automatic inclusion of winsock.h */
/* #undef WIN32_LEAN_AND_MEAN */

/* Define to 1 if OS is AIX. */
#ifndef _ALL_SOURCE
/* #  undef _ALL_SOURCE */
#endif

/* Enable large inode numbers on Mac OS X 10.5.  */
#ifndef _DARWIN_USE_64_BIT_INODE
# define _DARWIN_USE_64_BIT_INODE 1
#endif

/* Number of bits in a file offset, on hosts where this is settable. */
/* #undef _FILE_OFFSET_BITS */

/* Define for large files, on AIX-style hosts. */
/* #undef _LARGE_FILES */

/* Define to empty if `const' does not conform to ANSI C. */
/* #undef const */

/* Type to use in place of in_addr_t when system does not provide it. */
/* #undef in_addr_t */

/* Define to `__inline__' or `__inline' if that's what the C compiler
   calls it, or to nothing if 'inline' is not supported under any name.  */
#ifndef __cplusplus
/* #undef inline */
#endif

/* Define to `unsigned int' if <sys/types.h> does not define. */
/* #undef size_t */

/* the signed version of size_t */
/* #undef ssize_t */

#ifndef HEADER_CURL_SETUP_H
#define HEADER_CURL_SETUP_H
/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at https://curl.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 * SPDX-License-Identifier: curl
 *
 ***************************************************************************/

#if defined(BUILDING_LIBCURL) && !defined(CURL_NO_OLDIES)
#define CURL_NO_OLDIES
#endif

/* Tell "curl/curl.h" not to include "curl/mprintf.h" */
#define CURL_SKIP_INCLUDE_MPRINTF

/* FIXME: Delete this once the warnings have been fixed. */
#if !defined(CURL_WARN_SIGN_CONVERSION)
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic ignored "-Wsign-conversion"
#endif
#endif

/* Set default _WIN32_WINNT */
#ifdef __MINGW32__
#include <_mingw.h>
#endif

/* Workaround for Homebrew gcc 12.4.0, 13.3.0, 14.1.0, 14.2.0 (initial build)
   that started advertising the `availability` attribute, which then gets used
   by Apple SDK, but, in a way incompatible with gcc, resulting in misc errors
   inside SDK headers, e.g.:
     error: attributes should be specified before the declarator in a function
            definition
     error: expected ',' or '}' before
   Followed by missing declarations.
   Work it around by overriding the built-in feature-check macro used by the
   headers to enable the problematic attributes. This makes the feature check
   fail. Fixed in 14.2.0_1. Disable the workaround if the fix is detected. */
#if defined(__APPLE__) && !defined(__clang__) && defined(__GNUC__) && \
  defined(__has_attribute)
#  if !defined(__has_feature)
#    define availability curl_pp_attribute_disabled
#  elif !__has_feature(attribute_availability)
#    define availability curl_pp_attribute_disabled
#  endif
#endif

#if defined(__APPLE__)
#include <sys/types.h>
#include <TargetConditionals.h>
/* Fixup faulty target macro initialization in macOS SDK since v14.4 (as of
   15.0 beta). The SDK target detection in `TargetConditionals.h` correctly
   detects macOS, but fails to set the macro's old name `TARGET_OS_OSX`, then
   continues to set it to a default value of 0. Other parts of the SDK still
   rely on the old name, and with this inconsistency our builds fail due to
   missing declarations. It happens when using mainline llvm older than v18.
   Later versions fixed it by predefining these target macros, avoiding the
   faulty dynamic detection. gcc is not affected (for now) because it lacks
   the necessary dynamic detection features, so the SDK falls back to
   a codepath that sets both the old and new macro to 1. */
#if defined(TARGET_OS_MAC) && TARGET_OS_MAC && \
  defined(TARGET_OS_OSX) && !TARGET_OS_OSX && \
  (!defined(TARGET_OS_IPHONE) || !TARGET_OS_IPHONE) && \
  (!defined(TARGET_OS_SIMULATOR) || !TARGET_OS_SIMULATOR)
#undef TARGET_OS_OSX
#define TARGET_OS_OSX TARGET_OS_MAC
#endif
#endif

/*
 * Disable Visual Studio warnings:
 * 4127 "conditional expression is constant"
 */
#ifdef _MSC_VER
#pragma warning(disable:4127)
#endif

#ifdef _WIN32
/*
 * Do not include unneeded stuff in Windows headers to avoid compiler
 * warnings and macro clashes.
 * Make sure to define this macro before including any Windows headers.
 */
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif
#  ifndef NOGDI
#    define NOGDI
#  endif
/* Detect Windows App environment which has a restricted access
 * to the Win32 APIs. */
# if (defined(_WIN32_WINNT) && (_WIN32_WINNT >= 0x0602)) || \
  defined(WINAPI_FAMILY)
#  include <winapifamily.h>
#  if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_APP) &&  \
     !WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
#    define CURL_WINDOWS_UWP
#  endif
# endif
#endif

/* Compatibility */
#if defined(ENABLE_IPV6)
#  define USE_IPV6 1
#endif

/*
 * Include configuration script results or hand-crafted
 * configuration file for platforms which lack config tool.
 */

#ifdef HAVE_CONFIG_H

#include "curl_config.h"

#else /* HAVE_CONFIG_H */

#ifdef _WIN32_WCE
#  include "config-win32ce.h"
#else
#  ifdef _WIN32
#    include "config-win32.h"
#  endif
#endif

#ifdef macintosh
#  include "config-mac.h"
#endif

#ifdef __riscos__
#  include "config-riscos.h"
#endif

#ifdef __AMIGA__
#  include "config-amigaos.h"
#endif

#ifdef __OS400__
#  include "config-os400.h"
#endif

#ifdef __PLAN9__
#  include "config-plan9.h"
#endif

#ifdef MSDOS
#  include "config-dos.h"
#endif

#endif /* HAVE_CONFIG_H */

/* ================================================================ */
/* Definition of preprocessor macros/symbols which modify compiler  */
/* behavior or generated code characteristics must be done here,   */
/* as appropriate, before any system header file is included. It is */
/* also possible to have them defined in the config file included   */
/* before this point. As a result of all this we frown inclusion of */
/* system header files in our config files, avoid this at any cost. */
/* ================================================================ */

/*
 * AIX 4.3 and newer needs _THREAD_SAFE defined to build
 * proper reentrant code. Others may also need it.
 */

#ifdef NEED_THREAD_SAFE
#  ifndef _THREAD_SAFE
#    define _THREAD_SAFE
#  endif
#endif

/*
 * Tru64 needs _REENTRANT set for a few function prototypes and
 * things to appear in the system header files. Unixware needs it
 * to build proper reentrant code. Others may also need it.
 */

#ifdef NEED_REENTRANT
#  ifndef _REENTRANT
#    define _REENTRANT
#  endif
#endif

/* Solaris needs this to get a POSIX-conformant getpwuid_r */
#if defined(sun) || defined(__sun)
#  ifndef _POSIX_PTHREAD_SEMANTICS
#    define _POSIX_PTHREAD_SEMANTICS 1
#  endif
#endif

/* ================================================================ */
/*  If you need to include a system header file for your platform,  */
/*  please, do it beyond the point further indicated in this file.  */
/* ================================================================ */

/* Give calloc a chance to be dragging in early, so we do not redefine */
#if defined(USE_THREADS_POSIX) && defined(HAVE_PTHREAD_H)
#  include <pthread.h>
#endif

/*
 * Disable other protocols when http is the only one desired.
 */

#ifdef HTTP_ONLY
#  ifndef CURL_DISABLE_DICT
#    define CURL_DISABLE_DICT
#  endif
#  ifndef CURL_DISABLE_FILE
#    define CURL_DISABLE_FILE
#  endif
#  ifndef CURL_DISABLE_FTP
#    define CURL_DISABLE_FTP
#  endif
#  ifndef CURL_DISABLE_GOPHER
#    define CURL_DISABLE_GOPHER
#  endif
#  ifndef CURL_DISABLE_IMAP
#    define CURL_DISABLE_IMAP
#  endif
#  ifndef CURL_DISABLE_LDAP
#    define CURL_DISABLE_LDAP
#  endif
#  ifndef CURL_DISABLE_LDAPS
#    define CURL_DISABLE_LDAPS
#  endif
#  ifndef CURL_DISABLE_MQTT
#    define CURL_DISABLE_MQTT
#  endif
#  ifndef CURL_DISABLE_POP3
#    define CURL_DISABLE_POP3
#  endif
#  ifndef CURL_DISABLE_RTSP
#    define CURL_DISABLE_RTSP
#  endif
#  ifndef CURL_DISABLE_SMB
#    define CURL_DISABLE_SMB
#  endif
#  ifndef CURL_DISABLE_SMTP
#    define CURL_DISABLE_SMTP
#  endif
#  ifndef CURL_DISABLE_TELNET
#    define CURL_DISABLE_TELNET
#  endif
#  ifndef CURL_DISABLE_TFTP
#    define CURL_DISABLE_TFTP
#  endif
#endif

/*
 * When http is disabled rtsp is not supported.
 */

#if defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_RTSP)
#  define CURL_DISABLE_RTSP
#endif

/*
 * When HTTP is disabled, disable HTTP-only features
 */

#if defined(CURL_DISABLE_HTTP)
#  define CURL_DISABLE_ALTSVC 1
#  define CURL_DISABLE_COOKIES 1
#  define CURL_DISABLE_BASIC_AUTH 1
#  define CURL_DISABLE_BEARER_AUTH 1
#  define CURL_DISABLE_AWS 1
#  define CURL_DISABLE_DOH 1
#  define CURL_DISABLE_FORM_API 1
#  define CURL_DISABLE_HEADERS_API 1
#  define CURL_DISABLE_HSTS 1
#  define CURL_DISABLE_HTTP_AUTH 1
#endif

/* ================================================================ */
/* No system header file shall be included in this file before this */
/* point.                                                           */
/* ================================================================ */

/*
 * OS/400 setup file includes some system headers.
 */

#ifdef __OS400__
#  include "setup-os400.h"
#endif

/*
 * VMS setup file includes some system headers.
 */

#ifdef __VMS
#  include "setup-vms.h"
#endif

/*
 * Windows setup file includes some system headers.
 */

#ifdef _WIN32
#  include "setup-win32.h"
#endif

#include <curl/system.h>

/* Helper macro to expand and concatenate two macros.
 * Direct macros concatenation does not work because macros
 * are not expanded before direct concatenation.
 */
#define CURL_CONC_MACROS_(A,B) A ## B
#define CURL_CONC_MACROS(A,B) CURL_CONC_MACROS_(A,B)

/* curl uses its own printf() function internally. It understands the GNU
 * format. Use this format, so that is matches the GNU format attribute we
 * use with the MinGW compiler, allowing it to verify them at compile-time.
 */
#ifdef  __MINGW32__
#  undef CURL_FORMAT_CURL_OFF_T
#  undef CURL_FORMAT_CURL_OFF_TU
#  define CURL_FORMAT_CURL_OFF_T   "lld"
#  define CURL_FORMAT_CURL_OFF_TU  "llu"
#endif

/* based on logic in "curl/mprintf.h" */

#if (defined(__GNUC__) || defined(__clang__) ||                         \
  defined(__IAR_SYSTEMS_ICC__)) &&                                      \
  defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L) &&         \
  !defined(CURL_NO_FMT_CHECKS)
#if defined(__MINGW32__) && !defined(__clang__)
#define CURL_PRINTF(fmt, arg) \
  __attribute__((format(gnu_printf, fmt, arg)))
#else
#define CURL_PRINTF(fmt, arg) \
  __attribute__((format(__printf__, fmt, arg)))
#endif
#else
#define CURL_PRINTF(fmt, arg)
#endif

/* Override default printf mask check rules in "curl/mprintf.h" */
#define CURL_TEMP_PRINTF CURL_PRINTF

/* Workaround for mainline llvm v16 and earlier missing a built-in macro
   expected by macOS SDK v14 / Xcode v15 (2023) and newer.
   gcc (as of v14) is also missing it. */
#if defined(__APPLE__) &&                                   \
  ((!defined(__apple_build_version__) &&                    \
    defined(__clang__) && __clang_major__ < 17) ||          \
   (defined(__GNUC__) && __GNUC__ <= 14)) &&                \
  defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__) && \
  !defined(__ENVIRONMENT_OS_VERSION_MIN_REQUIRED__)
#define __ENVIRONMENT_OS_VERSION_MIN_REQUIRED__             \
  __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__
#endif

/*
 * Use getaddrinfo to resolve the IPv4 address literal. If the current network
 * interface does not support IPv4, but supports IPv6, NAT64, and DNS64,
 * performing this task will result in a synthesized IPv6 address.
 */
#if defined(__APPLE__) && !defined(USE_ARES)
#define USE_RESOLVE_ON_IPS 1
#  if TARGET_OS_MAC && !(defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE) && \
     defined(USE_IPV6)
#    define CURL_MACOS_CALL_COPYPROXIES 1
#  endif
#endif

#ifdef USE_LWIPSOCK
#  include <lwip/init.h>
#  include <lwip/sockets.h>
#  include <lwip/netdb.h>
#endif

#ifdef HAVE_EXTRA_STRICMP_H
#  include <extra/stricmp.h>
#endif

#ifdef HAVE_EXTRA_STRDUP_H
#  include <extra/strdup.h>
#endif

#ifdef __AMIGA__
#  ifdef __amigaos4__
#    define __USE_INLINE__
     /* use our own resolver which uses runtime feature detection */
#    define CURLRES_AMIGA
     /* getaddrinfo() currently crashes bsdsocket.library, so disable */
#    undef HAVE_GETADDRINFO
#    if !(defined(__NEWLIB__) || \
          (defined(__CLIB2__) && defined(__THREAD_SAFE)))
       /* disable threaded resolver with clib2 - requires newlib or clib-ts */
#      undef USE_THREADS_POSIX
#    endif
#  endif
#  include <exec/types.h>
#  include <exec/execbase.h>
#  include <proto/exec.h>
#  include <proto/dos.h>
#  include <unistd.h>
#  if defined(HAVE_PROTO_BSDSOCKET_H) && \
    (!defined(__amigaos4__) || defined(USE_AMISSL))
     /* use bsdsocket.library directly, instead of libc networking functions */
#    define _SYS_MBUF_H /* m_len define clashes with curl */
#    include <proto/bsdsocket.h>
#    ifdef __amigaos4__
       int Curl_amiga_select(int nfds, fd_set *readfds, fd_set *writefds,
                             fd_set *errorfds, struct timeval *timeout);
#      define select(a,b,c,d,e) Curl_amiga_select(a,b,c,d,e)
#    else
#      define select(a,b,c,d,e) WaitSelect(a,b,c,d,e,0)
#    endif
     /* must not use libc's fcntl() on bsdsocket.library sockfds! */
#    undef HAVE_FCNTL
#    undef HAVE_FCNTL_O_NONBLOCK
#  else
     /* use libc networking and hence close() and fnctl() */
#    undef HAVE_CLOSESOCKET_CAMEL
#    undef HAVE_IOCTLSOCKET_CAMEL
#  endif
/*
 * In clib2 arpa/inet.h warns that some prototypes may clash
 * with bsdsocket.library. This avoids the definition of those.
 */
#  define __NO_NET_API
#endif

#include <stdio.h>
#include <assert.h>

#ifdef __TANDEM /* for ns*-tandem-nsk systems */
# if ! defined __LP64
#  include <floss.h> /* FLOSS is only used for 32-bit builds. */
# endif
#endif

#ifndef STDC_HEADERS /* no standard C headers! */
#include <curl/stdcheaders.h>
#endif

/*
 * Large file (>2Gb) support using Win32 functions.
 */

#ifdef USE_WIN32_LARGE_FILES
#  include <io.h>
#  include <sys/types.h>
#  include <sys/stat.h>
#  undef  lseek
#  define lseek(fdes,offset,whence)  _lseeki64(fdes, offset, whence)
#  undef  fstat
#  define fstat(fdes,stp)            _fstati64(fdes, stp)
#  undef  stat
#  define stat(fname,stp)            curlx_win32_stat(fname, stp)
#  define struct_stat                struct _stati64
#  define LSEEK_ERROR                (__int64)-1
#  define open                       curlx_win32_open
#  define fopen(fname,mode)          curlx_win32_fopen(fname, mode)
   int curlx_win32_open(const char *filename, int oflag, ...);
   int curlx_win32_stat(const char *path, struct_stat *buffer);
   FILE *curlx_win32_fopen(const char *filename, const char *mode);
#endif

/*
 * Small file (<2Gb) support using Win32 functions.
 */

#ifdef USE_WIN32_SMALL_FILES
#  include <io.h>
#  include <sys/types.h>
#  include <sys/stat.h>
#  ifndef _WIN32_WCE
#    undef  lseek
#    define lseek(fdes,offset,whence)  _lseek(fdes, (long)offset, whence)
#    define fstat(fdes,stp)            _fstat(fdes, stp)
#    define stat(fname,stp)            curlx_win32_stat(fname, stp)
#    define struct_stat                struct _stat
#    define open                       curlx_win32_open
#    define fopen(fname,mode)          curlx_win32_fopen(fname, mode)
     int curlx_win32_stat(const char *path, struct_stat *buffer);
     int curlx_win32_open(const char *filename, int oflag, ...);
     FILE *curlx_win32_fopen(const char *filename, const char *mode);
#  endif
#  define LSEEK_ERROR                (long)-1
#endif

#ifndef struct_stat
#  define struct_stat struct stat
#endif

#ifndef LSEEK_ERROR
#  define LSEEK_ERROR (off_t)-1
#endif

#ifndef SIZEOF_TIME_T
/* assume default size of time_t to be 32 bits */
#define SIZEOF_TIME_T 4
#endif

#ifndef SIZEOF_CURL_SOCKET_T
/* configure and cmake check and set the define */
#  ifdef _WIN64
#    define SIZEOF_CURL_SOCKET_T 8
#  else
/* default guess */
#    define SIZEOF_CURL_SOCKET_T 4
#  endif
#endif

#if SIZEOF_CURL_SOCKET_T < 8
#  define FMT_SOCKET_T "d"
#elif defined(__MINGW32__)
#  define FMT_SOCKET_T "zd"
#else
#  define FMT_SOCKET_T "qd"
#endif

/*
 * Default sizeof(off_t) in case it has not been defined in config file.
 */

#ifndef SIZEOF_OFF_T
#  if defined(__VMS) && !defined(__VAX)
#    if defined(_LARGEFILE)
#      define SIZEOF_OFF_T 8
#    endif
#  elif defined(__OS400__) && defined(__ILEC400__)
#    if defined(_LARGE_FILES)
#      define SIZEOF_OFF_T 8
#    endif
#  elif defined(__MVS__) && defined(__IBMC__)
#    if defined(_LP64) || defined(_LARGE_FILES)
#      define SIZEOF_OFF_T 8
#    endif
#  elif defined(__370__) && defined(__IBMC__)
#    if defined(_LP64) || defined(_LARGE_FILES)
#      define SIZEOF_OFF_T 8
#    endif
#  endif
#  ifndef SIZEOF_OFF_T
#    define SIZEOF_OFF_T 4
#  endif
#endif

#if (SIZEOF_CURL_OFF_T < 8)
#error "too small curl_off_t"
#else
   /* assume SIZEOF_CURL_OFF_T == 8 */
#  define CURL_OFF_T_MAX CURL_OFF_T_C(0x7FFFFFFFFFFFFFFF)
#endif
#define CURL_OFF_T_MIN (-CURL_OFF_T_MAX - CURL_OFF_T_C(1))

#if (SIZEOF_CURL_OFF_T != 8)
#  error "curl_off_t must be exactly 64 bits"
#else
  typedef unsigned CURL_TYPEOF_CURL_OFF_T curl_uint64_t;
  typedef CURL_TYPEOF_CURL_OFF_T  curl_int64_t;
#  ifndef CURL_SUFFIX_CURL_OFF_TU
#    error "CURL_SUFFIX_CURL_OFF_TU must be defined"
#  endif
#  define CURL_UINT64_SUFFIX  CURL_SUFFIX_CURL_OFF_TU
#  define CURL_UINT64_C(val)  CURL_CONC_MACROS(val,CURL_UINT64_SUFFIX)
# define FMT_PRId64  CURL_FORMAT_CURL_OFF_T
# define FMT_PRIu64  CURL_FORMAT_CURL_OFF_TU
#endif

#define FMT_OFF_T CURL_FORMAT_CURL_OFF_T
#define FMT_OFF_TU CURL_FORMAT_CURL_OFF_TU

#if (SIZEOF_TIME_T == 4)
#  ifdef HAVE_TIME_T_UNSIGNED
#  define TIME_T_MAX UINT_MAX
#  define TIME_T_MIN 0
#  else
#  define TIME_T_MAX INT_MAX
#  define TIME_T_MIN INT_MIN
#  endif
#else
#  ifdef HAVE_TIME_T_UNSIGNED
#  define TIME_T_MAX 0xFFFFFFFFFFFFFFFF
#  define TIME_T_MIN 0
#  else
#  define TIME_T_MAX 0x7FFFFFFFFFFFFFFF
#  define TIME_T_MIN (-TIME_T_MAX - 1)
#  endif
#endif

#ifndef SIZE_T_MAX
/* some limits.h headers have this defined, some do not */
#if defined(SIZEOF_SIZE_T) && (SIZEOF_SIZE_T > 4)
#define SIZE_T_MAX 18446744073709551615U
#else
#define SIZE_T_MAX 4294967295U
#endif
#endif

#ifndef SSIZE_T_MAX
/* some limits.h headers have this defined, some do not */
#if defined(SIZEOF_SIZE_T) && (SIZEOF_SIZE_T > 4)
#define SSIZE_T_MAX 9223372036854775807
#else
#define SSIZE_T_MAX 2147483647
#endif
#endif

/*
 * Arg 2 type for gethostname in case it has not been defined in config file.
 */

#ifndef GETHOSTNAME_TYPE_ARG2
#  ifdef USE_WINSOCK
#    define GETHOSTNAME_TYPE_ARG2 int
#  else
#    define GETHOSTNAME_TYPE_ARG2 size_t
#  endif
#endif

/* Below we define some functions. They should

   4. set the SIGALRM signal timeout
   5. set dir/file naming defines
   */

#ifdef _WIN32

#  define DIR_CHAR      "\\"

#else /* _WIN32 */

#  ifdef MSDOS  /* Watt-32 */

#    include <sys/ioctl.h>
#    define select(n,r,w,x,t) select_s(n,r,w,x,t)
#    define ioctl(x,y,z) ioctlsocket(x,y,(char *)(z))
#    include <tcp.h>
#    ifdef word
#      undef word
#    endif
#    ifdef byte
#      undef byte
#    endif

#  endif /* MSDOS */

#  ifdef __minix
     /* Minix 3 versions up to at least 3.1.3 are missing these prototypes */
     extern char *strtok_r(char *s, const char *delim, char **last);
     extern struct tm *gmtime_r(const time_t * const timep, struct tm *tmp);
#  endif

#  define DIR_CHAR      "/"

#endif /* _WIN32 */

/* ---------------------------------------------------------------- */
/*             resolver specialty compile-time defines              */
/*         CURLRES_* defines to use in the host*.c sources          */
/* ---------------------------------------------------------------- */

/*
 * MSVC threads support requires a multi-threaded runtime library.
 * _beginthreadex() is not available in single-threaded ones.
 */

#if defined(_MSC_VER) && !defined(_MT)
#  undef USE_THREADS_POSIX
#  undef USE_THREADS_WIN32
#endif

/*
 * Mutually exclusive CURLRES_* definitions.
 */

#if defined(USE_IPV6) && defined(HAVE_GETADDRINFO)
#  define CURLRES_IPV6
#elif defined(USE_IPV6) && (defined(_WIN32) || defined(__CYGWIN__))
/* assume on Windows that IPv6 without getaddrinfo is a broken build */
#  error "Unexpected build: IPv6 is enabled but getaddrinfo was not found."
#else
#  define CURLRES_IPV4
#endif

#ifdef USE_ARES
#  define CURLRES_ASYNCH
#  define CURLRES_ARES
/* now undef the stock libc functions just to avoid them being used */
#  undef HAVE_GETADDRINFO
#  undef HAVE_FREEADDRINFO
#elif defined(USE_THREADS_POSIX) || defined(USE_THREADS_WIN32)
#  define CURLRES_ASYNCH
#  define CURLRES_THREADED
#else
#  define CURLRES_SYNCH
#endif

/* ---------------------------------------------------------------- */

#if defined(HAVE_LIBIDN2) && defined(HAVE_IDN2_H) && \
  !defined(USE_WIN32_IDN) && !defined(USE_APPLE_IDN)
/* The lib and header are present */
#define USE_LIBIDN2
#endif

#if defined(USE_LIBIDN2) && (defined(USE_WIN32_IDN) || defined(USE_APPLE_IDN))
#error "libidn2 cannot be enabled with WinIDN or AppleIDN, choose one."
#endif

#define LIBIDN_REQUIRED_VERSION "0.4.1"

#if defined(USE_GNUTLS) || defined(USE_OPENSSL) || defined(USE_MBEDTLS) || \
  defined(USE_WOLFSSL) || defined(USE_SCHANNEL) || defined(USE_SECTRANSP) || \
  defined(USE_BEARSSL) || defined(USE_RUSTLS)
#define USE_SSL    /* SSL support has been enabled */
#endif

#if defined(USE_WOLFSSL) && defined(USE_GNUTLS)
/* Avoid defining unprefixed wolfSSL SHA macros colliding with nettle ones */
#define NO_OLD_WC_NAMES
#endif

/* Single point where USE_SPNEGO definition might be defined */
#if !defined(CURL_DISABLE_NEGOTIATE_AUTH) && \
    (defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI))
#define USE_SPNEGO
#endif

/* Single point where USE_KERBEROS5 definition might be defined */
#if !defined(CURL_DISABLE_KERBEROS_AUTH) && \
    (defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI))
#define USE_KERBEROS5
#endif

/* Single point where USE_NTLM definition might be defined */
#if !defined(CURL_DISABLE_NTLM)
#  if defined(USE_OPENSSL) || defined(USE_MBEDTLS) ||                   \
  defined(USE_GNUTLS) || defined(USE_SECTRANSP) ||                      \
  defined(USE_OS400CRYPTO) || defined(USE_WIN32_CRYPTO) ||              \
  (defined(USE_WOLFSSL) && defined(HAVE_WOLFSSL_DES_ECB_ENCRYPT))
#    define USE_CURL_NTLM_CORE
#  endif
#  if defined(USE_CURL_NTLM_CORE) || defined(USE_WINDOWS_SSPI)
#    define USE_NTLM
#  endif
#endif

#ifdef CURL_WANTS_CA_BUNDLE_ENV
#error "No longer supported. Set CURLOPT_CAINFO at runtime instead."
#endif

#if defined(USE_LIBSSH2) || defined(USE_LIBSSH) || defined(USE_WOLFSSH)
#define USE_SSH
#endif

/*
 * Provide a mechanism to silence picky compilers, such as gcc 4.6+.
 * Parameters should of course normally not be unused, but for example when
 * we have multiple implementations of the same interface it may happen.
 */

#if defined(__GNUC__) && ((__GNUC__ >= 3) || \
  ((__GNUC__ == 2) && defined(__GNUC_MINOR__) && (__GNUC_MINOR__ >= 7)))
#  define UNUSED_PARAM __attribute__((__unused__))
#  define WARN_UNUSED_RESULT __attribute__((warn_unused_result))
#elif defined(__IAR_SYSTEMS_ICC__)
#  define UNUSED_PARAM __attribute__((__unused__))
#  if (__VER__ >= 9040001)
#    define WARN_UNUSED_RESULT __attribute__((warn_unused_result))
#  else
#    define WARN_UNUSED_RESULT
#  endif
#else
#  define UNUSED_PARAM /* NOTHING */
#  define WARN_UNUSED_RESULT
#endif

/* noreturn attribute */

#if !defined(CURL_NORETURN)
#if (defined(__GNUC__) && (__GNUC__ >= 3)) || defined(__clang__) || \
  defined(__IAR_SYSTEMS_ICC__)
#  define CURL_NORETURN  __attribute__((__noreturn__))
#elif defined(_MSC_VER) && (_MSC_VER >= 1200)
#  define CURL_NORETURN  __declspec(noreturn)
#else
#  define CURL_NORETURN
#endif
#endif

/* fallthrough attribute */

#if !defined(FALLTHROUGH)
#if (defined(__GNUC__) && __GNUC__ >= 7) || \
    (defined(__clang__) && __clang_major__ >= 10)
#  define FALLTHROUGH()  __attribute__((fallthrough))
#else
#  define FALLTHROUGH()  do {} while (0)
#endif
#endif

/*
 * Include macros and defines that should only be processed once.
 */

#ifndef HEADER_CURL_SETUP_ONCE_H
#include "curl_setup_once.h"
#endif

/*
 * Definition of our NOP statement Object-like macro
 */

#ifndef Curl_nop_stmt
#  define Curl_nop_stmt do { } while(0)
#endif

/*
 * Ensure that Winsock and lwIP TCP/IP stacks are not mixed.
 */

#if defined(__LWIP_OPT_H__) || defined(LWIP_HDR_OPT_H)
#  if defined(SOCKET) || defined(USE_WINSOCK)
#    error "Winsock and lwIP TCP/IP stack definitions shall not coexist!"
#  endif
#endif

/*
 * shutdown() flags for systems that do not define them
 */

#ifndef SHUT_RD
#define SHUT_RD 0x00
#endif

#ifndef SHUT_WR
#define SHUT_WR 0x01
#endif

#ifndef SHUT_RDWR
#define SHUT_RDWR 0x02
#endif

/* Define S_ISREG if not defined by system headers, e.g. MSVC */
#if !defined(S_ISREG) && defined(S_IFMT) && defined(S_IFREG)
#define S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#endif

/* Define S_ISDIR if not defined by system headers, e.g. MSVC */
#if !defined(S_ISDIR) && defined(S_IFMT) && defined(S_IFDIR)
#define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#endif

/* In Windows the default file mode is text but an application can override it.
Therefore we specify it explicitly. https://github.com/curl/curl/pull/258
*/
#if defined(_WIN32) || defined(MSDOS)
#define FOPEN_READTEXT "rt"
#define FOPEN_WRITETEXT "wt"
#define FOPEN_APPENDTEXT "at"
#elif defined(__CYGWIN__)
/* Cygwin has specific behavior we need to address when _WIN32 is not defined.
https://cygwin.com/cygwin-ug-net/using-textbinary.html
For write we want our output to have line endings of LF and be compatible with
other Cygwin utilities. For read we want to handle input that may have line
endings either CRLF or LF so 't' is appropriate.
*/
#define FOPEN_READTEXT "rt"
#define FOPEN_WRITETEXT "w"
#define FOPEN_APPENDTEXT "a"
#else
#define FOPEN_READTEXT "r"
#define FOPEN_WRITETEXT "w"
#define FOPEN_APPENDTEXT "a"
#endif

/* for systems that do not detect this in configure */
#ifndef CURL_SA_FAMILY_T
#  if defined(HAVE_SA_FAMILY_T)
#    define CURL_SA_FAMILY_T sa_family_t
#  elif defined(HAVE_ADDRESS_FAMILY)
#    define CURL_SA_FAMILY_T ADDRESS_FAMILY
#  else
/* use a sensible default */
#    define CURL_SA_FAMILY_T unsigned short
#  endif
#endif

/* Some convenience macros to get the larger/smaller value out of two given.
   We prefix with CURL to prevent name collisions. */
#define CURLMAX(x,y) ((x)>(y)?(x):(y))
#define CURLMIN(x,y) ((x)<(y)?(x):(y))

/* A convenience macro to provide both the string literal and the length of
   the string literal in one go, useful for functions that take "string,len"
   as their argument */
#define STRCONST(x) x,sizeof(x)-1

/* Some versions of the Android SDK is missing the declaration */
#if defined(HAVE_GETPWUID_R) && defined(HAVE_DECL_GETPWUID_R_MISSING)
struct passwd;
int getpwuid_r(uid_t uid, struct passwd *pwd, char *buf,
               size_t buflen, struct passwd **result);
#endif

#ifdef UNITTESTS
#define UNITTEST
#else
#define UNITTEST static
#endif

/* Hyper supports HTTP2 also, but Curl's integration with Hyper does not */
#if defined(USE_NGHTTP2)
#define USE_HTTP2
#endif

#if (defined(USE_NGTCP2) && defined(USE_NGHTTP3)) || \
    (defined(USE_OPENSSL_QUIC) && defined(USE_NGHTTP3)) || \
    defined(USE_QUICHE) || defined(USE_MSH3)

#ifdef CURL_WITH_MULTI_SSL
#error "Multi-SSL combined with QUIC is not supported"
#endif

#define USE_HTTP3
#endif

/* Certain Windows implementations are not aligned with what curl expects,
   so always use the local one on this platform. E.g. the mingw-w64
   implementation can return wrong results for non-ASCII inputs. */
#if defined(HAVE_BASENAME) && defined(_WIN32)
#undef HAVE_BASENAME
#endif

#if defined(USE_UNIX_SOCKETS) && defined(_WIN32)
#  if !defined(UNIX_PATH_MAX)
     /* Replicating logic present in afunix.h
        (distributed with newer Windows 10 SDK versions only) */
#    define UNIX_PATH_MAX 108
     /* !checksrc! disable TYPEDEFSTRUCT 1 */
     typedef struct sockaddr_un {
       ADDRESS_FAMILY sun_family;
       char sun_path[UNIX_PATH_MAX];
     } SOCKADDR_UN, *PSOCKADDR_UN;
#    define WIN32_SOCKADDR_UN
#  endif
#endif

/* OpenSSLv3 marks DES, MD5 and ENGINE functions deprecated but we have no
   replacements (yet) so tell the compiler to not warn for them. */
#ifdef USE_OPENSSL
#define OPENSSL_SUPPRESS_DEPRECATED
#endif

#if defined(inline)
  /* 'inline' is defined as macro and assumed to be correct */
  /* No need for 'inline' replacement */
#elif defined(__cplusplus)
  /* The code is compiled with C++ compiler.
     C++ always supports 'inline'. */
  /* No need for 'inline' replacement */
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901
  /* C99 (and later) supports 'inline' keyword */
  /* No need for 'inline' replacement */
#elif defined(__GNUC__) && __GNUC__ >= 3
  /* GCC supports '__inline__' as an extension */
#  define inline __inline__
#elif defined(_MSC_VER) && _MSC_VER >= 1400
  /* MSC supports '__inline' from VS 2005 (or even earlier) */
#  define inline __inline
#else
  /* Probably 'inline' is not supported by compiler.
     Define to the empty string to be on the safe side. */
#  define inline /* empty */
#endif

#endif /* HEADER_CURL_SETUP_H */
