/*----------------------------------------------------------------------------/
  Lovyan GFX library - LCD graphics library .

  support platform:
    ESP32 (SPI/I2S) with Arduino/ESP-IDF
    ATSAMD51 (SPI) with Arduino

Original Source:
 https://github.com/lovyan03/LovyanGFX/

Licence:
 [BSD](https://github.com/lovyan03/LovyanGFX/blob/master/license.txt)

Author:
 [lovyan03](https://twitter.com/lovyan03)

Contributors:
 [ciniml](https://github.com/ciniml)
 [mongonta0716](https://github.com/mongonta0716)
 [tobozo](https://github.com/tobozo)
/----------------------------------------------------------------------------*/
#ifndef LGFX_FILESYSTEM_SUPPORT_HPP_
#define LGFX_FILESYSTEM_SUPPORT_HPP_

#include "misc/enum.hpp"

#include <math.h>
#include <string.h>

#include "../internal/alloca.h"

namespace lgfx
{
 inline namespace v1
 {
//----------------------------------------------------------------------------

  /// Warning : Do not add any data members to this class.
  /// This class only allows include order dependent extension methods.
  /// ( e.g. file system related, HTTP related )
  /// このクラスにメンバ変数を追加してはならない。(コンパイル単位間でLovyanGFXクラスに不整合が生じる);
  /// このクラスにはinclude順依存の機能拡張メソッドのみ追加を許可する。;
  /// ( 例：ファイルシステム関連、HTTP関連 );
  template <class Base>
  class LGFX_FILESYSTEM_Support : public Base
  {
  public:

    using Base::drawBmp;
    using Base::drawJpg;
    using Base::drawPng;
    using Base::drawQoi;
    using Base::drawBmpFile;
    using Base::drawJpgFile;
    using Base::drawPngFile;
    using Base::drawQoiFile;
    using Base::loadFont;

#if defined (ARDUINO)

 #if defined (FS_H)

  #define LGFX_FUNCTION_GENERATOR(drawImg, draw_img) \
    template <typename T> \
    inline bool drawImg##File(T &fs, fs::File *file, int32_t x=0, int32_t y=0, int32_t maxWidth=0, int32_t maxHeight=0, int32_t offX=0, int32_t offY=0, float scale_x = 1.0f, float scale_y = 0.0f, datum_t datum = datum_t::top_left) \
    { \
      DataWrapperT<T> data { fs, file }; \
      bool res = this->draw_img(&data, x, y, maxWidth, maxHeight, offX, offY, scale_x, scale_y, datum); \
      data.close(); \
      return res; \
    } \
    inline bool drawImg(fs::File *file, int32_t x = 0, int32_t y = 0, int32_t maxWidth = 0, int32_t maxHeight = 0, int32_t offX = 0, int32_t offY = 0, float scale_x = 1.0f, float scale_y = 0.0f, datum_t datum = datum_t::top_left) \
    { \
      DataWrapperT<fs::File> data { file }; \
      return this->draw_img(&data, x, y, maxWidth, maxHeight, offX, offY, scale_x, scale_y, datum); \
    } \
    template <typename T> \
    inline bool drawImg##File(T &fs, const String& path, int32_t x = 0, int32_t y = 0, int32_t maxWidth = 0, int32_t maxHeight = 0, int32_t offX = 0, int32_t offY = 0, float scale_x = 1.0f, float scale_y = 0.0f, datum_t datum = datum_t::top_left) \
    { \
      return drawImg##File(fs, path.c_str(), x, y, maxWidth, maxHeight, offX, offY, scale_x, scale_y, datum); \
    }

    LGFX_FUNCTION_GENERATOR(drawBmp, draw_bmp)
    LGFX_FUNCTION_GENERATOR(drawJpg, draw_jpg)
    LGFX_FUNCTION_GENERATOR(drawPng, draw_png)
    LGFX_FUNCTION_GENERATOR(drawQoi, draw_qoi)

  #undef LGFX_FUNCTION_GENERATOR

    inline bool drawBmp(fs::FS &fs, const char *path, int32_t x = 0, int32_t y = 0, int32_t maxWidth = 0, int32_t maxHeight = 0, int32_t offX = 0, int32_t offY = 0, float scale_x = 1.0f, float scale_y = 0.0f, datum_t datum = datum_t::top_left)
    {
      return drawBmpFile(fs, path, x, y, maxWidth, maxHeight, offX, offY, scale_x, scale_y, datum);
    }
    [[deprecated("use float scale")]]
    inline bool drawJpgFile(fs::FS &fs, const char *path, int32_t x, int32_t y, int32_t maxWidth, int32_t maxHeight, int32_t offX, int32_t offY, jpeg_div::jpeg_div_t scale)
    {
      return drawJpgFile(fs, path, x, y, maxWidth, maxHeight, offX, offY, 1.0f / (1 << scale));
    }
    [[deprecated("use float scale")]]
    inline bool drawJpgFile(fs::FS &fs, fs::File *file, int32_t x, int32_t y, int32_t maxWidth, int32_t maxHeight, int32_t offX, int32_t offY, jpeg_div::jpeg_div_t scale)
    {
      return drawJpgFile(fs, file, x, y, maxWidth, maxHeight, offX, offY, 1.0f / (1 << scale));
    }
    [[deprecated("use float scale")]]
    inline bool drawJpg(fs::File *dataSource, int32_t x, int32_t y, int32_t maxWidth, int32_t maxHeight, int32_t offX, int32_t offY, jpeg_div::jpeg_div_t scale)
    {
      return drawJpg(dataSource, x, y, maxWidth, maxHeight, offX, offY, 1.0f / (1 << scale));
    }

 #endif
/*
 #if defined (__STORAGE_H__) // for SPRESENSE

    /// load vlw fontdata from filesystem.
    bool loadFont(const char *path, StorageClass &fs)
    {
      init_font_file<FileWrapper>(fs);
      return load_font_with_path(path);
    }

    bool loadFont(StorageClass &fs, const char *path)
    {
      init_font_file<FileWrapper>(fs);
      return load_font_with_path(path);
    }

  #define LGFX_FUNCTION_GENERATOR(drawImg, draw_img) \
    inline bool drawImg##File(StorageClass &fs, const char *path, int32_t x = 0, int32_t y = 0, int32_t maxWidth = 0, int32_t maxHeight = 0, int32_t offX = 0, int32_t offY = 0, float scale_x = 1.0f, float scale_y = 0.0f, datum_t datum = datum_t::top_left) \
    {  FileWrapper file(fs); \
       bool res = this->drawImg##File(&file, path, x, y, maxWidth, maxHeight, offX, offY, scale_x, scale_y, datum); \
       file.close(); \
       return res; \
    } \
    inline bool drawImg##File(StorageClass &fs, File *file, int32_t x=0, int32_t y=0, int32_t maxWidth=0, int32_t maxHeight=0, int32_t offX=0, int32_t offY=0, float scale_x = 1.0f, float scale_y = 0.0f, datum_t datum = datum_t::top_left) \
    { \
      FileWrapper data(fs, file); \
      bool res = this->draw_img(&data, x, y, maxWidth, maxHeight, offX, offY, scale_x, scale_y, datum); \
      data.close(); \
      return res; \
    } \
    inline bool drawImg##File(StorageClass &fs, const String& path, int32_t x = 0, int32_t y = 0, int32_t maxWidth = 0, int32_t maxHeight = 0, int32_t offX = 0, int32_t offY = 0, float scale_x = 1.0f, float scale_y = 0.0f, datum_t datum = datum_t::top_left) \
    { \
      return drawImg##File(fs, path.c_str(), x, y, maxWidth, maxHeight, offX, offY, scale_x, scale_y, datum); \
    } \
    inline bool drawImg(File *dataSource, int32_t x = 0, int32_t y = 0, int32_t maxWidth = 0, int32_t maxHeight = 0, int32_t offX = 0, int32_t offY = 0, float scale_x = 1.0f, float scale_y = 0.0f, datum_t datum = datum_t::top_left) \
    { \
      StreamWrapper data; \
      data.set(dataSource); \
      data.need_transaction = true; \
      return this->draw_img(&data, x, y, maxWidth, maxHeight, offX, offY, scale_x, scale_y, datum); \
    } \

    LGFX_FUNCTION_GENERATOR(drawBmp, draw_bmp)
    LGFX_FUNCTION_GENERATOR(drawJpg, draw_jpg)
    LGFX_FUNCTION_GENERATOR(drawPng, draw_png)
    LGFX_FUNCTION_GENERATOR(drawQoi, draw_qoi)

  #undef LGFX_FUNCTION_GENERATOR

    inline bool drawBmp(StorageClass &fs, const char *path, int32_t x = 0, int32_t y = 0, int32_t maxWidth = 0, int32_t maxHeight = 0, int32_t offX = 0, int32_t offY = 0, float scale_x = 1.0f, float scale_y = 0.0f, datum_t datum = datum_t::top_left)
    {
      return drawBmpFile(fs, path, x, y, maxWidth, maxHeight, offX, offY, scale_x, scale_y, datum);
    }

    [[deprecated("use float scale")]]
    inline bool drawJpgFile(StorageClass &fs, const char *path, int32_t x, int32_t y, int32_t maxWidth, int32_t maxHeight, int32_t offX, int32_t offY, jpeg_div::jpeg_div_t scale)
    {
      return drawJpgFile(fs, path, x, y, maxWidth, maxHeight, offX, offY, 1.0f / (1 << scale));
    }
    [[deprecated("use float scale")]]
    inline bool drawJpgFile(StorageClass &fs, File *file, int32_t x, int32_t y, int32_t maxWidth, int32_t maxHeight, int32_t offX, int32_t offY, jpeg_div::jpeg_div_t scale)
    {
      return drawJpgFile(fs, file, x, y, maxWidth, maxHeight, offX, offY, 1.0f / (1 << scale));
    }
    [[deprecated("use float scale")]]
    inline bool drawJpg(File *dataSource, int32_t x, int32_t y, int32_t maxWidth, int32_t maxHeight, int32_t offX, int32_t offY, jpeg_div::jpeg_div_t scale)
    {
      return drawJpg(dataSource, x, y, maxWidth, maxHeight, offX, offY, 1.0f / (1 << scale));
    }

 #endif
//*/
 #if defined (Stream_h) || defined ARDUINO_ARCH_RP2040 // RP2040 has no defines for builtin Stream API

  #define LGFX_FUNCTION_GENERATOR(drawImg, draw_img) \
    inline bool drawImg(Stream *dataSource, int32_t x = 0, int32_t y = 0, int32_t maxWidth = 0, int32_t maxHeight = 0, int32_t offX = 0, int32_t offY = 0, float scale_x = 1.0f, float scale_y = 0.0f, datum_t datum = datum_t::top_left) \
    { \
      StreamWrapper data; \
      data.set(dataSource); \
      data.need_transaction = this->isBusShared(); \
      return this->draw_img(&data, x, y, maxWidth, maxHeight, offX, offY, scale_x, scale_y, datum); \
    } \

    LGFX_FUNCTION_GENERATOR(drawBmp, draw_bmp)
    LGFX_FUNCTION_GENERATOR(drawJpg, draw_jpg)
    LGFX_FUNCTION_GENERATOR(drawPng, draw_png)
    LGFX_FUNCTION_GENERATOR(drawQoi, draw_qoi)

  #undef LGFX_FUNCTION_GENERATOR

    [[deprecated("use float scale")]]
    inline bool drawJpg(Stream *dataSource, int32_t x, int32_t y, int32_t maxWidth, int32_t maxHeight, int32_t offX, int32_t offY, jpeg_div::jpeg_div_t scale)
    {
      return drawJpg(dataSource, x, y, maxWidth, maxHeight, offX, offY, 1.0f / (1 << scale));
    }

  #if defined (HTTPClient_H_)

    struct HttpWrapper : public StreamWrapper
    {
      int read(uint8_t *buf, uint32_t len) override {
        while (_http.connected() && !_stream->available() && _index < _length) delay(1);
        return StreamWrapper::read(buf, len);
      }

      bool open(const char* url) {
        _http.begin(url);
        int httpCode = _http.GET();
        set(_http.getStreamPtr(), _http.getSize());
        if (httpCode == HTTP_CODE_OK) return true;

        log_e("HTTP ERROR: %d\n", httpCode);
        return false;
      }

      void close(void) override { _http.end(); }

    private:
      HTTPClient _http;
    };

  #define LGFX_FUNCTION_GENERATOR(drawImg) \
    inline bool drawImg##Url(const char* url, int32_t x = 0, int32_t y = 0, int32_t maxWidth = 0, int32_t maxHeight = 0, int32_t offX = 0, int32_t offY = 0, float scale_x = 1.0f, float scale_y = 0.0f, datum_t datum = datum_t::top_left) \
    { \
      HttpWrapper http; \
      return http.open(url) && drawImg(&http, x, y, maxWidth, maxHeight, offX, offY, scale_x, scale_y, datum); \
    } \
    inline bool drawImg##Url(const String& url, int32_t x = 0, int32_t y = 0, int32_t maxWidth = 0, int32_t maxHeight = 0, int32_t offX = 0, int32_t offY = 0, float scale_x = 1.0f, float scale_y = 0.0f, datum_t datum = datum_t::top_left) \
    { \
      return drawImg##Url(url.c_str(), x, y, maxWidth, maxHeight, offX, offY, scale_x, scale_y, datum); \
    } \

    LGFX_FUNCTION_GENERATOR(drawBmp)
    LGFX_FUNCTION_GENERATOR(drawJpg)
    LGFX_FUNCTION_GENERATOR(drawPng)
    LGFX_FUNCTION_GENERATOR(drawQoi)

  #undef LGFX_FUNCTION_GENERATOR

    [[deprecated("use float scale")]]
    inline bool drawJpgUrl(const char* url, int32_t x, int32_t y, int32_t maxWidth, int32_t maxHeight, int32_t offX, int32_t offY, jpeg_div::jpeg_div_t scale)
    {
      return drawJpgUrl(url, x, y, maxWidth, maxHeight, offX, offY, 1.0f / (1 << scale));
    }

  #endif
 #endif
/*
#elif defined (ESP_PLATFORM) || defined(__SAMD51_HARMONY__) || defined(stdin) // ESP-IDF, Harmony, stdio

  #define LGFX_FUNCTION_GENERATOR(drawImg) \
    inline bool drawImg##File(const char *path, int32_t x = 0, int32_t y = 0, int32_t maxWidth = 0, int32_t maxHeight = 0, int32_t offX = 0, int32_t offY = 0, float scale_x = 1.0f, float scale_y = 0.0f, datum_t datum = datum_t::top_left) \
    { \
      FileWrapper file; \
      return drawImg##File(&file, path, x, y, maxWidth, maxHeight, offX, offY, scale_x, scale_y, datum); \
    } \

    LGFX_FUNCTION_GENERATOR(drawBmp)
    LGFX_FUNCTION_GENERATOR(drawJpg)
    LGFX_FUNCTION_GENERATOR(drawPng)
    LGFX_FUNCTION_GENERATOR(drawQoi)

  #undef LGFX_FUNCTION_GENERATOR

    [[deprecated("use float scale")]]
    inline bool drawJpgFile(const char *path, int32_t x, int32_t y, int32_t maxWidth, int32_t maxHeight, int32_t offX, int32_t offY, jpeg_div::jpeg_div_t scale)
    {
      return drawJpgFile(path, x, y, maxWidth, maxHeight, offX, offY, 1.0f / (1 << scale));
    }

    bool loadFont(const char *path)
    {
      init_font_file<FileWrapper>();
      return load_font_with_path(path);
    }
//*/
#endif

#define LGFX_URL_MAXLENGTH 2083

#if defined ( _WINHTTPX_ )

    struct HttpWrapper : public DataWrapper
    {
      HttpWrapper(void) : DataWrapper()
      {
        hConnect = nullptr;
        hRequest = nullptr;
        hSession = WinHttpOpen( L"UserAgent/1.0"
                              , WINHTTP_ACCESS_TYPE_DEFAULT_PROXY
                              , WINHTTP_NO_PROXY_NAME
                              , WINHTTP_NO_PROXY_BYPASS, 0);
      }
      virtual ~HttpWrapper(void)
      {
        disconnect();
      }
      bool open(const char* url) override
      {
        _index = 0;
        _content_length = ~0u;
        dwStatusCode = 0;
        if (hSession == nullptr) { return false; }
        if (strlen(url) > LGFX_URL_MAXLENGTH) { return false; }
        if (hRequest) { WinHttpCloseHandle(hRequest); hRequest = nullptr; }

        WCHAR wchar_url[LGFX_URL_MAXLENGTH+1];
        size_t iReturnValue;
        if (0 != mbstowcs_s(&iReturnValue
                           , wchar_url
                           , sizeof(wchar_url) / sizeof(wchar_url[0])
                           , url
                           , _TRUNCATE
                           ))
        {
          return false;
        }

        WCHAR szHostName[256], szUrlPath[LGFX_URL_MAXLENGTH+1];
        URL_COMPONENTS urlComponents = { 0 };
        urlComponents.dwStructSize = sizeof(URL_COMPONENTS);
        urlComponents.lpszHostName = szHostName;
        urlComponents.dwHostNameLength = sizeof(szHostName) / sizeof(WCHAR);
        urlComponents.lpszUrlPath = szUrlPath;
        urlComponents.dwUrlPathLength = sizeof(szUrlPath) / sizeof(WCHAR);
        if (!WinHttpCrackUrl(wchar_url, wcslen(wchar_url), 0, &urlComponents))
        {
          WinHttpCloseHandle(hSession);
          return false;
        }


        if (hConnect == nullptr || wcscmp(szHostName, _last_host))
        {
          if (hConnect)
          {
            WinHttpCloseHandle(hConnect);
          }
          hConnect = WinHttpConnect(hSession, szHostName, urlComponents.nPort, 0);
          lstrcpynW(_last_host, szHostName, sizeof(_last_host) / sizeof(_last_host[0]));
        }

        if (hConnect != nullptr)
        {
          hRequest = WinHttpOpenRequest( hConnect
                                       , L"GET", szUrlPath
                                       , nullptr
                                       , WINHTTP_NO_REFERER
                                       , WINHTTP_DEFAULT_ACCEPT_TYPES
                                       , (INTERNET_SCHEME_HTTPS == urlComponents.nScheme) ? WINHTTP_FLAG_SECURE : 0);
          if (hRequest != nullptr)
          {
            if (WinHttpSendRequest( hRequest
                                  , L"Accept: */*\r\n"
                                  , -1L
                                  , WINHTTP_NO_REQUEST_DATA
                                  , 0
                                  , WINHTTP_IGNORE_REQUEST_TOTAL_LENGTH
                                  , 0
                                  ))
            {
              if (WinHttpReceiveResponse(hRequest, nullptr))
              {
                DWORD dwSize = 0;
                WinHttpQueryHeaders( hRequest
                                    , WINHTTP_QUERY_CONTENT_LENGTH | WINHTTP_QUERY_FLAG_NUMBER
                                    , WINHTTP_HEADER_NAME_BY_INDEX
                                    , &_content_length
                                    , &dwSize
                                    , WINHTTP_NO_HEADER_INDEX);
                WinHttpQueryHeaders( hRequest
                                    , WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER
                                    , WINHTTP_HEADER_NAME_BY_INDEX
                                    , &dwStatusCode
                                    , &dwSize
                                    , WINHTTP_NO_HEADER_INDEX);
                if (dwStatusCode == HTTP_STATUS_OK)
                {
                  return true;
                }
              }
            }
            WinHttpCloseHandle(hRequest);
            hRequest = nullptr;
          }
        }
        return false;
      }

      void close(void) override { if (hRequest) { WinHttpCloseHandle(hRequest); hRequest = nullptr; } }

      int read(uint8_t* buf, uint32_t len) override
      {
        if (len > _content_length - _index)
        {
          len = _content_length - _index;
        }
        if (len == 0) { return 0; }
        DWORD readbytes = 0;
        WinHttpReadData(hRequest, buf, len, &readbytes);
        _index += readbytes;
        return readbytes;
      }
      void skip(int32_t offset) override
      {
        if (0 >= offset) { return; }
        uint8_t dummy[64];
        size_t len = ((offset - 1) & 63) + 1;
        do
        {
          read(dummy, len);
          offset -= len;
          len = 64;
        } while (offset);
      }

      bool seek(uint32_t offset) { return false; }
      int32_t tell(void) { return _index; }

    protected:
      HINTERNET hSession, hConnect, hRequest;
      SOCKET _socket;
      WCHAR _last_host[256] = L"";
      int32_t _index = 0;
      DWORD _content_length = ~0u;
      DWORD dwStatusCode;

      void disconnect(void)
      {
        if (hRequest) { WinHttpCloseHandle(hRequest); hRequest = nullptr; }
        if (hConnect) { WinHttpCloseHandle(hConnect); hConnect = nullptr; }
        if (hSession) { WinHttpCloseHandle(hSession); hSession = nullptr; }
        _index = 0;
        _content_length = ~0u;
        dwStatusCode = 0;
      }

      void checkHeaderString(const char* str)
      {
        if (_http_code == 0        && memcmp(str, "HTTP/1.1 "        , 9) == 0) { _http_code      = atoi(&str[ 9]); }
        if (_content_length == ~0u && memcmp(str, "Content-Length: ", 16) == 0) { _content_length = atoi(&str[16]); }
      }

      void parseHttpHeader(void)
      {
        char buffer[257];

        int index = 0;
        int readlen;
        int limit = -1;
        while ((readlen = recv(_socket, &buffer[index], 1, 0)) > 0)
        {
          if (buffer[index] == '\r')
          {
            if (limit == -1)
            {
              limit = index;
              buffer[index] = 0;
              checkHeaderString(buffer);
            }
            index = 0;
          }
          else if (buffer[index] == '\n')
          {
            if (limit == 0) { return; }
            limit = -1;
          }
          else
          {
            index = (index + readlen) & 0xFF;
          }
        }
      }
    };

  #define LGFX_FUNCTION_GENERATOR(drawImg) \
    inline bool drawImg##Url(const char* url, int32_t x = 0, int32_t y = 0, int32_t maxWidth = 0, int32_t maxHeight = 0, int32_t offX = 0, int32_t offY = 0, float scale_x = 1.0f, float scale_y = 0.0f, datum_t datum = datum_t::top_left) \
    { \
      HttpWrapper http; \
      return http.open(url) && drawImg(&http, x, y, maxWidth, maxHeight, offX, offY, scale_x, scale_y, datum); \
    } \

    LGFX_FUNCTION_GENERATOR(drawBmp)
    LGFX_FUNCTION_GENERATOR(drawJpg)
    LGFX_FUNCTION_GENERATOR(drawPng)
    LGFX_FUNCTION_GENERATOR(drawQoi)

  #undef LGFX_FUNCTION_GENERATOR

#elif defined ( _WINSOCK2API_ )

    struct HttpWrapper : public DataWrapper
    {
      HttpWrapper(void) : DataWrapper()
      {
        WSADATA wsaData;
        _wsa_startup = (WSAStartup(MAKEWORD(2, 2), &wsaData) == 0);
      }
      virtual ~HttpWrapper(void)
      {
        disconnect();
      }
      bool open(const char* url) override
      {
        _index = 0;
        _content_length = ~0u;
        _http_code = 0;
        if (!_wsa_startup) { return false; }
        if (strlen(url) > LGFX_URL_MAXLENGTH) { return false; }
        const char* urlpart_host = strstr(url, "://");
        if (urlpart_host == nullptr) { return false; }
        urlpart_host += 3;
        const char* urlpart_path = strstr(urlpart_host, "/");
        if (urlpart_path == nullptr)
        {
          urlpart_path = &url[strlen(url)];
        }

        char* hostname = (char*)alloca(urlpart_path - urlpart_host + 1);
        memcpy(hostname, urlpart_host, urlpart_path - urlpart_host);
        hostname[urlpart_path - urlpart_host] = 0;
        if (!_connected || strcmp(hostname, _last_host))
        {
          if (_connected)
          {
            disconnect();
          }
          strncpy(_last_host, hostname, sizeof(_last_host));
          hostent* Host = gethostbyname(hostname);
          SOCKADDR_IN SockAddr;
          SockAddr.sin_port = htons(80);
          SockAddr.sin_family = AF_INET;
          SockAddr.sin_addr.s_addr = *((unsigned long*)Host->h_addr);

          _socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
          _connected = (connect(_socket, (SOCKADDR*)(&SockAddr), sizeof(SockAddr)) == 0);
        }
        if (_connected)
        {
          char* get_http = (char*)alloca(LGFX_URL_MAXLENGTH+1);
          memset(get_http, ' ', sizeof(get_http));
          strcpy(get_http, "GET ");
          if (urlpart_path[0] == 0)
          {
            strcat(get_http, "/");
          }
          else
          {
            strcat(get_http, urlpart_path);
          }
          strcat(get_http, " HTTP/1.1\r\nHost: ");
          strcat(get_http, hostname);
          strcat(get_http, "\r\nConnection: keep-alive\r\n\r\n");
          send(_socket, get_http, strlen(get_http), 0);

          parseHttpHeader();

          if (_http_code == 200)
          {
            return true;
          }
        }
        return false;
      }

      void close(void) override {}

      int read(uint8_t* buf, uint32_t len) override
      {
        if (len > _content_length - _index)
        {
          len = _content_length - _index;
        }
        if (len == 0) { return 0; }
        int32_t res = recv(_socket, (char*)buf, (int)len, 0);
        _index += res;
        return res;
      }
      void skip(int32_t offset) override
      {
        if (0 >= offset) { return; }
        uint8_t dummy[64];
        size_t len = ((offset - 1) & 63) + 1;
        do
        {
          read(dummy, len);
          offset -= len;
          len = 64;
        } while (offset);
      }

      bool seek(uint32_t offset) { return false; }
      int32_t tell(void) { return _index; }

    protected:
      bool _wsa_startup = false;
      bool _connected = false;
      SOCKET _socket;
      char _last_host[256] = "";
      int32_t _index = 0;
      int32_t _content_length = ~0u;
      int32_t _http_code = 0;

      void disconnect(void)
      {
        if (_connected) { closesocket(_socket); }
        _index = 0;
        _content_length = ~0u;
        _http_code = 0;
      }

      void checkHeaderString(const char* str)
      {
        if (_http_code == 0        && memcmp(str, "HTTP/1.1 "        , 9) == 0) { _http_code      = atoi(&str[ 9]); }
        if (_content_length == ~0u && memcmp(str, "Content-Length: ", 16) == 0) { _content_length = atoi(&str[16]); }
      }

      void parseHttpHeader(void)
      {
        char buffer[257];

        int index = 0;
        int readlen;
        int limit = -1;
        while ((readlen = recv(_socket, &buffer[index], 1, 0)) > 0)
        {
          if (buffer[index] == '\r')
          {
            if (limit == -1)
            {
              limit = index;
              buffer[index] = 0;
              checkHeaderString(buffer);
            }
            index = 0;
          }
          else if (buffer[index] == '\n')
          {
            if (limit == 0) { return; }
            limit = -1;
          }
          else
          {
            index = (index + readlen) & 0xFF;
          }
        }
      }
    };

  #define LGFX_FUNCTION_GENERATOR(drawImg) \
    inline bool drawImg##Url(const char* url, int32_t x = 0, int32_t y = 0, int32_t maxWidth = 0, int32_t maxHeight = 0, int32_t offX = 0, int32_t offY = 0, float scale_x = 1.0f, float scale_y = 0.0f, datum_t datum = datum_t::top_left) \
    { \
      HttpWrapper http; \
      return http.open(url) && drawImg(&http, x, y, maxWidth, maxHeight, offX, offY, scale_x, scale_y, datum); \
    } \

    LGFX_FUNCTION_GENERATOR(drawBmp)
    LGFX_FUNCTION_GENERATOR(drawJpg)
    LGFX_FUNCTION_GENERATOR(drawPng)
    LGFX_FUNCTION_GENERATOR(drawQoi)

  #undef LGFX_FUNCTION_GENERATOR

#endif

#undef LGFX_URL_MAXLENGTH

  private:

    template<typename T, typename Tfs>
    void init_font_file(Tfs &fs)
    {
      this->unloadFont();
      this->_font_file.reset(new T(fs));
    }

    template<typename T>
    void init_font_file(void)
    {
      this->unloadFont();
      this->_font_file.reset(new T());
    }
  };

//----------------------------------------------------------------------------
 }
}

#endif
