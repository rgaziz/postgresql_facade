// psql_facade.cc   13/02/2022
/* ========================================================================== */
#include "psql_facade.h"
#include <chrono>

/* ========================================================================== */
using namespace psql;
using namespace std::chrono;

/* ========================================================================== */
bool PsqlFacade::insert_group(
    const std::string &table_name, const std::vector<std::string> &fields,
    const std::vector<std::vector<std::string>> &params,
    const size_t pack_size) {
  std::stringstream ss, vv;

  ss << "INSERT INTO " << table_name;
  if (not fields.empty()) {
    ss << create_str(fields);
  }

  ss << "VALUES";
  for (size_t i{0}, count{0}; i < params.size(); ++i, ++count) {
    vv << create_str(params[i]) << (i < params.size() - 1 ? ", " : "");

    if (count == pack_size) {
      std::string query{ss.str() + vv.str()};

      execute(query);
      if (not commit()) {
        std::cerr << "Insert to table " << table_name << " failed!"
                  << std::endl;
        rollback();
        return false;
      }

      count = 0;
      vv.clear();
    }
  }

  std::string query{ss.str() + vv.str()};
  execute(query);
  return true;
}

/* ========================================================================== */
bool PsqlFacade::commit() {
  PGresult *result = PQexec(conn_, "COMMIT");

  bool state = check_result(result);
  PQclear(result);

  return state;
}

/* ========================================================================== */
bool PsqlFacade::rollback() {
  PGresult *result = PQexec(conn_, "ROLLBACK");

  bool state = check_result(result);
  PQclear(result);

  return state;
}

/* ========================================================================== */
bool PsqlFacade::check_result(PGresult *result) {
  ExecStatusType status = PQresultStatus(result);

  switch (status) {
    case PGRES_COMMAND_OK:
    case PGRES_TUPLES_OK:
      return true;

    default:
      std::cerr << status << " - " << PQresultErrorMessage(result) << std::endl;
      PQclear(result);
      return false;
  }
}

/* ========================================================================== */
std::vector<PsqlRow> PsqlFacade::parse_result(PGresult *result) {
  std::vector<PsqlRow> data;

  for (int r = 0; r < PQntuples(result); ++r) {
    PsqlRow row;

    for (int c = 0; c < PQnfields(result); ++c) {
      char *name = PQfname(result, c);

      if (PQgetisnull(result, r, c)) {
        row.set_data(std::string(name), std::string());

      } else {
        char *value = PQgetvalue(result, r, c);
        row.set_data(std::string(name), std::string(value));
      }
    }

    data.emplace_back(row);
  }

  PQclear(result);
  return data;
}

/* ========================================================================== */
std::string PsqlFacade::create_str(const std::vector<std::string> &values) {
  std::stringstream ss;
  ss << "(";

  for (size_t i{0}; i < values.size(); ++i) {
    ss << values[i] << (i < values.size() - 1 ? ", " : "");
  }

  ss << ")";
  return ss.str();
}

/* ========================================================================== */
void PsqlFacade::log_query_time(const std::string_view &query,
                                time_point<steady_clock> &start) {
  auto end = steady_clock::now();
  auto diff = duration_cast<microseconds>(end - start);

  auto sec = duration_cast<seconds>(diff);
  auto msec = duration_cast<microseconds>(diff - sec);

  std::cout << "Query: " << query << ". dTime: " << sec.count() << "."
            << msec.count() << std::endl;
}

/* ========================================================================== */
std::string PsqlFacade::get_last_error() {
  return std::string(PQerrorMessage(conn_));
}

/* ========================================================================== */
