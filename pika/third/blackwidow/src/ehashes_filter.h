#ifndef SRC_EHASHES_FILTER_H_
#define SRC_EHASHES_FILTER_H_

#include <string>
#include <memory>
#include <vector>

#include "src/debug.h"
#include "src/base_meta_value_format.h"
#include "src/base_data_key_format.h"
#include "src/base_filter.h"
#include "rocksdb/compaction_filter.h"
#include "src/strings_value_format.h"

namespace blackwidow {

class EhashesDataFilter : public rocksdb::CompactionFilter {
public:
    EhashesDataFilter(rocksdb::DB* db,
                      std::vector<rocksdb::ColumnFamilyHandle*>* cf_handles_ptr)
        : db_(db)
        , cf_handles_ptr_(cf_handles_ptr)
        , cur_key_("")
        , meta_not_found_(false)
        , cur_meta_version_(0)
        , cur_meta_timestamp_(0) {}

    bool Filter(int level, const rocksdb::Slice& key,
                const rocksdb::Slice& value,
                std::string* new_value, bool* value_changed) const override {

        ParsedBaseDataKey parsed_base_data_key(key);
        Trace("==========================START==========================");
        Trace("[DataFilter], key: %s, data = %s, version = %d",
              parsed_base_data_key.key().ToString().c_str(),
              parsed_base_data_key.data().ToString().c_str(),
              parsed_base_data_key.version());

        if (parsed_base_data_key.key().ToString() != cur_key_) {
            cur_key_ = parsed_base_data_key.key().ToString();
            std::string meta_value;
            // destroyed when close the database, Reserve Current key value
            if (cf_handles_ptr_->size() == 0) {
                return false;
            }
            Status s = db_->Get(default_read_options_, (*cf_handles_ptr_)[0], cur_key_, &meta_value);
            if (s.ok()) {
                meta_not_found_ = false;
                ParsedBaseMetaValue parsed_base_meta_value(&meta_value);
                cur_meta_version_ = parsed_base_meta_value.version();
                cur_meta_timestamp_ = parsed_base_meta_value.timestamp();
            } else if (s.IsNotFound()) {
                meta_not_found_ = true;
            } else {
                cur_key_ = "";
                Trace("Reserve[Get meta_key faild]");
                return false;
            }
        }

        if (meta_not_found_) {
            Trace("Drop[Meta key not exist]");
            return true;
        }

        int64_t unix_time;
        rocksdb::Env::Default()->GetCurrentTime(&unix_time);
        if (cur_meta_timestamp_ != 0 
            && cur_meta_timestamp_ < static_cast<int32_t>(unix_time)) {
            Trace("Drop[Timeout]");
            return true;
        }

        if (cur_meta_version_ > parsed_base_data_key.version()) {
            Trace("Drop[data_key_version < cur_meta_version]");
            return true;
        }

        ParsedStringsValue parsed_strings_value(value);
        if (parsed_strings_value.timestamp() != 0
            && parsed_strings_value.timestamp() < unix_time) {
            Trace("Drop[ehashes data timeout]");
            return true;
        }

        return false;
    }
    bool IgnoreSnapshots() const override { return true; }
    
    const char* Name() const override { return "EhashesDataFilter"; }

private:
    rocksdb::DB* db_;
    std::vector<rocksdb::ColumnFamilyHandle*>* cf_handles_ptr_;
    rocksdb::ReadOptions default_read_options_;
    mutable std::string cur_key_;
    mutable bool meta_not_found_;
    mutable int32_t cur_meta_version_;
    mutable int32_t cur_meta_timestamp_;
};

class EhashesDataFilterFactory : public rocksdb::CompactionFilterFactory {
public:
    EhashesDataFilterFactory(rocksdb::DB** db_ptr,
                             std::vector<rocksdb::ColumnFamilyHandle*>* handles_ptr)
      : db_ptr_(db_ptr)
      , cf_handles_ptr_(handles_ptr) {}

    std::unique_ptr<rocksdb::CompactionFilter> CreateCompactionFilter(
        const rocksdb::CompactionFilter::Context& context) override {
        return std::unique_ptr<rocksdb::CompactionFilter>(
            new EhashesDataFilter(*db_ptr_, cf_handles_ptr_));
    }

    const char* Name() const override {
        return "EhashesDataFilterFactory";
    }

private:
    rocksdb::DB** db_ptr_;
    std::vector<rocksdb::ColumnFamilyHandle*>* cf_handles_ptr_;
};

typedef BaseMetaFilter EhashesMetaFilter;
typedef BaseMetaFilterFactory EhashesMetaFilterFactory;

} // namespace blackwidow

#endif
