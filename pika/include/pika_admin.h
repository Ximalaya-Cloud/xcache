// Copyright (c) 2015-present, Qihoo, Inc.  All rights reserved.
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree. An additional grant
// of patent rights can be found in the PATENTS file in the same directory.

#ifndef PIKA_ADMIN_H_
#define PIKA_ADMIN_H_

#include "pika_command.h"
#include "pika_client_conn.h"
#include "blackwidow/blackwidow.h"

/*
 * Admin
 */
class SlaveofCmd : public Cmd {
public:
  SlaveofCmd() : is_noone_(false), have_offset_(false),
  filenum_(0), pro_offset_(0) {
  }
  virtual void Do();
private:
  std::string master_ip_;
  int64_t master_port_;
  bool is_noone_;
  bool have_offset_;
  int64_t filenum_;
  int64_t pro_offset_;
  virtual void DoInitial(const PikaCmdArgsType &argvs, const CmdInfo* const ptr_info);
  virtual void Clear() {
    is_noone_ = false;
    have_offset_ = false;
  }
};

class TrysyncCmd : public Cmd {
public:
  TrysyncCmd() {
  }
  virtual void Do();
private:
  std::string slave_ip_;
  int64_t slave_port_;
  int64_t filenum_;
  int64_t pro_offset_;
  virtual void DoInitial(const PikaCmdArgsType &argvs, const CmdInfo* const ptr_info);
};

class AuthCmd : public Cmd {
public:
  AuthCmd() {
  }
  virtual void Do();
private:
  std::string pwd_;
  virtual void DoInitial(const PikaCmdArgsType &argvs, const CmdInfo* const ptr_info);
};

class BgsaveCmd : public Cmd {
public:
  BgsaveCmd() {
  }
  virtual void Do();
private:
  virtual void DoInitial(const PikaCmdArgsType &argvs, const CmdInfo* const ptr_info);
};

class BgsaveoffCmd : public Cmd {
public:
  BgsaveoffCmd() {
  }
  virtual void Do();
private:
  virtual void DoInitial(const PikaCmdArgsType &argvs, const CmdInfo* const ptr_info);
};

class CompactCmd : public Cmd {
public:
  CompactCmd() {
  }
  virtual void Do();
private:
  virtual void DoInitial(const PikaCmdArgsType &argvs, const CmdInfo* const ptr_info);
};

class PurgelogstoCmd : public Cmd {
public:
  PurgelogstoCmd() : num_(0){
  }
  virtual void Do();
private:
  uint32_t num_;
  virtual void DoInitial(const PikaCmdArgsType &argvs, const CmdInfo* const ptr_info);
};

class PingCmd : public Cmd {
public:
  PingCmd() {
  }
  virtual void Do();
private:
  virtual void DoInitial(const PikaCmdArgsType &argvs, const CmdInfo* const ptr_info);
};

class SelectCmd : public Cmd {
public:
  SelectCmd() {
  }
  virtual void Do();
private:
  virtual void DoInitial(const PikaCmdArgsType &argvs, const CmdInfo* const ptr_info);
};

class FlushallCmd : public Cmd {
public:
  FlushallCmd() {
  }
  virtual void Do();
  virtual void CacheDo();
  virtual void PostDo();
private:
  virtual void DoInitial(const PikaCmdArgsType &argvs, const CmdInfo* const ptr_info);
};

class ReadonlyCmd : public Cmd {
public:
  ReadonlyCmd() : is_open_(false) {
  }
  virtual void Do();
private:
  bool is_open_;
  virtual void DoInitial(const PikaCmdArgsType &argvs, const CmdInfo* const ptr_info);
};

class ClientCmd : public Cmd {
public:
  ClientCmd() {
  }
  virtual void Do();
  const static std::string CLIENT_LIST_S;
  const static std::string CLIENT_KILL_S;
private:
  std::string operation_, ip_port_;
  virtual void DoInitial(const PikaCmdArgsType &argvs, const CmdInfo* const ptr_info);
};

class InfoCmd : public Cmd {
public:
  enum InfoSection {
    kInfoErr = 0x0,
    kInfoServer,
    kInfoClients,
    kInfoStats,
    kInfoReplication,
    kInfoKeyspace,
    kInfoBgstats,
    kInfoLog,
    kInfoData,
    kInfoCache,
    kInfoZset,
    kInfoDelay,
    kInfoRocks,
    kInfoLevelStats,
    kInfoAll
  };

  InfoCmd() : rescan_(false), off_(false), interval_(0), 
    rocksdb_type_(blackwidow::ALL_DB),
    levelstats_db_type_(blackwidow::ALL_DB) {
  }
  virtual void Do();
private:
  InfoSection info_section_;
  bool rescan_; //whether to rescan the keyspace
  bool off_;
  int32_t interval_;
  std::string rocksdb_type_;
  std::string levelstats_db_type_;

