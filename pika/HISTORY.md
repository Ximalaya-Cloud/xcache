# pika for codis
## 3.0.4-2.2 (2019-05-09)
### New Features
* 添加ehash数据类型，支持field过期时间设置

## 3.0.4-2.1 (2019-05-06)
### optimize
* 支持静态编译pika，运行时不再需要额外的动态库
* 将info命令中耗时操作放到异步线程中去做

### bug fix
* 为配置项设置默认值，防止配置项注释掉后，被赋予随机值

## 3.0.4-2.0 (2019-04-02)
### New Features
* 添加redis缓存模块，支持所有数据类型的缓存读写
* 添加发布订阅功能
* 添加慢日志功能
* 写binlog流程优化，使用异步线程去写binlog，worker线程只是创建一个写binlog任务，并加入到异步线程队列

### bug fix
* 修复block-cache配置读取异常的bug

## 3.0.4-1.3 (2019-01-25)
### bug fix
* 修复slotsdel命令处理不存在的slotsinfo key时直接退出的bug
* 修复slotsdel命令删除key失败的bug

## 3.0.4-1.2 (2018-12-27)
### New Features
* 增加block_cache相关配置
* 增加max_subcompactions配置项
* 支持动态修改rocksdb配置

### bug fix
* 修改hset命令不写slotinfo信息

## 3.0.4-1.1 (2018-11-26)
### New Features
* 增加block_cache相关配置
* 增加max_subcompactions配置项
* 支持动态修改rocksdb配置

### bug fix
* 修改string类型expireat命令当timestamp为0时的行为与redis不一致
* 修改hset命令不写slotinfo信息
* 修改全量同步过程中resync database文件两次的bug

## 3.0.4-1.0 (2018-10-25)
### New Features
* rocksdb 中 max_subcompactions 参数默认值设为3
* 升级至最新引擎black_widow,新引擎读写性能更优
* 新引擎与pika-2.2.6数据不兼容
* 在线修改rocksdb配置不生效

## 2.2.6-3.6 (2018-07-11)
### New Features
* 支持发布订阅
* aof_to_pika工具增加启动参数-b和-s,用于限制发送速度

## 3.5-2.2.6 (2018-03-20)
### New Features
* 新增max-log-size配置项
* 新增disable-auto-compactions配置项
* 新增max-write-buffer-number配置项
* 新增max-bytes-for-level-base配置项
* 新增level0-file-num-compaction-trigger配置项
* 新增level0-slowdown-writes-trigger配置项
* 新增level0-stop-writes-trigger配置项
* config rewrite 将注释的配置项追加到配置文件末尾
* glog日志文件按文件大小切分
* aof_to_pika工具支持断点续传

## 2.2.6-4(xmly) (2018-03-13)
### bug fix
* 兼容与pika-2.1.0进行主从同步

## 2.2.6-3(xmly) (2018-02-05)
### bug fix
* bgsave_thread_在执行DoDBSync()时获取已经销毁的db_sync_protector_导致coredump

## 2.2.6-2(xmly) (2018-01-16)
### Performance Improvements
* 删除多余日志

## 2.2.6-1(xmly) (2017-12-18)
### New Features
* 增加slotsrestore命令，支持redis向pika迁移数据
* slave状态中增加了repl_down_since_字段(master宕机codis选slave时使用)
* slot操作增加tag功能
* 增加key数量显示
* 修改定期compact逻辑，同时满足compact-cron和compact-interval才会执行
* 修改通过config get maxmemory时返回pika占用磁盘空间

* third/slash中添加trylock接口

### bug fix
* pike_trysync_thread.cc中RecvProc()函数获取是否需要认证时使用masterauth()
* PikaTrysyncThread::Send()删除多余的获取requirepass的逻辑

* third/slash中将条件变量等待时间从系统时间改为开机时间CLOCK_REALTIME  to CLOCK_MONOTONIC，防止修改系统时间导致timewait执行错误

* 修复pink中redis_cli.cc中解析redis响应遇到REDIS_HALF是类型错误的bug
* pink中默认rbuf大小为64M改为2M

* 系统接收kill信号处理
* 修复磁盘空间满时，pika coredump问题(pika-2.3.0修复，增加写binlog失败释放锁)
* DBSyncSendFile()函数判断GetChildren()返回值错误bug修复
* 修复PurgeFiles()函数中purgebinlog日志异常bug

* 修复数据迁移建立连接使用masterauth密码，改为使用userpass密码
* 去掉Trysync命令必须是requirepass才允许执行的现在(这样只有命令黑名单中的命令需要requirepass认证才能执行)
* 修复bitset和bitget命令当key过期后，依然可以获取到key的value的bug
* 修复lpushx、rpushx在key不存在时也去写slots的错误逻辑，将ok()和isnotfound()分开处理
* 为lset、linsert增加slots操作，当执行ok时写slot

### Performance Improvements
* 优化info命令，每隔60秒才去统计一次磁盘空间使用情况
* 数据迁移复用连接
* 将slotsdel放到后台运行，并支持范围删除
* SerializeRedisCommand redis.cc中argv类型应该传引用更节省内存
* 修复hgetall不存在的key非常耗时的问题，执行hgetall之前先判断一下key是否存在


### Public API changes
* 
