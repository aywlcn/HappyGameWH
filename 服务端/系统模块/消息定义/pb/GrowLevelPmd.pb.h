// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: GrowLevelPmd.proto

#ifndef PROTOBUF_GrowLevelPmd_2eproto__INCLUDED
#define PROTOBUF_GrowLevelPmd_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 3002000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 3002000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/metadata.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/unknown_field_set.h>
#include "NullPmd.pb.h"
// @@protoc_insertion_point(includes)
namespace GrowLevelPmd {
class tagGrowLevelConfig;
class tagGrowLevelConfigDefaultTypeInternal;
extern tagGrowLevelConfigDefaultTypeInternal _tagGrowLevelConfig_default_instance_;
class tagGrowLevelConfig_s2c;
class tagGrowLevelConfig_s2cDefaultTypeInternal;
extern tagGrowLevelConfig_s2cDefaultTypeInternal _tagGrowLevelConfig_s2c_default_instance_;
}  // namespace GrowLevelPmd
namespace NullPmd {
class command;
class commandDefaultTypeInternal;
extern commandDefaultTypeInternal _command_default_instance_;
class head;
class headDefaultTypeInternal;
extern headDefaultTypeInternal _head_default_instance_;
class indication;
class indicationDefaultTypeInternal;
extern indicationDefaultTypeInternal _indication_default_instance_;
class info;
class infoDefaultTypeInternal;
extern infoDefaultTypeInternal _info_default_instance_;
class message;
class messageDefaultTypeInternal;
extern messageDefaultTypeInternal _message_default_instance_;
class request;
class requestDefaultTypeInternal;
extern requestDefaultTypeInternal _request_default_instance_;
class response;
class responseDefaultTypeInternal;
extern responseDefaultTypeInternal _response_default_instance_;
}  // namespace NullPmd

namespace GrowLevelPmd {

namespace protobuf_GrowLevelPmd_2eproto {
// Internal implementation detail -- do not call these.
struct TableStruct {
  static const ::google::protobuf::uint32 offsets[];
  static void InitDefaultsImpl();
  static void Shutdown();
};
void AddDescriptors();
void InitDefaults();
}  // namespace protobuf_GrowLevelPmd_2eproto

// ===================================================================

class tagGrowLevelConfig : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:GrowLevelPmd.tagGrowLevelConfig) */ {
 public:
  tagGrowLevelConfig();
  virtual ~tagGrowLevelConfig();

  tagGrowLevelConfig(const tagGrowLevelConfig& from);

  inline tagGrowLevelConfig& operator=(const tagGrowLevelConfig& from) {
    CopyFrom(from);
    return *this;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const tagGrowLevelConfig& default_instance();

  static inline const tagGrowLevelConfig* internal_default_instance() {
    return reinterpret_cast<const tagGrowLevelConfig*>(
               &_tagGrowLevelConfig_default_instance_);
  }

  void Swap(tagGrowLevelConfig* other);

  // implements Message ----------------------------------------------

  inline tagGrowLevelConfig* New() const PROTOBUF_FINAL { return New(NULL); }

  tagGrowLevelConfig* New(::google::protobuf::Arena* arena) const PROTOBUF_FINAL;
  void CopyFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void MergeFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void CopyFrom(const tagGrowLevelConfig& from);
  void MergeFrom(const tagGrowLevelConfig& from);
  void Clear() PROTOBUF_FINAL;
  bool IsInitialized() const PROTOBUF_FINAL;

  size_t ByteSizeLong() const PROTOBUF_FINAL;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) PROTOBUF_FINAL;
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const PROTOBUF_FINAL;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* target) const PROTOBUF_FINAL;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output)
      const PROTOBUF_FINAL {
    return InternalSerializeWithCachedSizesToArray(
        ::google::protobuf::io::CodedOutputStream::IsDefaultSerializationDeterministic(), output);
  }
  int GetCachedSize() const PROTOBUF_FINAL { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const PROTOBUF_FINAL;
  void InternalSwap(tagGrowLevelConfig* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return NULL;
  }
  inline void* MaybeArenaPtr() const {
    return NULL;
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const PROTOBUF_FINAL;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // .NullPmd.response respcmd = 1;
  bool has_respcmd() const;
  void clear_respcmd();
  static const int kRespcmdFieldNumber = 1;
  const ::NullPmd::response& respcmd() const;
  ::NullPmd::response* mutable_respcmd();
  ::NullPmd::response* release_respcmd();
  void set_allocated_respcmd(::NullPmd::response* respcmd);

  // uint64 dwexperience = 3;
  void clear_dwexperience();
  static const int kDwexperienceFieldNumber = 3;
  ::google::protobuf::uint64 dwexperience() const;
  void set_dwexperience(::google::protobuf::uint64 value);

  // int32 wlevelid = 2;
  void clear_wlevelid();
  static const int kWlevelidFieldNumber = 2;
  ::google::protobuf::int32 wlevelid() const;
  void set_wlevelid(::google::protobuf::int32 value);

  // @@protoc_insertion_point(class_scope:GrowLevelPmd.tagGrowLevelConfig)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::NullPmd::response* respcmd_;
  ::google::protobuf::uint64 dwexperience_;
  ::google::protobuf::int32 wlevelid_;
  mutable int _cached_size_;
  friend struct  protobuf_GrowLevelPmd_2eproto::TableStruct;
};
// -------------------------------------------------------------------

class tagGrowLevelConfig_s2c : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:GrowLevelPmd.tagGrowLevelConfig_s2c) */ {
 public:
  tagGrowLevelConfig_s2c();
  virtual ~tagGrowLevelConfig_s2c();

