// psql_connection.cc   13/02/2022
/* ========================================================================== */
#include "psql_connection.h"

/* ========================================================================== */
using namespace psql;

/* ========================================================================== */
PsqlConnection::PsqlConnection(const std::string &connection_info) {
  connection_info_ = connection_info;
}

/* ========================================================================== */
PsqlConnection::PsqlConnection(
    const std::string &host, const std::string &port,
    const std::string &db_name, const std::string &user,
    const std::string &password, const int connect_timeout_sec,
    const std::map<std::string, std::string> &params) {
  std::stringstream ss;

  if (not host.empty()) {
    ss << "host=" << host;
  }

  if (not port.empty()) {
    ss << " port=" << port;
  }

  if (not db_name.empty()) {
    ss << " dbname=" << db_name;
  }

  if (not user.empty()) {
    ss << " user=" << user;
  }

  if (not password.empty()) {
    ss << " password=" << password;
  }

  ss << " connect_timeout=" << connect_timeout_sec;

  if (not params.empty()) {
    for (auto[key, value] : params) {
      ss << " " << key << "=" << value;
    }
  }

  connection_info_ = ss.str();
}

/* ========================================================================== */
PsqlConnection::~PsqlConnection() {
  if (is_connected()) {
    disconnect();
  }
}

/* ========================================================================== */
bool PsqlConnection::connect() {
  conn_ = PQconnectdb(connection_info_.c_str());

  if (not is_connected()) {
    std::cerr << "Connection to database failed! Error: "
              << PQerrorMessage(conn_) << std::endl;

    return false;
  }

  return true;
}

/* ========================================================================== */
void PsqlConnection::disconnect() {
  if (conn_) {
    PQfinish(conn_);
  }
}

/* ========================================================================== */
bool PsqlConnection::ping() {
  PGPing state = PQping(connection_info_.c_str());

  switch (state) {
    case PQPING_OK:
      return true;

    case PQPING_REJECT:
      std::cerr << "The server is running but is in a state that disallows "
                   "connections"
                << std::endl;
      return false;

    case PQPING_NO_RESPONSE:
      std::cerr << "The server could not be contacted" << std::endl;
      return false;

    case PQPING_NO_ATTEMPT:
      std::cerr << "No attempt was made to contact the server" << std::endl;
      return false;

    default:
      std::cerr << "Unknown state for ping" << std::endl;
      return false;
  }
}

/* ========================================================================== */
bool PsqlConnection::reconnect() {
  PQreset(conn_);

  if (is_connected()) {
    return true;
  }

  disconnect();
  return connect();
}

/* ========================================================================== */
bool PsqlConnection::reconnect(int try_count, int wait_sec) {
  bool state{false};

  do {
    PQreset(conn_);
    if (is_connected()) {
      return true;
    }

    disconnect();
    state = connect();
    sleep(wait_sec);

  } while (not is_connected() and --try_count >= 0 and not state);

  return state;
}

/* ========================================================================== */
PGconn *PsqlConnection::get_connection() const { return conn_; }

/* ========================================================================== */
bool PsqlConnection::is_connected() const {
  return (PQstatus(conn_) == CONNECTION_OK);
}

/* ========================================================================== */
