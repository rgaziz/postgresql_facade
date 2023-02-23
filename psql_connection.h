// psql_connection.h   13/02/2022
/* ========================================================================== */
#pragma once

/* ========================================================================== */
#include <libpq-fe.h>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

/* ========================================================================== */
namespace psql {

/* ========================================================================== */
class PsqlConnection {
 public:
  explicit PsqlConnection(const std::string &connect_info);
  explicit PsqlConnection(
      const std::string &host, const std::string &port,
      const std::string &db_name, const std::string &user,
      const std::string &password, const int connect_timeout_sec = 10,
      const std::map<std::string, std::string> &params = {});

  virtual ~PsqlConnection();

 public:
  bool ping();
  bool connect();
  void disconnect();

 public:
  bool reconnect();
  bool reconnect(int try_count, int wait_sec = 1);

 public:
  PGconn *get_connection() const;
  bool is_connected() const;

 protected:
  PGconn *conn_{nullptr};
  std::string connection_info_;
};

/* ========================================================================== */
}  // namespace psql
/* ========================================================================== */
