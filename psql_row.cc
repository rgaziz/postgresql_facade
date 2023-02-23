// psql_row.cc   14/02/2022
/* ========================================================================== */
#include "psql_row.h"

/* ========================================================================== */
using namespace psql;

/* ========================================================================== */
bool PsqlRow::is_null(const std::string &name) { return fields_[name].empty(); }

/* ========================================================================== */
void PsqlRow::set_data(const std::string &name, const std::string &value) {
  fields_.emplace(std::make_pair(name, value));
}

/* ========================================================================== */
int PsqlRow::convert(std::string &value, int8_t &res) {
  return std::sscanf(value.data(), "%c", &res);
}

/* ========================================================================== */
int PsqlRow::convert(std::string &value, int32_t &res) {
  return std::sscanf(value.data(), "%d", &res);
}

/* ========================================================================== */
int PsqlRow::convert(std::string &value, int16_t &res) {
  return std::sscanf(value.data(), "%hd", &res);
}

/* ========================================================================== */
int PsqlRow::convert(std::string &value, int64_t &res) {
  return std::sscanf(value.data(), "%ld", &res);
}

/* ========================================================================== */
int PsqlRow::convert(std::string &value, uint8_t &res) {
  return std::sscanf(value.data(), "%cu", &res);
}

/* ========================================================================== */
int PsqlRow::convert(std::string &value, uint32_t &res) {
  return std::sscanf(value.data(), "%ud", &res);
}

/* ========================================================================== */
int PsqlRow::convert(std::string &value, uint16_t &res) {
  return std::sscanf(value.data(), "%hu", &res);
}

/* ========================================================================== */
int PsqlRow::convert(std::string &value, uint64_t &res) {
  return std::sscanf(value.data(), "%lu", &res);
}

/* ========================================================================== */
int PsqlRow::convert(std::string &value, float &res) {
  return std::sscanf(value.data(), "%f", &res);
}

/* ========================================================================== */
int PsqlRow::convert(std::string &value, double &res) {
  return std::sscanf(value.data(), "%lf", &res);
}

/* ========================================================================== */
int PsqlRow::convert(std::string &value, std::string &res) {
  res = value;
  return CONVERT_OK;
}

/* ========================================================================== */