  tagGrowLevelConfig_s2c(const tagGrowLevelConfig_s2c& from);

  inline tagGrowLevelConfig_s2c& operator=(const tagGrowLevelConfig_s2c& from) {
    CopyFrom(from);
    return *this;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const tagGrowLevelConfig_s2c& default_instance();

  static inline const tagGrowLevelConfig_s2c* internal_default_instance() {
    return reinterpret_cast<const tagGrowLevelConfig_s2c*>(
               &_tagGrowLevelConfig_s2c_default_instance_);
  }

  void Swap(tagGrowLevelConfig_s2c* other);

  // implements Message ----------------------------------------------

  inline tagGrowLevelConfig_s2c* New() const PROTOBUF_FINAL { return New(NULL); }

  tagGrowLevelConfig_s2c* New(::google::protobuf::Arena* arena) const PROTOBUF_FINAL;
  void CopyFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void MergeFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void CopyFrom(const tagGrowLevelConfig_s2c& from);
  void MergeFrom(const tagGrowLevelConfig_s2c& from);
  void Clear() PROTOBUF_FINAL;
  bool IsInitialized() const PROTOBUF_FINAL;

  size_t ByteSizeLong() const PROTOBUF_FINAL;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) PROTOBUF_FINAL;
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const PROTOBUF_FINAL;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* target) const PROTOBUF_FINAL;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output)
      const PROTOBUF_FINAL {
    return InternalSerializeWithCachedSizesToArray(
        ::google::protobuf::io::CodedOutputStream::IsDefaultSerializationDeterministic(), output);
  }
  int GetCachedSize() const PROTOBUF_FINAL { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const PROTOBUF_FINAL;
  void InternalSwap(tagGrowLevelConfig_s2c* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return NULL;
  }
  inline void* MaybeArenaPtr() const {
    return NULL;
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const PROTOBUF_FINAL;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // repeated .GrowLevelPmd.tagGrowLevelConfig growlevelitem = 3;
  int growlevelitem_size() const;
  void clear_growlevelitem();
  static const int kGrowlevelitemFieldNumber = 3;
  const ::GrowLevelPmd::tagGrowLevelConfig& growlevelitem(int index) const;
  ::GrowLevelPmd::tagGrowLevelConfig* mutable_growlevelitem(int index);
  ::GrowLevelPmd::tagGrowLevelConfig* add_growlevelitem();
  ::google::protobuf::RepeatedPtrField< ::GrowLevelPmd::tagGrowLevelConfig >*
      mutable_growlevelitem();
  const ::google::protobuf::RepeatedPtrField< ::GrowLevelPmd::tagGrowLevelConfig >&
      growlevelitem() const;

  // .NullPmd.response respcmd = 1;
  bool has_respcmd() const;
  void clear_respcmd();
  static const int kRespcmdFieldNumber = 1;
  const ::NullPmd::response& respcmd() const;
  ::NullPmd::response* mutable_respcmd();
  ::NullPmd::response* release_respcmd();
  void set_allocated_respcmd(::NullPmd::response* respcmd);

  // int32 wlevelcount = 2;
  void clear_wlevelcount();
  static const int kWlevelcountFieldNumber = 2;
  ::google::protobuf::int32 wlevelcount() const;
  void set_wlevelcount(::google::protobuf::int32 value);

  // @@protoc_insertion_point(class_scope:GrowLevelPmd.tagGrowLevelConfig_s2c)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::google::protobuf::RepeatedPtrField< ::GrowLevelPmd::tagGrowLevelConfig > growlevelitem_;
  ::NullPmd::response* respcmd_;
  ::google::protobuf::int32 wlevelcount_;
  mutable int _cached_size_;
  friend struct  protobuf_GrowLevelPmd_2eproto::TableStruct;
};
// ===================================================================


// ===================================================================

#if !PROTOBUF_INLINE_NOT_IN_HEADERS
// tagGrowLevelConfig

// .NullPmd.response respcmd = 1;
inline bool tagGrowLevelConfig::has_respcmd() const {
  return this != internal_default_instance() && respcmd_ != NULL;
}
inline void tagGrowLevelConfig::clear_respcmd() {
  if (GetArenaNoVirtual() == NULL && respcmd_ != NULL) delete respcmd_;
  respcmd_ = NULL;
}
inline const ::NullPmd::response& tagGrowLevelConfig::respcmd() const {
  // @@protoc_insertion_point(field_get:GrowLevelPmd.tagGrowLevelConfig.respcmd)
  return respcmd_ != NULL ? *respcmd_
                         : *::NullPmd::response::internal_default_instance();
}
inline ::NullPmd::response* tagGrowLevelConfig::mutable_respcmd() {
  
  if (respcmd_ == NULL) {
    respcmd_ = new ::NullPmd::response;
  }
  // @@protoc_insertion_point(field_mutable:GrowLevelPmd.tagGrowLevelConfig.respcmd)
  return respcmd_;
}
inline ::NullPmd::response* tagGrowLevelConfig::release_respcmd() {
  // @@protoc_insertion_point(field_release:GrowLevelPmd.tagGrowLevelConfig.respcmd)
  
  ::NullPmd::response* temp = respcmd_;
  respcmd_ = NULL;
  return temp;
}
inline void tagGrowLevelConfig::set_allocated_respcmd(::NullPmd::response* respcmd) {
  delete respcmd_;
  respcmd_ = respcmd;
  if (respcmd) {
    
  } else {
    
  }
  // @@protoc_insertion_point(field_set_allocated:GrowLevelPmd.tagGrowLevelConfig.respcmd)
}

// int32 wlevelid = 2;
inline void tagGrowLevelConfig::clear_wlevelid() {
  wlevelid_ = 0;
}
inline ::google::protobuf::int32 tagGrowLevelConfig::wlevelid() const {
  // @@protoc_insertion_point(field_get:GrowLevelPmd.tagGrowLevelConfig.wlevelid)
  return wlevelid_;
}
inline void tagGrowLevelConfig::set_wlevelid(::google::protobuf::int32 value) {
  
  wlevelid_ = value;
  // @@protoc_insertion_point(field_set:GrowLevelPmd.tagGrowLevelConfig.wlevelid)
}

// uint64 dwexperience = 3;
inline void tagGrowLevelConfig::clear_dwexperience() {
  dwexperience_ = GOOGLE_ULONGLONG(0);
}
inline ::google::protobuf::uint64 tagGrowLevelConfig::dwexperience() const {
  // @@protoc_insertion_point(field_get:GrowLevelPmd.tagGrowLevelConfig.dwexperience)
  return dwexperience_;
}
inline void tagGrowLevelConfig::set_dwexperience(::google::protobuf::uint64 value) {
  
  dwexperience_ = value;
  // @@protoc_insertion_point(field_set:GrowLevelPmd.tagGrowLevelConfig.dwexperience)
}

// -------------------------------------------------------------------

// tagGrowLevelConfig_s2c

// .NullPmd.response respcmd = 1;
inline bool tagGrowLevelConfig_s2c::has_respcmd() const {
  return this != internal_default_instance() && respcmd_ != NULL;
}
inline void tagGrowLevelConfig_s2c::clear_respcmd() {
  if (GetArenaNoVirtual() == NULL && respcmd_ != NULL) delete respcmd_;
  respcmd_ = NULL;
}
inline const ::NullPmd::response& tagGrowLevelConfig_s2c::respcmd() const {
  // @@protoc_insertion_point(field_get:GrowLevelPmd.tagGrowLevelConfig_s2c.respcmd)
  return respcmd_ != NULL ? *respcmd_
                         : *::NullPmd::response::internal_default_instance();
}
inline ::NullPmd::response* tagGrowLevelConfig_s2c::mutable_respcmd() {
  
  if (respcmd_ == NULL) {
    respcmd_ = new ::NullPmd::response;
  }
  // @@protoc_insertion_point(field_mutable:GrowLevelPmd.tagGrowLevelConfig_s2c.respcmd)
  return respcmd_;
}
inline ::NullPmd::response* tagGrowLevelConfig_s2c::release_respcmd() {
  // @@protoc_insertion_point(field_release:GrowLevelPmd.tagGrowLevelConfig_s2c.respcmd)
  
  ::NullPmd::response* temp = respcmd_;
  respcmd_ = NULL;
  return temp;
}
inline void tagGrowLevelConfig_s2c::set_allocated_respcmd(::NullPmd::response* respcmd) {
  delete respcmd_;
  respcmd_ = respcmd;
  if (respcmd) {
    
  } else {
    
  }
  // @@protoc_insertion_point(field_set_allocated:GrowLevelPmd.tagGrowLevelConfig_s2c.respcmd)
}

// int32 wlevelcount = 2;
inline void tagGrowLevelConfig_s2c::clear_wlevelcount() {
  wlevelcount_ = 0;
}
inline ::google::protobuf::int32 tagGrowLevelConfig_s2c::wlevelcount() const {
  // @@protoc_insertion_point(field_get:GrowLevelPmd.tagGrowLevelConfig_s2c.wlevelcount)
  return wlevelcount_;
}
inline void tagGrowLevelConfig_s2c::set_wlevelcount(::google::protobuf::int32 value) {
  
  wlevelcount_ = value;
  // @@protoc_insertion_point(field_set:GrowLevelPmd.tagGrowLevelConfig_s2c.wlevelcount)
}

// repeated .GrowLevelPmd.tagGrowLevelConfig growlevelitem = 3;
inline int tagGrowLevelConfig_s2c::growlevelitem_size() const {
  return growlevelitem_.size();
}
inline void tagGrowLevelConfig_s2c::clear_growlevelitem() {
  growlevelitem_.Clear();
}
inline const ::GrowLevelPmd::tagGrowLevelConfig& tagGrowLevelConfig_s2c::growlevelitem(int index) const {
  // @@protoc_insertion_point(field_get:GrowLevelPmd.tagGrowLevelConfig_s2c.growlevelitem)
  return growlevelitem_.Get(index);
}
inline ::GrowLevelPmd::tagGrowLevelConfig* tagGrowLevelConfig_s2c::mutable_growlevelitem(int index) {
  // @@protoc_insertion_point(field_mutable:GrowLevelPmd.tagGrowLevelConfig_s2c.growlevelitem)
  return growlevelitem_.Mutable(index);
}
inline ::GrowLevelPmd::tagGrowLevelConfig* tagGrowLevelConfig_s2c::add_growlevelitem() {
  // @@protoc_insertion_point(field_add:GrowLevelPmd.tagGrowLevelConfig_s2c.growlevelitem)
  return growlevelitem_.Add();
}
inline ::google::protobuf::RepeatedPtrField< ::GrowLevelPmd::tagGrowLevelConfig >*
tagGrowLevelConfig_s2c::mutable_growlevelitem() {
  // @@protoc_insertion_point(field_mutable_list:GrowLevelPmd.tagGrowLevelConfig_s2c.growlevelitem)
  return &growlevelitem_;
}
inline const ::google::protobuf::RepeatedPtrField< ::GrowLevelPmd::tagGrowLevelConfig >&
tagGrowLevelConfig_s2c::growlevelitem() const {
  // @@protoc_insertion_point(field_list:GrowLevelPmd.tagGrowLevelConfig_s2c.growlevelitem)
  return growlevelitem_;
}

#endif  // !PROTOBUF_INLINE_NOT_IN_HEADERS
// -------------------------------------------------------------------


// @@protoc_insertion_point(namespace_scope)


}  // namespace GrowLevelPmd

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_GrowLevelPmd_2eproto__INCLUDED
