**Фасад для работы с БД PostgreSQL**

**- Методы инициализации фасада**
 
   Заголовок `#include <psql_facade.h>`
   1) <pre> PsqlFacade db_facade("host=localhost port=5432 dbname=mydb user=user password=***** connect_timeout=10"); <\pre>
   
   2) <pre>  std::string coninfo{"host=localhost port=5432 dbname=mydb user=user password=***** connect_timeout=10"}; 
        PsqlFacade db_facade(coninfo); <\pre>

   3) <pre> PsqlFacade db_facade("host", "port", "db_name", "user", "password", connect_timeout, params);
        Следующие поля можно пропустить:
            - int connect_timeout - таймаут на подлюключение. Если не установить, то по умолчанию равен 10 секунд 
            - std::map< std::string, std::string > params - Доплнительные поля для подключения, если есть необходимость [ключ=значение]. По умолчанию пустой <\pre> 

**- Методы подключения:**
   1) <pre> bool connect() - Подлюключение к БД <\pre>
   2) <pre> void disconnect() - Отключение от БД <\pre>
   3) <pre> bool reconnect() - Переподлюключение к БД <\pre>
   4) <pre> bool reconnect(int try_count, int wait_sec = 1) - Попытка переподключения к БД указанное количество раз с указанным ожиданием секунд<\pre>
   5) <pre> bool ping() - Проверка доступности БД <\pre>
   6) <pre> bool is_connected() - Проверка подключения к БД <\pre>
   7) <pre> PGconn *get_connection()- Возвращает объект подключения <\pre>
   8) <pre> bool commit() - Сохранение изменений, если имеются <\pre>
   9) <pre> bool rollback() - Откат последних изменений, если имеются<\pre>

**- Методы исполнения запросов:**
   1) <pre> std::vector< PsqlRow > execute(const std::string &query)

       - Исполнение готового запроса в виде строки. Использовать
        только для запросов, которые редко применяются. 
        Пример:
            auto result = db_facade.execute_prepared("SELECT * FROM t_table WHERE id = 111"); <\pre>

   2) <pre>   std::vector<PsqlRow> execute_params(const std::string &query, std::vector<std::string> &values)

      - Отправляет команду серверу и ожидает результата. Имеет возможность передать параметры отдельно от текста SQL-команды. 
      Использовать только для запросов, которые редко применяются.
        Пример:
            std::vector< std::string > params {"123", "test"};
            auto result = db_facade.execute_params("SELECT * FROM t_table WHERE id = $1 and name = $2", params); <\pre>    

   3) <pre>   std::vector<PsqlRow> execute_prepared(const std::string &query, Params... params)

      - Отправляет запрос на исполнение подготовленного оператора с данными параметрами и ожидает результата. 
      Использовать для запросов, которые часто применяются.
         Пример:
             auto result = db_facade.execute_prepared("SELECT * FROM t_table WHERE id = $1 and name = $2", 123, "test"); <\pre>      
   
   4) <pre>   bool insert_group(const std::string &table_name,
                    const std::vector<std::string> &fields,
                    const std::vector<std::vector<std::string>> &params,
                    const size_t pack_size = 100) 
                    
      - Вставка в таблицу пачкой. INSERT INTO t_table(id, name) VALUES (1, "test1"),(2, "test2"), (3, "test3") и т.д. 
         table_name - имя таблицы для вставки
         fields     - названия полей талбицы, куда надо вставить
         params     - Значения для этих полей. Для строк необходимо использовать одинарные кавычки
         pack_size  - Размер пакета для вставки. По умолчанию равен 100
         
         Пример:
            std::vector < std::string > fields {"id", "name"};
            std::vector < std::vector < std::string > > values {{"1", "'test1'"}, {"2", "'test2'"}, {"3", "'test3'"}};     
           
            bool state  = db_facade.insert_group("t_table", fields, values, 10); <\pre>        
            
            
            
**Пример кода**
<pre>
#include </psql_facade.h>
 
// Создание объекта
PsqlFacade db_facade("host=localhost port=5432 dbname=mydb user=user password=quwerty123 connect_timeout=10");
 
// Подключение к БД
if (not db_facade.connect()) {
    std::cerr << "Connect to DB failed!") << std::endl;
}
 
// Исполнение запроса
auto result = db_facade.execute("SELECT * FROM my_table WHERE id = 123");
if (not result.empty()) {
  for (auto &row : result) {
     std::cout << *row.get_value<int64_t>("id") << std::endl;
     std::cout << *row.get_value<std::string>("name") << std::endl;
     std::cout << *row.get_value<std::string>("date") << std::endl; 
  }
}
<\pre>
            
