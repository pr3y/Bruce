/*----------------------------------------------------------------------------/
  Lovyan GFX - Graphics library for embedded devices.

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
#pragma once

#if defined ( ARDUINO )
 #include <Arduino.h>
#endif

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "../../utility/pgmspace.h"

namespace lgfx
{
 inline namespace v1
 {

#if defined ( _MSVC_LANG )
 #define LGFX_INLINE inline
#else
 #define LGFX_INLINE __attribute__ ((always_inline)) inline
#endif

//----------------------------------------------------------------------------
  class LGFXBase;

  struct DataWrapper
  {
    constexpr DataWrapper(void) = default;
    virtual ~DataWrapper(void) = default;

    uint8_t read8(void)
    {
      uint8_t result;
      read(&result, 1);
      return result;
    }

    uint16_t read16(void)
    {
      uint16_t result;
      read(reinterpret_cast<uint8_t*>(&result), 2);
      return result;
    }

    uint32_t read32(void) {
      uint32_t result;
      read(reinterpret_cast<uint8_t*>(&result), 4);
      return result;
    }

    LGFX_INLINE uint16_t read16swap(void) { auto r = read16(); return (r<<8)+(r>>8); }
    LGFX_INLINE uint32_t read32swap(void) { auto r = read32(); return r = (r >> 16) + (r << 16); return ((r >> 8) & 0xFF00FF) + ((r & 0xFF00FF) << 8); }

    virtual bool open(const char* path) { (void)path;  return true; };
    virtual int read(uint8_t *buf, uint32_t len) = 0;
    virtual int read(uint8_t *buf, uint32_t maximum_len, uint32_t required_len) { (void)required_len; return read(buf, maximum_len); }
    virtual void skip(int32_t offset) = 0;
    virtual bool seek(uint32_t offset) = 0;
    virtual void close(void) = 0;
    virtual int32_t tell(void) = 0;

    LGFX_INLINE void preRead(void) { if (fp_pre_read) fp_pre_read(parent); }
    LGFX_INLINE void postRead(void) { if (fp_post_read) fp_post_read(parent); }
    LGFX_INLINE bool hasParent(void) const { return parent; }
    LGFXBase* parent = nullptr;
    void (*fp_pre_read )(LGFXBase*) = nullptr;
    void (*fp_post_read)(LGFXBase*) = nullptr;
    bool need_transaction = false;
  };

//----------------------------------------------------------------------------

  template <typename T>
  struct DataWrapperT : public DataWrapper {
    DataWrapperT(void) : DataWrapper() {}
    DataWrapperT(void*) : DataWrapper() {}
  };

  struct DataWrapperFactory {
    virtual DataWrapper* create(void) = 0;
    virtual ~DataWrapperFactory() {};
  };

//----------------------------------------------------------------------------

#if defined (__FILE_defined) || defined (_FILE_DEFINED) || defined (_FSTDIO)
  template <>
  struct DataWrapperT<FILE> : public DataWrapper
  {
    DataWrapperT(FILE* fp = nullptr) : DataWrapper() , _fp { fp }
    {
      need_transaction = true;
    }
#if defined (__STDC_WANT_SECURE_LIB__)
    bool open(const char* path) override {
      while (0 != fopen_s(&_fp, path, "rb") && path[0] == '/')
      { ++path; }
      return _fp;
    }
#else
    bool open(const char* path) override {
      while (nullptr == (_fp = fopen(path, "rb")) && path[0] == '/')
      { ++path; }
      return _fp;
    }
#endif
    int read(uint8_t *buf, uint32_t len) override { return fread((char*)buf, 1, len, _fp); }
    void skip(int32_t offset) override { seek(offset, SEEK_CUR); }
    bool seek(uint32_t offset) override { return seek(offset, SEEK_SET); }
    bool seek(uint32_t offset, int origin) { return fseek(_fp, offset, origin); }
    void close(void) override { if (_fp) { fclose(_fp); _fp = nullptr; } }
    int32_t tell(void) override { return ftell(_fp); }
  protected:
    FILE* _fp;
  };

  template <>
  struct DataWrapperT<void> : public DataWrapperT<FILE>
  {
    DataWrapperT(void) : DataWrapperT<FILE>() {}
  };
#else
  template <>
  struct DataWrapperT<void> : public DataWrapper
  {
    DataWrapperT(void) : DataWrapper() { }
    int read(uint8_t *buf, uint32_t len) override { return false; }
    void skip(int32_t offset) override { }
    bool seek(uint32_t offset) override { return false; }
    bool seek(uint32_t offset, int origin) { return false; }
    void close(void) override { }
    int32_t tell(void) override { return 0; }
  };
#endif

//----------------------------------------------------------------------------

  struct PointerWrapper : public DataWrapper
  {
    PointerWrapper() : DataWrapper{} , _ptr { nullptr }, _index { 0 }, _length { 0 } {};
    PointerWrapper(const uint8_t* src, uint32_t length = ~0) : DataWrapper{}, _ptr { src }, _index { 0 }, _length { length } {}
    void set(const uint8_t* src, uint32_t length = ~0) { _ptr = src; _length = length; _index = 0; }
    int read(uint8_t *buf, uint32_t len) override {
      if (len > _length - _index) { len = _length - _index; }
      memcpy_P(buf, &_ptr[_index], len);
      _index += len;
      return len;
    }
    void skip(int32_t offset) override { _index += offset; }
    bool seek(uint32_t offset) override { _index = offset; return true; }
    void close(void) override { }
    int32_t tell(void) override { return _index; }

  protected:
    const uint8_t* _ptr;
    uint32_t _index;
    uint32_t _length;
  };

//----------------------------------------------------------------------------

#if defined (SdFat_h)
  // #if SD_FAT_VERSION >= 20102
  //  #define LGFX_SDFAT_TYPE SdBase<FsVolume,FsFormatter>
  // #else
  //  #define LGFX_SDFAT_TYPE SdBase<FsVolume>
  // #endif

  template <typename TFile>
  struct DataWrapperT_SdFatFile : public DataWrapper
  {
    DataWrapperT_SdFatFile(TFile* fp = nullptr) : DataWrapper{}, _fp { fp } { need_transaction = true; }
    int read(uint8_t *buf, uint32_t len) override { uint32_t a = _fp->available(); return _fp->read(buf, a < len ? a : len); }
    void skip(int32_t offset) override { _fp->seekCur(offset); }
    bool seek(uint32_t offset) override { return _fp->seekSet(offset); }
    void close(void) override { if (_fp) { _fp->close(); _fp = nullptr; } }
    int32_t tell(void) override { return _fp->position(); }
  protected:
    TFile *_fp;
  };

  template <>
  struct DataWrapperT<FsFile> : public DataWrapperT_SdFatFile<FsFile> {
    DataWrapperT(FsFile* fp = nullptr) : DataWrapperT_SdFatFile<FsFile>(fp) {}
  };

  template <>
  struct DataWrapperT<ExFile> : public DataWrapperT_SdFatFile<ExFile> {
    DataWrapperT(ExFile* fp = nullptr) : DataWrapperT_SdFatFile<ExFile>(fp) {}
  };

  template <>
  struct DataWrapperT<File32> : public DataWrapperT_SdFatFile<File32> {
    DataWrapperT(File32* fp = nullptr) : DataWrapperT_SdFatFile<File32>(fp) {}
  };

  template <typename TFS, typename TFile>
  struct DataWrapperT_SdFatFS : public DataWrapperT_SdFatFile<TFile>
  {
    DataWrapperT_SdFatFS(TFS *fs, TFile* fp = nullptr) : DataWrapperT_SdFatFile<TFile>{ fp }, _fs { fs } {}
    bool open(const char* path) override
    {
      _file = _fs->open(path, O_RDONLY);
      DataWrapperT_SdFatFile<TFile>::_fp = &_file;
      return _file;
    }
  protected:
    TFS *_fs;
    TFile _file;
  };

  template <>
  struct DataWrapperT<SdFs> : public DataWrapperT_SdFatFS<SdFs, FsFile> {
    DataWrapperT(SdFs* fs, FsFile* fp = nullptr) : DataWrapperT_SdFatFS<SdFs, FsFile>(fs, fp) {}
  };

  template <>
  struct DataWrapperT<SdExFat> : public DataWrapperT_SdFatFS<SdExFat, ExFile> {
    DataWrapperT(SdExFat* fs, ExFile* fp = nullptr) : DataWrapperT_SdFatFS<SdExFat, ExFile>(fs, fp) {}
  };

  template <>
  struct DataWrapperT<SdFat32> : public DataWrapperT_SdFatFS<SdFat32, File32> {
    DataWrapperT(SdFat32* fs, File32* fp = nullptr) : DataWrapperT_SdFatFS<SdFat32, File32>(fs, fp) {}
  };

  struct SdFatWrapper : public DataWrapperT<SdFs>
  {
    SdFatWrapper(SdFs &fs, FsFile* fp = nullptr) : DataWrapperT<SdFs> ( &fs, fp ) {}
    SdFatWrapper(SdFs *fs, FsFile* fp = nullptr) : DataWrapperT<SdFs> ( fs, fp ) {}
  };

//  #undef LGFX_SDFAT_TYPE

#endif

//----------------------------------------------------------------------------

#if ( defined (ARDUINO) && defined (Stream_h) ) || defined (ARDUINO_ARCH_RP2040) // RP2040 has no defines for builtin Stream API

  struct StreamWrapper : public DataWrapper
  {
    void set(Stream* src, uint32_t length = ~0u) { _stream = src; _length = length; _index = 0; }

    int read(uint8_t *buf, uint32_t len) override
    {
      if (len > _length - _index) { len = _length - _index; }
      if (len == 0) { return 0; }
      len = _stream->readBytes(buf, len);
      _index += len;
      return len;
    }

    int read(uint8_t *buf, uint32_t maximum_len, uint32_t required_len) override
    {
      uint32_t len = maximum_len;
      if (len > _length - _index) { len = _length - _index; }
      if (len == 0) { return 0; }

      int32_t tmp = _stream->available();
      if (0 < tmp && (len > (uint32_t)tmp)) { len = tmp; }
      if (len < required_len) { len = required_len; }
      len = _stream->readBytes(buf, len);
      _index += len;
      return len;
    }

    void skip(int32_t offset) override
    {
      if (0 >= offset) { return; }
      _index += offset;
      char dummy[64];
      size_t len = ((offset - 1) & 63) + 1;
      do
      {
        _stream->readBytes(dummy, len);
        offset -= len;
        len = 64;
      } while (offset);
    }
    bool seek(uint32_t offset) override { if (offset < _index) { return false; } skip(offset - _index); return true; }
    void close() override { }
    int32_t tell(void) override { return _index; }

  protected:
    Stream* _stream;
    uint32_t _index;
    uint32_t _length = 0;
  };

#endif

//----------------------------------------------------------------------------

  template <typename T>
  struct DataWrapperTFactoryT : public DataWrapperFactory {
    DataWrapperTFactoryT(T* fs) : _fs { fs } {}
    DataWrapperTFactoryT(T& fs) : _fs { &fs } {}
    DataWrapper* create(void) override { return new DataWrapperT<T>(_fs); }
  protected:
    T* _fs;
  };

  template <>
  struct DataWrapperTFactoryT<void> : public DataWrapperFactory {
    DataWrapperTFactoryT(void) {}
    DataWrapperTFactoryT(void*) {}
    DataWrapper* create(void) override { return new DataWrapperT<void>(); }
  };

//----------------------------------------------------------------------------

#undef LGFX_INLINE

//----------------------------------------------------------------------------

 }
}
