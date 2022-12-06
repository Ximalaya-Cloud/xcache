// Copyright (c) 2015-present, Qihoo, Inc.  All rights reserved.
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree. An additional grant
// of patent rights can be found in the PATENTS file in the same directory.

#ifndef PIKA_CLIENT_CONN_H_
#define PIKA_CLIENT_CONN_H_

#include <atomic>

#include <glog/logging.h>

#include "pink/include/redis_conn.h"
#include "pink/include/pink_thread.h"
#include "slash/include/slash_mutex.h"
#include "pika_command.h"

class PikaWorkerSpecificData;

class PikaClientConn: public pink::RedisConn {
 public:
  struct BgTaskArg {
    std::shared_ptr<PikaClientConn> pcc;
    std::vector<pink::RedisCmdArgsType> redis_cmds;
    std::string* response;
    uint64_t recv_cmd_time_us;
    uint32_t queue_size;
  };

  PikaClientConn(int fd, std::string ip_port, pink::ServerThread *server_thread,
                 void* worker_specific_data, pink::PinkEpoll* pink_epoll,
                 const pink::HandleType& handle_type);
  virtual ~PikaClientConn() {}

  void SyncProcessRedisCmd(const pink::RedisCmdArgsType& argv, std::string* response) override;
  void AsynProcessRedisCmds(const std::vector<pink::RedisCmdArgsType>& argvs, std::string* response) override;

  void BatchExecRedisCmd(const std::vector<pink::RedisCmdArgsType>& argvs,
                         std::string* response,
                         uint64_t recv_cmd_time_us,
                         uint32_t queue_size);
  int ExecRedisCmd(const pink::RedisCmdArgsType& argv,
                   std::string* response,
                   uint64_t recv_cmd_time_us,
                   uint32_t queue_size);

  int DealMessage(const pink::RedisCmdArgsType& argv, std::string* response);
  static void DoBackgroundTask(void* arg);

  bool IsPubSub() { return is_pubsub_; }
  void SetIsPubSub(bool is_pubsub) { is_pubsub_ = is_pubsub; }

 private:
  pink::ServerThread* const server_thread_;
  CmdTable* const cmds_table_;
  bool is_pubsub_;

  static std::atomic<uint64_t> slowlog_count_;
  static slash::Mutex slowlog_mutex_;

  std::string DoCmd(const PikaCmdArgsType& argv,
                    const std::string& opt,
                    uint64_t recv_cmd_time_us,
                    uint32_t queue_size);
  std::string RestoreArgs(const PikaCmdArgsType& argv);

  // Auth related
  class AuthStat {
   public:
    void Init();
    bool IsAuthed(const CmdInfo* const cinfo_ptr);
    bool ChecknUpdate(const std::string& arg);
   private:
    enum StatType {
      kNoAuthed = 0,
      kAdminAuthed,
      kLimitAuthed,
    };
    StatType stat_;
  };
  AuthStat auth_stat_;
};

struct ClientInfo {
  int fd;
  std::string ip_port;
  int64_t last_interaction;
  std::shared_ptr<PikaClientConn> conn;
};

#endif