  const static std::string kAllSection;
  const static std::string kServerSection;
  const static std::string kClientsSection;
  const static std::string kStatsSection;
  const static std::string kReplicationSection;
  const static std::string kKeyspaceSection;
  const static std::string kLogSection;
  const static std::string kDataSection;
  const static std::string kCache;
  const static std::string kZset;
  const static std::string kDelay;
  const static std::string kRocks;
  const static std::string kLevelStats;


  virtual void DoInitial(const PikaCmdArgsType &argvs, const CmdInfo* const ptr_info);
  virtual void Clear() {
    rescan_ = false;
    off_ = false;
    interval_ = 0;
    rocksdb_type_ = blackwidow::ALL_DB;
    levelstats_db_type_ = blackwidow::ALL_DB;
  }

  void InfoServer(std::string &info);
  void InfoClients(std::string &info);
  void InfoStats(std::string &info);
  void InfoReplication(std::string &info);
  void InfoKeyspace(std::string &info);
  void InfoLog(std::string &info);
  void InfoData(std::string &info);
  void InfoRocks(std::string &info);
  void InfoLevelStats(std::string &info);
  void InfoCache(std::string &info);
  void InfoZset(std::string &info);
  void InfoDelay(std::string &info);

  std::string CacheStatusToString(int status);
  std::string TaskTypeToString(int task_type);
};

class ShutdownCmd : public Cmd {
public:
  ShutdownCmd() {
  }
  virtual void Do();
private:
  virtual void DoInitial(const PikaCmdArgsType &argvs, const CmdInfo* const ptr_info);
};

class ConfigCmd : public Cmd {
public:
  ConfigCmd() {
  }
  virtual void Do();
private:
  std::vector<std::string> config_args_v_;
  virtual void DoInitial(const PikaCmdArgsType &argv, const CmdInfo* const ptr_info);
  void ConfigGet(std::string &ret);
  void ConfigSet(std::string &ret);
  void ConfigRewrite(std::string &ret);
  void ConfigResetstat(std::string &ret);
};

class MonitorCmd : public Cmd {
public:
  MonitorCmd() {
  }
  virtual void Do();
private:
  virtual void DoInitial(const PikaCmdArgsType &argv, const CmdInfo* const ptr_info);
};

class DbsizeCmd : public Cmd {
public:
  DbsizeCmd() {
  }
  virtual void Do();
private:
  virtual void DoInitial(const PikaCmdArgsType &argv, const CmdInfo* const ptr_info);
};

class TimeCmd : public Cmd {
public:
  TimeCmd() {
  }
  virtual void Do();
private:
  virtual void DoInitial(const PikaCmdArgsType &argv, const CmdInfo* const ptr_info);
};

class DelbackupCmd : public Cmd {
public:
  DelbackupCmd() {
  }
  virtual void Do();
private:
  virtual void DoInitial(const PikaCmdArgsType &argv, const CmdInfo* const ptr_info);
};

#ifdef TCMALLOC_EXTENSION
class TcmallocCmd : public Cmd {
public:
  TcmallocCmd() {
  }
  virtual void Do();
private:
  int64_t type_;
  double rate_;
  virtual void DoInitial(const PikaCmdArgsType &argv, const CmdInfo* const ptr_info);
};
#endif

class EchoCmd : public Cmd {
public:
  EchoCmd() {
  }
  virtual void Do();
private:
  std::string echomsg_;
  virtual void DoInitial(const PikaCmdArgsType &argvs, const CmdInfo* const ptr_info);
};

class SlowlogCmd : public Cmd {
 public:
  enum SlowlogCondition{kGET, kLEN, kRESET};
  SlowlogCmd() : condition_(kGET) {}
  virtual void Do();
 private:
  int64_t number_;
  SlowlogCmd::SlowlogCondition condition_;
  virtual void DoInitial(const PikaCmdArgsType &argv, const CmdInfo* const ptr_info);
  virtual void Clear() {
    number_ = 10;
    condition_ = kGET;
  }
};

class CacheCmd : public Cmd {
 public:
  enum CacheCondition {kCLEAR_DB, kCLEAR_HITRATIO, kDEL_KEYS, kRANDOM_KEY};
  CacheCmd() {}
  virtual void Do();
 private:
  CacheCondition condition_;
  std::vector<std::string> keys_;
  virtual void DoInitial(const PikaCmdArgsType &argv, const CmdInfo* const ptr_info);
  virtual void Clear() override {
    keys_.clear();
  }
};

class ZsetAutoDelCmd : public Cmd {
public:
  virtual void Do();
private:
  int64_t cursor_;
  double speed_factor_;
  virtual void DoInitial(const PikaCmdArgsType &argv, const CmdInfo* const ptr_info);
};

class ZsetAutoDelOffCmd : public Cmd {
public:
  virtual void Do();
private:
  virtual void DoInitial(const PikaCmdArgsType &argv, const CmdInfo* const ptr_info);
};

class PikaAdminCmd : public Cmd {
public:
  virtual void Do();
private:
  std::string operation_;
  virtual void DoInitial(const PikaCmdArgsType &argvs, const CmdInfo* const ptr_info);
};
#endif
