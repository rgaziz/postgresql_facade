// psql_facade.h   13/02/2022
/* ========================================================================== */
#pragma once

/* ========================================================================== */
#include <algorithm>
#include <set>
#include <stdexcept>
#include "psql_row.h"

/* ========================================================================== */
namespace psql {

/* ========================================================================== */
class PsqlFacade final : public PsqlConnection {
 public:
  PsqlFacade(const std::string &connect_info) : PsqlConnection(connect_info) {}
  PsqlFacade(const std::string &host, const std::string &port,
             const std::string &db_name, const std::string &user,
             const std::string &password, const int connect_timeout_sec = 10,
             const std::map<std::string, std::string> &params = {})
      : PsqlConnection(host, port, db_name, user, password, connect_timeout_sec,
                       params) {}

  ~PsqlFacade() = default;

 private:
  /* ======================================================================== */
  template <typename T>
  std::string convert(T &value) {
    std::stringstream ss;
    ss << value;

    return ss.str();
  }

 public:
  /* ======================================================================== */
  std::vector<PsqlRow> execute(const std::string &query) {
    auto start = std::chrono::steady_clock::now();

    PGresult *result = PQexec(conn_, query.data());
    log_query_time(query, start);

    if (not check_result(result)) {
      throw std::runtime_error("Execute query failed!");
    }

    return parse_result(result);
  }

 public:
  /* ======================================================================== */
  std::vector<PsqlRow> execute_params(const std::string &query,
                                      std::vector<std::string> &params) {
    std::vector<const char *> values;
    std::transform(params.begin(), params.end(), std::back_inserter(values),
                   [&](std::string &str) { return str.data(); });

    auto start = std::chrono::steady_clock::now();
    PGresult *result = PQexecParams(conn_, query.data(), params.size(), nullptr,
                                    values.data(), nullptr, nullptr, 0);

    log_query_time(query, start);
    if (not check_result(result)) {
      throw std::runtime_error("Execute query failed!");
    }

    return parse_result(result);
  }

 public:
  /* ======================================================================== */
  template <typename... Params>
  std::vector<PsqlRow> execute_prepared(const std::string &query,
                                        Params... params) {
    std::vector<std::string> args{convert(params)...};

    std::vector<const char *> values;
    std::transform(args.begin(), args.end(), std::back_inserter(values),
                   [&](std::string &str) { return str.c_str(); });

    if (not statments_.count(query)) {
      auto result = PQprepare(conn_, query.c_str(), query.c_str(),
                              values.size(), nullptr);

      if (not check_result(result)) {
        throw std::runtime_error("Prepare stmt failed!");
      }

      statments_.insert(query);
      PQclear(result);
    }

    auto start = std::chrono::steady_clock::now();
    PGresult *result = PQexecPrepared(conn_, query.c_str(), values.size(),
                                      values.data(), nullptr, nullptr, 0);

    log_query_time(query, start);
    if (not check_result(result)) {
      throw std::runtime_error("Execute query failed!");
    }

    return parse_result(result);
  }

  /* ======================================================================== */
 public:
  bool commit();
  bool rollback();
  bool insert_group(const std::string &table_name,
                    const std::vector<std::string> &fields,
                    const std::vector<std::vector<std::string>> &params,
                    const size_t pack_size = 100);

  std::string get_last_error();

 private:
  bool check_result(PGresult *result);
  std::vector<PsqlRow> parse_result(PGresult *result);
  std::string create_str(const std::vector<std::string> &values);

  void log_query_time(
      const std::string_view &query,
      std::chrono::time_point<std::chrono::steady_clock> &start);

 private:
  std::set<std::string> statments_;
};
/* ========================================================================== */
};  // namespace psql
/* ========================================================================== */
