// psql_row.h   14/02/2022
/* ========================================================================== */
#pragma once

/* ========================================================================== */
#include <charconv>
#include <optional>
#include <unordered_map>
#include "psql_connection.h"

/* ========================================================================== */
namespace psql {

/* ========================================================================== */
const int CONVERT_OK = 1;

/* ========================================================================== */
class PsqlRow {
 public:
  PsqlRow() = default;
  ~PsqlRow() = default;

 public:
  bool is_null(const std::string &name);
  void set_data(const std::string &name, const std::string &value);

 public:
  template <typename T>
  std::optional<T> get_value(const std::string &name) {
    if (not fields_.count(name)) {
      std::cerr << "Field " << name << " not found!" << std::endl;
      return std::nullopt;
    }

    T value;
    if (convert(fields_[name], value) not_eq CONVERT_OK) {
      std::cerr << "Convert type error!" << std::endl;
      return std::nullopt;
    }

    return value;
  }

 private:
  int convert(std::string &value, int8_t &res);
  int convert(std::string &value, int32_t &res);
  int convert(std::string &value, int16_t &res);
  int convert(std::string &value, int64_t &res);

  int convert(std::string &value, uint8_t &res);
  int convert(std::string &value, uint32_t &res);
  int convert(std::string &value, uint16_t &res);
  int convert(std::string &value, uint64_t &res);

  int convert(std::string &value, float &res);
  int convert(std::string &value, double &res);

  int convert(std::string &value, std::string &res);

 private:
  std::unordered_map<std::string, std::string> fields_;
};

/* ========================================================================== */
}  // namespace psql
/* ========================================================================== */
