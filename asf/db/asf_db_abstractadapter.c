/*
  +----------------------------------------------------------------------+
  | API Services Framework                                               |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2018 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Jiapeng Fan <fanjiapeng@126.com>                             |
  +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "ext/pdo/php_pdo_driver.h"
#include "Zend/zend_interfaces.h" /* zend_call_ */
#include "zend_smart_str.h" /* for smart_str */
#include "ext/standard/php_var.h" /* php_serialize_data_t */

#include "php_asf.h"
#include "kernel/asf_namespace.h"
#include "asf_exception.h"
#include "kernel/asf_func.h"

#include "db/asf_db_adapterinterface.h"
#include "db/asf_db_abstractadapter.h"
#include "log/asf_log_adapter.h"
#include "log/adapter/asf_log_adapter_file.h"

#include <sys/time.h>

zend_class_entry *asf_absadapter_ce;

/* {{{ ARG_INFO
*/
ZEND_BEGIN_ARG_INFO_EX(asf_absadapter_doquery_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, sql)
    ZEND_ARG_ARRAY_INFO(0, bind_value, 1)
    ZEND_ARG_INFO(0, mode)
    ZEND_ARG_INFO(0, fetch_one)
ZEND_END_ARG_INFO()
/* }}} */

_Bool asf_db_absadapter_instance(asf_db_t *this_ptr, const char *type, zval *configs) /* {{{ */
{
    size_t args_i = 4;
    zval *dsn = NULL, *username = NULL, *password = NULL, *options_1 = NULL, options_2, *persistent = NULL;
    zval exception, timeout;

    if (UNEXPECTED((dsn = zend_hash_str_find(Z_ARRVAL_P(configs), "dsn", 3)) == NULL)) {
        asf_trigger_error(ASF_ERR_DB_OPTIONS_DSN, "The options 'dsn' not found");
        return 0;
    }

    // sqlite options username allow empty value
    username = zend_hash_str_find(Z_ARRVAL_P(configs), "username", 8);
    if (strncmp(type, ASF_DB_TYPE_SQLTILE, 6) != 0 && !username) {
        asf_trigger_error(ASF_ERR_DB_OPTIONS_USERNAME, "The options 'username' not found");
        return 0;
    }

    password = zend_hash_str_find(Z_ARRVAL_P(configs), "password", 8);
    options_1 = zend_hash_str_find(Z_ARRVAL_P(configs), "options", 7);

    if (options_1 && IS_ARRAY == Z_TYPE_P(options_1)) {
        if (!zend_hash_index_exists(Z_ARRVAL_P(options_1), PDO_ATTR_ERRMODE)) {
            ZVAL_LONG(&exception, PDO_ERRMODE_EXCEPTION);
            zend_hash_index_add(Z_ARRVAL_P(options_1), PDO_ATTR_ERRMODE, &exception);
        }
        if (!zend_hash_index_exists(Z_ARRVAL_P(options_1), PDO_ATTR_TIMEOUT)) {
            ZVAL_LONG(&timeout, 1);
            zend_hash_index_add(Z_ARRVAL_P(options_1), PDO_ATTR_TIMEOUT, &timeout);
        }

        ZVAL_COPY_VALUE(&options_2, options_1);
    } else {
        options_1 = NULL;
        array_init(&options_2);
        ZVAL_LONG(&exception, PDO_ERRMODE_EXCEPTION);
        ZVAL_LONG(&timeout, 1);

        zend_hash_index_add(Z_ARRVAL(options_2), PDO_ATTR_ERRMODE, &exception);
        zend_hash_index_add(Z_ARRVAL(options_2), PDO_ATTR_TIMEOUT, &timeout);
    }

    smart_str buf = {0};
    do {
        if (IS_ARRAY == Z_TYPE_P(dsn)) {
            zend_string *key = NULL, *sval = NULL;
            zval *val = NULL;

            /* set charset = utf8 */
            if (!zend_hash_str_exists(Z_ARRVAL_P(dsn), "charset", 7)) {
                smart_str_appendl(&buf, "charset=utf8;", 13);
            }

            ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(dsn), key, val) {
                if (key && (IS_NULL != Z_TYPE_P(val))) {
                    smart_str_appendl(&buf, ZSTR_VAL(key), ZSTR_LEN(key));
                    smart_str_appendl(&buf, "=", 1);
                    sval = zval_get_string(val);
                    smart_str_appendl(&buf, ZSTR_VAL(sval), ZSTR_LEN(sval));
                    smart_str_appendl(&buf, ";", 1);
                    zend_string_release(sval);
                }
            } ZEND_HASH_FOREACH_END();
            smart_str_0(&buf);
            break;
        }

        if (IS_STRING == Z_TYPE_P(dsn) && Z_STRLEN_P(dsn) > 0) {
            smart_str_appendl(&buf, Z_STRVAL_P(dsn), Z_STRLEN_P(dsn));
            smart_str_0(&buf);
            break;
        }

        asf_trigger_error(ASF_ERR_DB_OPTIONS_DSN, "The options 'dsn' must be array or string");
        return 0;

    } while (0);

    zval zmn_1, zret_1;
    char *data_source = NULL; size_t data_source_len = 0;

    data_source_len = spprintf(&data_source, 0, "%s:%s", type, ZSTR_VAL(buf.s));
    smart_str_free(&buf);

    // pdo construct
    zval args[args_i], pdo;

    ZVAL_STRINGL(&zmn_1, "__construct", 11);
    ZVAL_STRINGL(&args[0], data_source, data_source_len);

    if (username) {
        ZVAL_COPY_VALUE(&args[1], username);
    } else {
        ZVAL_NULL(&args[1]);
    }

    if (password) {
        ZVAL_COPY_VALUE(&args[2], password);
    } else {
        ZVAL_NULL(&args[2]);
    }

    ZVAL_COPY_VALUE(&args[3], &options_2);

    if (UNEXPECTED(object_init_ex(&pdo, php_pdo_get_dbh_ce()) != SUCCESS)) {
        asf_trigger_error(ASF_ERR_DB_OPTIONS_DSN, "Object init pdo falied");
        return 0;
    }
    
    /* trace log */
    double start_time = asf_func_trace_gettime();
    call_user_function_ex(&Z_OBJCE(pdo)->function_table, &pdo, &zmn_1, &zret_1, args_i, args, 1, NULL);
    (void)asf_func_trace_str_add(ASF_TRACE_MYSQL, start_time, "__construct", 11, args_i, args, NULL);

    efree(data_source);
    ASF_FAST_STRING_PTR_DTOR(args[0]);
    ASF_FAST_STRING_PTR_DTOR(zmn_1);

    if (!options_1) {
        zval_ptr_dtor(&options_2);
    }

    /* If get value, the result is NULL in most cases */
    if (!Z_ISUNDEF(zret_1)) {
        zval_ptr_dtor(&zret_1);
    }

    if (UNEXPECTED(EG(exception))) {
        zval_ptr_dtor(&pdo);
        return 0;
    }

    zend_class_entry *ce = Z_OBJCE_P(this_ptr);

    zend_update_property(ce, this_ptr, ZEND_STRL("_dbh"), &pdo);
    zval_ptr_dtor(&pdo);

    return 1;
}
/* }}} */

static inline void asf_db_get_lastid(asf_db_t *this_ptr, zval *return_value, zend_bool ret_id)  /* {{{ */
{
    if (return_value && IS_UNDEF != Z_TYPE_P(return_value) && ret_id) {
        zval zmn_2;
        zval *dbh = zend_read_property(asf_absadapter_ce, this_ptr, ZEND_STRL("_dbh"), 1, NULL);

        zval_ptr_dtor(return_value);

        ZVAL_STRINGL(&zmn_2, "lastInsertId", 12);
        call_user_function_ex(&Z_OBJCE_P(dbh)->function_table, dbh, &zmn_2, return_value, 0, 0, 1, NULL);
        ASF_FAST_STRING_PTR_DTOR(zmn_2);
    }
}
/* }}} */

static inline _Bool asf_db_is_sqlite(asf_db_t *db) /* {{{ */
{
    /* SQLite not support "IGNORE" Syntax */
    /* SQLite not support "ON DUPLICATE KEY UPDATE" Syntax */
    /* SQLite not support "UPDATE LIMIT" Syntax */

    zval *type = zend_read_property(Z_OBJCE_P(db), db, ZEND_STRL("_type"), 1, NULL);

    if (strncmp(Z_STRVAL_P(type), ASF_DB_TYPE_SQLTILE, 6) == 0) {
        return 1;
    }

    return 0;
}
/* }}} */

static inline void asf_db_findby(asf_db_t *db, zval *pairs, zval *fields_name, zend_long mode, zend_bool is_one, zval *return_value) /* {{{ */
{
    size_t size = zend_hash_num_elements(Z_ARRVAL_P(pairs));
    if (size < 1) {
        asf_trigger_error(ASF_ERR_DB_SET_COLUMN, "The first parameter is array elements and more than zero");
        return;
    }

    zend_string *key = NULL;
    zval *entry = NULL, *table = NULL, *self = NULL;
    zend_bool mod = 0;
    size_t len = 0;
    smart_str col_name = {0}, sfields_name = {0};
    char *sql = NULL;
    zval values, args[4], zmn_1;
    zend_class_entry *db_ce = Z_OBJCE_P(db);

    array_init_size(&values, size);

    ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(pairs), key, entry) {
        if (!key) {
            continue;
        }
        if (mod) {
            smart_str_appendl(&col_name, " AND ", 5);
        }

        mod = 1;
        smart_str_appendl(&col_name, "`", 1);
        smart_str_appendl(&col_name, ZSTR_VAL(key), ZSTR_LEN(key));
        smart_str_appendl(&col_name, "` = ?", 5);

        ZVAL_DEREF(entry);
        Z_TRY_ADDREF_P(entry);
        add_next_index_zval(&values, entry);

    } ZEND_HASH_FOREACH_END();
    smart_str_0(&col_name);

    if (UNEXPECTED(!col_name.s)) {
        zval_ptr_dtor(&values);
        smart_str_free(&col_name);
        asf_trigger_error(ASF_ERR_DB_SET_COLUMN, "The first parameter is associative array elements");
        return;
    }

    /* select field */
    mod = 0;
    ZVAL_DEREF(entry);
    if (fields_name && IS_ARRAY == Z_TYPE_P(fields_name) && zend_hash_num_elements(Z_ARRVAL_P(fields_name))) {
        ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(fields_name), entry) {
            if (mod) {
                smart_str_appendl(&sfields_name, ", ", 2);
            }

            mod = 1;
            ZVAL_DEREF(entry);
            smart_str_appendl(&sfields_name, "`", 1);
            smart_str_appendl(&sfields_name, Z_STRVAL_P(entry), Z_STRLEN_P(entry));
            smart_str_appendl(&sfields_name, "`", 1);
        } ZEND_HASH_FOREACH_END();	
    } else {
        smart_str_appendl(&sfields_name, "*", 1);
    }
    smart_str_0(&sfields_name);

    table = zend_read_property(db_ce, db, ZEND_STRL("_table"), 1, NULL);
    len = spprintf(&sql, 0, "SELECT %s FROM `%s` WHERE %s", ZSTR_VAL(sfields_name.s), Z_STRVAL_P(table), ZSTR_VAL(col_name.s));

    smart_str_free(&col_name);
    smart_str_free(&sfields_name);

    ZVAL_STRINGL(&zmn_1, "doQuery", 7);
    ZVAL_NULL(&args[0]);
    ZVAL_STRINGL(&args[0], sql, len);
    ZVAL_COPY_VALUE(&args[1], &values);
    ZVAL_LONG(&args[2], mode);
    ZVAL_LONG(&args[3], is_one);

    efree(sql); sql = NULL;

    call_user_function_ex(&db_ce->function_table, db, &zmn_1, return_value, 4, args, 1, NULL);

    ASF_FAST_STRING_PTR_DTOR(zmn_1);
    ASF_FAST_STRING_PTR_DTOR(args[0]);
    zval_ptr_dtor(&values);
}
/* }}} */

static inline void asf_db_write_logsql(zend_string *sql, zval *bind_values, double start_time) /* {{{ */
{
    php_serialize_data_t var_hash;
    smart_str ins = {0};
    char *buffer = NULL; size_t size = 0;

    size = spprintf(&buffer, 0, "%.5f | ", (double)((asf_func_gettimeofday() - start_time)));
    smart_str_appendl(&ins, buffer, size);

    smart_str_append(&ins, sql);

    if (bind_values && IS_ARRAY == Z_TYPE_P(bind_values)) {
        smart_str_appendl(&ins, " | ", 3);
        PHP_VAR_SERIALIZE_INIT(var_hash);
        php_var_serialize(&ins, bind_values, &var_hash);
        PHP_VAR_SERIALIZE_DESTROY(var_hash);
    }

    smart_str_0(&ins);

    (void)asf_log_adapter_write_file("Asf_Sql_Log", 11, "info", 4, ZSTR_VAL(ins.s), ZSTR_LEN(ins.s));
    
    smart_str_free(&ins);
    efree(buffer);
}
/* }}} */

/* {{{ proto object Asf_Db_AbstractAdapter::__construct(array $configs [, bool $reconnection = false])
 */
PHP_METHOD(asf_absadapter, __construct)
{
    zval *configs = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "a", &configs) == FAILURE) {
        return;
    }

    zval *self = getThis();
    zval *type = zend_read_property(asf_absadapter_ce, self, ZEND_STRL("_type"), 1, NULL);

    if (!asf_db_absadapter_instance(self, Z_STRVAL_P(type), configs)) {
        RETURN_FALSE;
    }
}
/* }}} */

/* {{{ proto int Asf_Db_AbstractAdapter::insert(array $data [, bool $retrun_insert_id = true])
 */
PHP_METHOD(asf_absadapter, insert)
{
    zval *data = NULL;
    zend_bool ret_id = 1;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "a|b", &data, &ret_id) == FAILURE) {
        return;
    }

    if (zend_hash_num_elements(Z_ARRVAL_P(data)) < 1) {
        asf_trigger_error(ASF_ERR_DB_SET_COLUMN, "The first parameter is array elements and more than zero");
        return;
    }

    zval zmn_1, args[1];
    zval *self = getThis();

    ZVAL_STRINGL(&zmn_1, "addByArray", 10);
    ZVAL_COPY_VALUE(&args[0], data);

    call_user_function_ex(&Z_OBJCE_P(self)->function_table, self, &zmn_1, return_value, 1, args, 1, NULL);
    ASF_FAST_STRING_PTR_DTOR(zmn_1);

    (void)asf_db_get_lastid(self, return_value, ret_id);
}
/* }}} */

/* {{{ proto int Asf_Db_AbstractAdapter::insertIgnore(array $data [, bool $retrun_insert_id = true])
*/
PHP_METHOD(asf_absadapter, insertIgnore)
{
    zval *data = NULL;
    zend_bool ret_id = 1;
    zval *self = getThis();

    if (asf_db_is_sqlite(self)) {
        RETURN_FALSE;
    }

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "a|b", &data, &ret_id) == FAILURE) {
        return;
    }

    if (zend_hash_num_elements(Z_ARRVAL_P(data)) < 1) {
        asf_trigger_error(ASF_ERR_DB_SET_COLUMN, "The first parameter is array elements and more than zero");
        return;
    }

    zval zmn_1, args[2];

    ZVAL_STRINGL(&zmn_1, "addByArray", 10);
    ZVAL_COPY_VALUE(&args[0], data);
    ZVAL_BOOL(&args[1], 1);

    call_user_function_ex(&Z_OBJCE_P(self)->function_table, self, &zmn_1, return_value, 2, args, 1, NULL);
    ASF_FAST_STRING_PTR_DTOR(zmn_1);

    (void)asf_db_get_lastid(self, return_value, ret_id);
}
/* }}} */

/* {{{ proto bool Asf_Db_AbstractAdapter::update(array $data, array $condition = array() [, int $limit = 1])
*/
PHP_METHOD(asf_absadapter, update)
{
    zval *data = NULL, *condition = NULL;
    zend_bool limit = 0;
    size_t size_1 = 0, size_2 = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "a|ab", &data, &condition, &limit) == FAILURE) {
        return;
    }

    size_1 = zend_hash_num_elements(Z_ARRVAL_P(data));
    if (size_1 < 1) {
        asf_trigger_error(ASF_ERR_DB_SET_COLUMN, "The first parameter is array elements and more than zero");
        return;
    }

    if (condition) {
        size_2 = zend_hash_num_elements(Z_ARRVAL_P(condition));
    }

    char *sql = NULL;
    zval zmn_1, values, *entry = NULL, args[2], *self = getThis(), *table = NULL;
    zend_string *key = NULL;
    zend_bool mod = 0;
    smart_str col_name = {0}, w_columns = {0};
    size_t len = 0;

    array_init_size(&values, size_1 + size_2);

    ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(data), key, entry) {
        if (!key) {
            continue;
        }
        if (mod) {
            smart_str_appendl(&col_name, ", ", 2);
        }
        smart_str_appendl(&col_name, "`", 1);
        smart_str_appendl(&col_name, ZSTR_VAL(key), ZSTR_LEN(key));
        smart_str_appendl(&col_name, "`", 1);
        smart_str_appendl(&col_name, " = ?", 4);
        mod = 1;

        ZVAL_DEREF(entry);
        Z_TRY_ADDREF_P(entry);
        add_next_index_zval(&values, entry);

    } ZEND_HASH_FOREACH_END();

    mod = 0;

    if (size_2 > 0) {
        ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(condition), key, entry) {
            if (!key) {
                continue;
            }
            if (mod) {
                smart_str_appendl(&w_columns, "AND ", 4);
            }
            smart_str_appendl(&w_columns, "`", 1);
            smart_str_appendl(&w_columns, ZSTR_VAL(key), ZSTR_LEN(key));
            smart_str_appendl(&w_columns, "`", 1);
            smart_str_appendl(&w_columns, " = ?", 4);
            mod = 1;

            ZVAL_DEREF(entry);
            Z_TRY_ADDREF_P(entry);
            add_next_index_zval(&values, entry);

        } ZEND_HASH_FOREACH_END();
    } else {
        /* No update condition */
        smart_str_appendl(&w_columns, "1 = 1", 5);   
    }

    if (limit && !asf_db_is_sqlite(self)) {
        smart_str_appendl(&w_columns, " LIMIT 1", 8);
    }

    smart_str_0(&col_name);
    smart_str_0(&w_columns);

    table = zend_read_property(asf_absadapter_ce, self, ZEND_STRL("_table"), 1, NULL);
    len = spprintf(&sql, 0, "UPDATE `%s` SET %s WHERE %s", Z_STRVAL_P(table), ZSTR_VAL(col_name.s), ZSTR_VAL(w_columns.s));

    smart_str_free(&col_name);
    smart_str_free(&w_columns);

    ZVAL_STRINGL(&zmn_1, "exeQuery", 8);
    ZVAL_STRINGL(&args[0], sql, len);
    ZVAL_COPY_VALUE(&args[1], &values);

    efree(sql); sql = NULL;

    call_user_function_ex(&Z_OBJCE_P(self)->function_table, self, &zmn_1, return_value, 2, args, 1, NULL);
    convert_to_boolean(return_value);

    ASF_FAST_STRING_PTR_DTOR(zmn_1);
    ASF_FAST_STRING_PTR_DTOR(args[0]);
    zval_ptr_dtor(&values);
}
/* }}} */

/* {{{ proto bool Asf_Db_AbstractAdapter::delete(array $condition [, int $limit = 1])
*/
PHP_METHOD(asf_absadapter, delete)
{
    zval *condition = NULL;
    zend_bool limit = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "a|b", &condition, &limit) == FAILURE) {
        return;
    }

    size_t size = zend_hash_num_elements(Z_ARRVAL_P(condition));
    if (size < 1) {
        asf_trigger_error(ASF_ERR_DB_SET_COLUMN, "The first parameter is array elements and more than zero");
        return;
    }

    char *sql = NULL;
    zval zmn_1, values, *entry = NULL, args[2], *self = getThis();
    zend_string *key = NULL;
    zend_bool mod = 0;
    smart_str columns = {0};

    array_init_size(&values, size);

    ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(condition), key, entry) {
        if (!key) {
            continue;
        }
        if (mod) {
            smart_str_appendl(&columns, ", ", 2);
        }
        smart_str_appendl(&columns, "`", 1);
        smart_str_appendl(&columns, ZSTR_VAL(key), ZSTR_LEN(key));
        smart_str_appendl(&columns, "`", 1);
        smart_str_appendl(&columns, " = ?", 4);
        mod = 1;

        ZVAL_DEREF(entry);
        Z_TRY_ADDREF_P(entry);
        add_next_index_zval(&values, entry);

    } ZEND_HASH_FOREACH_END();

    if (limit && !asf_db_is_sqlite(self)) {
        smart_str_appendl(&columns, " LIMIT 1", 8);
    }

    smart_str_0(&columns);

    zval *table = zend_read_property(asf_absadapter_ce, self, ZEND_STRL("_table"), 1, NULL);
    size_t len = spprintf(&sql, 0, "DELETE FROM `%s` WHERE %s", Z_STRVAL_P(table), ZSTR_VAL(columns.s));

    smart_str_free(&columns);

    ZVAL_STRINGL(&zmn_1, "exeNoQuery", 10);
    ZVAL_STRINGL(&args[0], sql, len);
    ZVAL_COPY_VALUE(&args[1], &values);

    efree(sql); sql = NULL;

    call_user_function_ex(&Z_OBJCE_P(self)->function_table, self, &zmn_1, return_value, 2, args, 1, NULL);

    ASF_FAST_STRING_PTR_DTOR(zmn_1);
    ASF_FAST_STRING_PTR_DTOR(args[0]);
    zval_ptr_dtor(&values);
}
/* }}} */

/* {{{ proto array Asf_Db_AbstractAdapter::findOne(string $sql [, array $bind_value = array() [, int $mode = PDO::FETCH_ASSOC]])
*/
PHP_METHOD(asf_absadapter, findOne)
{
    zend_string *sql = NULL;
    zval *bind_value = NULL;
    zend_long mode = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|al", &sql, &bind_value, &mode) == FAILURE) {
        return;
    }

    zval args[4], zmn_1;
    zval *self = getThis();

    ZVAL_STRINGL(&zmn_1, "doQuery", 7);
    ZVAL_STR_COPY(&args[0], sql);
    ZVAL_LONG(&args[2], mode);
    ZVAL_LONG(&args[3], 1);

    if (bind_value && IS_ARRAY == Z_TYPE_P(bind_value) && zend_hash_num_elements(Z_ARRVAL_P(bind_value))) {
        ZVAL_COPY_VALUE(&args[1], bind_value);
    } else {
        ZVAL_NULL(&args[1]);
    }

    call_user_function_ex(&Z_OBJCE_P(self)->function_table, self, &zmn_1, return_value, 4, args, 1, NULL);

    ASF_FAST_STRING_PTR_DTOR(zmn_1);
    ASF_FAST_STRING_PTR_DTOR(args[0]);
}
/* }}} */

/* {{{ proto array Asf_Db_AbstractAdapter::findAll(string $sql [, array $bind_value = array() [, int $mode = PDO::FETCH_ASSOC]])
*/
PHP_METHOD(asf_absadapter, findAll)
{
    zend_string *sql;
    zval *bind_value = NULL;
    zend_long mode = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|al", &sql, &bind_value, &mode) == FAILURE) {
        return;
    }

    zval args[4], zmn_1;
    zval *self = getThis();

    ZVAL_STRINGL(&zmn_1, "doQuery", 7);
    ZVAL_STR_COPY(&args[0], sql);
    ZVAL_LONG(&args[2], mode);
    ZVAL_LONG(&args[3], 0);

    if (bind_value && IS_ARRAY == Z_TYPE_P(bind_value) && zend_hash_num_elements(Z_ARRVAL_P(bind_value))) {
        ZVAL_COPY_VALUE(&args[1], bind_value);
    } else {
        ZVAL_NULL(&args[1]);
    }

    call_user_function_ex(&Z_OBJCE_P(self)->function_table, self, &zmn_1, return_value, 4, args, 1, NULL);
    ASF_FAST_STRING_PTR_DTOR(zmn_1);
    ASF_FAST_STRING_PTR_DTOR(args[0]);
}
/* }}} */

/* {{{ proto array Asf_Db_AbstractAdapter::findOneBy(array $data [, array $bind_value = array() [, int $mode = PDO::FETCH_ASSOC]])
*/
PHP_METHOD(asf_absadapter, findOneBy)
{
    zval *pairs = NULL, *fields_name = NULL;
    zend_long mode = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "a|al", &pairs, &fields_name, &mode) == FAILURE) {
        return;
    }

    (void)asf_db_findby(getThis(), pairs, fields_name, mode, 1, return_value);
}
/* }}} */

/* {{{ proto array Asf_Db_AbstractAdapter::findAllBy(array $data [, array $bind_value = array() [, int $mode = PDO::FETCH_ASSOC]])
*/
PHP_METHOD(asf_absadapter, findAllBy)
{
    zval *pairs = NULL, *fields_name = NULL;
    zend_long mode = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "a|al", &pairs, &fields_name, &mode) == FAILURE) {
        return;
    }

    (void)asf_db_findby(getThis(), pairs, fields_name, mode, 0, return_value);
}
/* }}} */

/* {{{ proto mixed Asf_Db_AbstractAdapter::doQuery(string $sql [, array $bind_value = array() [, int $mode = PDO::FETCH_ASSOC]])
*/
PHP_METHOD(asf_absadapter, doQuery)
{
    zend_string *sql = NULL;
    zval *bind_value = NULL;
    zend_long mode = 0, one = 0;
    size_t size = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|zll", &sql, &bind_value, &mode, &one) == FAILURE) {
        return;
    }

    /* Why you to do this? */
    if (UNEXPECTED(asf_func_isempty(ZSTR_VAL(sql)))) {
        return;
    }

    zval *self = getThis();
    zval zret_1, zmn_1, zmn_2, args[3];
    zval fetch_type;

    /* trace log */
    double start_time = asf_func_trace_gettime();

    if (mode == 0) {
        mode = PDO_FETCH_ASSOC;
    }

    if (bind_value && IS_ARRAY == Z_TYPE_P(bind_value)) {
        size = zend_hash_num_elements(Z_ARRVAL_P(bind_value));
        if (size < 1) {
            zval_ptr_dtor(bind_value);
        }
    }

    ZVAL_STRINGL(&zmn_1, "exeNoQuery", 10);
    ZVAL_STR_COPY(&args[0], sql);

    if (size > 0) {
        ZVAL_COPY_VALUE(&args[1], bind_value);
    } else {
        ZVAL_NULL(&args[1]);
    }
    ZVAL_LONG(&args[2], 1);

    call_user_function_ex(&Z_OBJCE_P(self)->function_table, self, &zmn_1, &zret_1, 3, args, 1, NULL);
    ASF_FAST_STRING_PTR_DTOR(zmn_1);
    ASF_FAST_STRING_PTR_DTOR(args[0]);

    ZVAL_STRING(&zmn_2, (one == 1) ? "fetch" : "fetchAll");
    ZVAL_LONG(&fetch_type, mode);

    /* If FALSE(return_value) is returned on failure */
    call_user_function_ex(&Z_OBJCE_P(&zret_1)->function_table, &zret_1, &zmn_2, return_value, 1, &fetch_type, 1, NULL);

    ASF_FAST_STRING_PTR_DTOR(zmn_2);
    zval_ptr_dtor(&zret_1);

    /* The correct result is an array */
    if (!return_value || Z_TYPE_P(return_value) == IS_FALSE || Z_TYPE_P(return_value) == IS_UNDEF) {
        array_init(return_value);
    }

    (void)asf_func_trace_str_add(ASF_TRACE_MYSQL, start_time, ZSTR_VAL(sql), ZSTR_LEN(sql), size ? 1 : 0, bind_value, return_value);
}
/* }}} */

/* {{{ proto bool Asf_Db_AbstractAdapter::exeNoQuery(string $sql [, array $bind_value = array()])
*/
PHP_METHOD(asf_absadapter, exeNoQuery)
{
    zend_string *sql = NULL;
    zval *bind_value = NULL;
    zend_long ret_obj = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|zl", &sql, &bind_value, &ret_obj) == FAILURE) {
        return;
    }

    if (UNEXPECTED(asf_func_isempty(ZSTR_VAL(sql)))) {
        asf_trigger_error(ASF_ERR_DB_PARAM, "Parameter 'sql' must be a string");
        return;
    }

    zval *dbh = NULL, *self = NULL;
    zval zsql, sth, zret_1, zret_2, zmn_1, zmn_2, args[1];
    uint count = 0;

    self   = getThis();
    dbh    = zend_read_property(asf_absadapter_ce, self, ZEND_STRL("_dbh"), 1, NULL);
    
    /* Add Debug SQL information */
    zend_update_property_str(asf_absadapter_ce, self, ZEND_STRL("_sql"), sql);

    ZVAL_STRINGL(&zmn_1, "prepare", 7);
    ZVAL_STR_COPY(&zsql, sql);

    call_user_function_ex(&Z_OBJCE_P(dbh)->function_table, dbh, &zmn_1, &zret_1, 1, &zsql, 1, NULL);
    ASF_FAST_STRING_PTR_DTOR(zmn_1);
    ASF_FAST_STRING_PTR_DTOR(zsql);

    if (IS_OBJECT != Z_TYPE(zret_1)) {
        asf_trigger_error(ASF_ERR_DB_PREPARE, "Prepare a statement '%s' failed", ZSTR_VAL(sql));
        return;
    }

    if (bind_value && IS_ARRAY == Z_TYPE_P(bind_value) && zend_hash_num_elements(Z_ARRVAL_P(bind_value))) {
        ZVAL_COPY_VALUE(&args[0], bind_value);
        count = 1;
    } else {
        ZVAL_NULL(&args[0]);
    }

    /* Add Debug value information */
    zend_update_property(asf_absadapter_ce, self, ZEND_STRL("_value"), &args[0]);

    /* trace log */
    double start_time = asf_func_gettimeofday();

    ZVAL_STRINGL(&zmn_2, "execute", 7);
    call_user_function_ex(&Z_OBJCE_P(&zret_1)->function_table, &zret_1, &zmn_2, &zret_2, count, args, 1, NULL);
    ASF_FAST_STRING_PTR_DTOR(zmn_2);

    if (EG(exception)) {
        zval_ptr_dtor(&zret_1);
        zend_exception_error(EG(exception), E_ERROR);
        return;
    }

    if (IS_FALSE == Z_TYPE(zret_2)) {
        zval_ptr_dtor(&zret_1);
        asf_trigger_error(ASF_ERR_DB_EXECUTE, "Executes a prepared statement failed");
        return;
    }

    /* Only record the correct log */
    if (ASF_G(log_sql) && ASF_G(log_path)) {
        (void)asf_db_write_logsql(sql, bind_value, start_time);
    }

    if (ret_obj) {
        ZVAL_COPY_VALUE(return_value, &zret_1);
    } else {
        (void)asf_func_trace_str_add(ASF_TRACE_MYSQL, start_time, ZSTR_VAL(sql), ZSTR_LEN(sql), count, bind_value, &zret_2);
        zval_ptr_dtor(&zret_1);
        RETURN_BOOL(&zret_2);
    }
}
/* }}} */

/* {{{ proto int Asf_Db_AbstractAdapter::exeQuery(string $sql [, array $bind_value = array()])
*/
PHP_METHOD(asf_absadapter, exeQuery)
{
    zend_string *sql = NULL;
    zval *bind_value = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|a", &sql, &bind_value) == FAILURE) {
        return;
    }

    zval args[3], zmn_1, zmn_2, zret_1;
    zval *self = getThis();
    double start_time = asf_func_trace_gettime();

    ZVAL_STRINGL(&zmn_1, "exeNoQuery", 10);
    ZVAL_STR(&args[0], sql);
    if (!bind_value) {
        ZVAL_NULL(&args[1]);  
    } else {
        ZVAL_COPY_VALUE(&args[1], bind_value);
    }
    ZVAL_LONG(&args[2], 1);

    call_user_function_ex(&Z_OBJCE_P(self)->function_table, self, &zmn_1, &zret_1, 3, args, 1, NULL);
    ASF_FAST_STRING_PTR_DTOR(zmn_1);

    ZVAL_STRINGL(&zmn_2, "rowCount", 8);
    call_user_function_ex(&Z_OBJCE_P(&zret_1)->function_table, &zret_1, &zmn_2, return_value, 0, 0, 1, NULL);
    ASF_FAST_STRING_PTR_DTOR(zmn_2);
    zval_ptr_dtor(&zret_1);

    (void)asf_func_trace_str_add(ASF_TRACE_MYSQL, start_time, ZSTR_VAL(sql), ZSTR_LEN(sql),
           bind_value ? 1 : 0, &args[1], return_value);
}
/* }}} */

/* {{{ proto bool Asf_Db_AbstractAdapter::addByArray(array $data)
*/
PHP_METHOD(asf_absadapter, addByArray)
{
    zval *pairs = NULL;
    zend_bool ignore = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "a|b", &pairs, &ignore) == FAILURE) {
        return;
    }

    zval zmn_1, values, args[2];
    zend_string *key = NULL;
    zval *entry = NULL, *table = NULL, *self = NULL;
    zend_bool mod = 0;
    size_t size = 0, len = 0;
    smart_str col_name = {0}, qmark = {0};
    char *sql = NULL;

    size = zend_hash_num_elements(Z_ARRVAL_P(pairs));
    if (size < 1) {
        asf_trigger_error(ASF_ERR_DB_SET_COLUMN, "The first parameter is array elements and more than zero");
        return;
    }

    array_init_size(&values, size);

    ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(pairs), key, entry) {
        if (!key) {
            continue;
        }
        if (mod) {
            smart_str_appendl(&col_name, ", ", 2);
            smart_str_appendl(&qmark, ", ", 2);
        }
        smart_str_appendl(&col_name, "`", 1);
        smart_str_appendl(&col_name, ZSTR_VAL(key), ZSTR_LEN(key));
        smart_str_appendl(&col_name, "`", 1);

        smart_str_appendl(&qmark, "?", 1);
        mod = 1;

        ZVAL_DEREF(entry);
        Z_TRY_ADDREF_P(entry);
        add_next_index_zval(&values, entry);

    } ZEND_HASH_FOREACH_END();
    smart_str_0(&col_name);
    smart_str_0(&qmark);

    self = getThis();
    table = zend_read_property(asf_absadapter_ce, self, ZEND_STRL("_table"), 1, NULL);
    len = spprintf(&sql, 0, "INSERT %s INTO `%s`(%s) VALUES (%s)", (ignore ? "IGNORE" : ""), Z_STRVAL_P(table), ZSTR_VAL(col_name.s), ZSTR_VAL(qmark.s));
    smart_str_free(&col_name);
    smart_str_free(&qmark);

    ZVAL_STRINGL(&zmn_1, "exeNoQuery", 10);
    ZVAL_STRINGL(&args[0], sql, len);
    ZVAL_COPY_VALUE(&args[1], &values);

    efree(sql); sql = NULL;

    call_user_function_ex(&Z_OBJCE_P(self)->function_table, self, &zmn_1, return_value, 2, args, 1, NULL);

    ASF_FAST_STRING_PTR_DTOR(zmn_1);
    ASF_FAST_STRING_PTR_DTOR(args[0]);
    zval_ptr_dtor(&values);
}
/* }}} */

/* {{{ proto bool Asf_Db_AbstractAdapter::addDupByArray(array $data)
*/
PHP_METHOD(asf_absadapter, addDupByArray)
{
    zval *pairs = NULL, *update_cols = NULL;
    zval *self = getThis();

    if (asf_db_is_sqlite(self)) {
        RETURN_FALSE;
    }

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "aa", &pairs, &update_cols) == FAILURE) {
        return;
    }

    zval zmn_1, values, args[2];
    zend_string *key = NULL;
    zval *entry = NULL, *table = NULL;
    zend_bool mod = 0;
    size_t size_1 = 0, size_2 = 0, len = 0;
    smart_str col_name = {0}, up_columns = {0}, qmark = {0};;
    char *sql = NULL;

    size_1 = zend_hash_num_elements(Z_ARRVAL_P(pairs));
    if (size_1 < 1) {
        asf_trigger_error(ASF_ERR_DB_SET_COLUMN, "The first parameter is array elements and more than zero");
        return;
    }

    size_2 = zend_hash_num_elements(Z_ARRVAL_P(update_cols));
    if (size_2 < 1) {
        asf_trigger_error(ASF_ERR_DB_SET_COLUMN, "The second parameter two is array elements and more than zero");
        return;
    }

    array_init_size(&values, size_1 + size_2);

    ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(pairs), key, entry) {
        if (!key) {
            continue;
        }
        if (mod) {
            smart_str_appendl(&col_name, ", ", 2);
            smart_str_appendl(&qmark, ", ", 2);
        }
        smart_str_appendl(&col_name, "`", 1);
        smart_str_appendl(&col_name, ZSTR_VAL(key), ZSTR_LEN(key));
        smart_str_appendl(&col_name, "`", 1);
        //smart_str_appendl(&col_name, " = ?", 4);

        smart_str_appendl(&qmark, "?", 1);
        mod = 1;

        ZVAL_DEREF(entry);
        Z_TRY_ADDREF_P(entry);
        add_next_index_zval(&values, entry);

    } ZEND_HASH_FOREACH_END();
    smart_str_0(&col_name);
    smart_str_0(&qmark);

    mod = 0;
    ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(update_cols), key, entry) {
        if (!key) {
            continue;
        }
        if (mod) {
            smart_str_appendl(&up_columns, ", ", 2);
        }
        smart_str_appendl(&up_columns, "`", 1);
        smart_str_appendl(&up_columns, ZSTR_VAL(key), ZSTR_LEN(key));
        smart_str_appendl(&up_columns, "`", 1);
        smart_str_appendl(&up_columns, " = ?", 4);
        mod = 1;

        ZVAL_DEREF(entry);
        Z_TRY_ADDREF_P(entry);
        add_next_index_zval(&values, entry);

    } ZEND_HASH_FOREACH_END();
    smart_str_0(&up_columns);

    table = zend_read_property(asf_absadapter_ce, self, ZEND_STRL("_table"), 1, NULL);
    len = spprintf(&sql, 0, "INSERT INTO `%s`(%s) VALUES (%s) ON DUPLICATE KEY UPDATE %s", Z_STRVAL_P(table), ZSTR_VAL(col_name.s), ZSTR_VAL(qmark.s), ZSTR_VAL(up_columns.s));
    smart_str_free(&col_name);
    smart_str_free(&qmark);
    smart_str_free(&up_columns);

    ZVAL_STRINGL(&zmn_1, "exeNoQuery", 10);
    ZVAL_STRINGL(&args[0], sql, len);
    ZVAL_COPY_VALUE(&args[1], &values);

    efree(sql); sql = NULL;

    call_user_function_ex(&Z_OBJCE_P(self)->function_table, self, &zmn_1, return_value, 2, args, 1, NULL);

    ASF_FAST_STRING_PTR_DTOR(zmn_1);
    ASF_FAST_STRING_PTR_DTOR(args[0]);
    zval_ptr_dtor(&values);
}
/* }}} */

/* {{{ proto int Asf_Db_AbstractAdapter::getCount([string $sql_where = '', array $bind_value = array()])
*/
PHP_METHOD(asf_absadapter, getCount)
{
    zend_string *sql_where = NULL;
    zval *bind_value = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|z", &sql_where, &bind_value) == FAILURE) {
        return;
    }

    char *sql = NULL;
    zval args[4], zmn_1, ret;
    zval *self = getThis();

    zval *table = zend_read_property(asf_absadapter_ce, self, ZEND_STRL("_table"), 1, NULL);
    size_t len = spprintf(&sql, 0, "SELECT count(*) as asf_cnt FROM `%s` WHERE %s", Z_STRVAL_P(table), ZSTR_VAL(sql_where));

    ZVAL_STRINGL(&zmn_1, "doQuery", 7);
    ZVAL_STRINGL(&args[0], sql, len);
    if (bind_value && IS_ARRAY == Z_TYPE_P(bind_value) && zend_hash_num_elements(Z_ARRVAL_P(bind_value))) {
        ZVAL_COPY_VALUE(&args[1], bind_value);
    } else {
        ZVAL_NULL(&args[1]);
    }
    
    ZVAL_LONG(&args[2], 0);
    ZVAL_LONG(&args[3], 1);

    call_user_function_ex(&Z_OBJCE_P(self)->function_table, self, &zmn_1, &ret, 4, args, 1, NULL);

    efree(sql); sql = NULL;
    ASF_FAST_STRING_PTR_DTOR(zmn_1);
    ASF_FAST_STRING_PTR_DTOR(args[0]);

    if (IS_ARRAY == Z_TYPE(ret)) {
        convert_to_long(return_value);
        Z_LVAL_P(return_value) = zval_get_long(zend_hash_str_find(Z_ARRVAL(ret), "asf_cnt", 7));
        zval_ptr_dtor(&ret);
    } else {
        RETURN_LONG(0);
    }
}
/* }}} */

/* {{{ proto bool Asf_Db_AbstractAdapter::setTable(string $name)
*/
PHP_METHOD(asf_absadapter, setTable)
{
    zval *name = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &name) == FAILURE) {
        return;
    }

    if ((IS_LONG != Z_TYPE_P(name) && IS_STRING != Z_TYPE_P(name)) ||
            (IS_STRING == Z_TYPE_P(name) && UNEXPECTED(asf_func_isempty(Z_STRVAL_P(name))))) {
        asf_trigger_error(ASF_ERR_DB_TABLE, "Table name must be a string or integer");
        return;
    }

    zend_update_property(asf_absadapter_ce, getThis(), ZEND_STRL("_table"), name);

    RETURN_TRUE;
}
/* }}} */

/* {{{ proto string Asf_Db_AbstractAdapter::getTable(void)
*/
PHP_METHOD(asf_absadapter, getTable)
{
    ZVAL_COPY(return_value, zend_read_property(asf_absadapter_ce,
                getThis(), ZEND_STRL("_table"), 1, NULL));
}
/* }}} */

/* {{{ proto string Asf_Db_AbstractAdapter::getLastSql(void)
*/
PHP_METHOD(asf_absadapter, getLastSql)
{
    zval *self = getThis();

    array_init(return_value);

    zval *sql   = zend_read_property(asf_absadapter_ce, self, ZEND_STRL("_sql"), 1, NULL);
    zval *value = zend_read_property(asf_absadapter_ce, self, ZEND_STRL("_value"), 1, NULL);

    add_assoc_zval_ex(return_value, "sql", 3, sql);
    add_assoc_zval_ex(return_value, "value", 5, value);

    Z_TRY_ADDREF_P(sql);
    Z_TRY_ADDREF_P(value);
}
/* }}} */

/* {{{ proto mixed Asf_Db_AbstractAdapter::__call(string $function_name, array $args)
*/
ASF_METHOD_CALL(ASF_TRACE_MYSQL, asf_absadapter, "_dbh")
/* }}} */

/* {{{ proto bool Asf_Db_AbstractAdapter::close(void)
*/
PHP_METHOD(asf_absadapter, close)
{
    zval *pdo = zend_read_property(asf_absadapter_ce, getThis(), ZEND_STRL("_dbh"), 1, NULL);
    if (Z_TYPE_P(pdo) == IS_OBJECT) {
        zend_update_property_null(asf_absadapter_ce, getThis(), ZEND_STRL("_dbh"));
    }

    RETURN_TRUE;
}
/* }}} */

/* {{{ asf_absadapter_methods[]
*/
zend_function_entry asf_absadapter_methods[] = {
    PHP_ME(asf_absadapter, __construct, asf_db_adapterinterface_construct_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_MALIAS(asf_absadapter, init, __construct, asf_db_adapterinterface_construct_arginfo, ZEND_ACC_PUBLIC)

    PHP_ME(asf_absadapter, insert, asf_db_adapterinterface_insert_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(asf_absadapter, insertIgnore, asf_db_adapterinterface_insert_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(asf_absadapter, update, asf_db_adapterinterface_update_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(asf_absadapter, delete, asf_db_adapterinterface_delete_arginfo, ZEND_ACC_PUBLIC)

    PHP_ME(asf_absadapter, findOne, asf_db_adapterinterface_findone_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(asf_absadapter, findAll, asf_db_adapterinterface_findone_arginfo, ZEND_ACC_PUBLIC)

    PHP_MALIAS(asf_absadapter, getRowByCmd, findOne, asf_db_adapterinterface_findone_arginfo, ZEND_ACC_PUBLIC)
    PHP_MALIAS(asf_absadapter, getRowsByCmd, findAll, asf_db_adapterinterface_findone_arginfo, ZEND_ACC_PUBLIC)

    PHP_ME(asf_absadapter, findOneBy, asf_db_adapterinterface_findby_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(asf_absadapter, findAllBy, asf_db_adapterinterface_findby_arginfo, ZEND_ACC_PUBLIC)

    PHP_ME(asf_absadapter, doQuery, asf_absadapter_doquery_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(asf_absadapter, exeNoQuery, asf_db_adapterinterface_query_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(asf_absadapter, exeQuery, asf_db_adapterinterface_query_arginfo, ZEND_ACC_PUBLIC)

    PHP_ME(asf_absadapter, addByArray, asf_db_adapterinterface_addbyarray_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(asf_absadapter, addDupByArray, asf_db_adapterinterface_adddupbyarray_arginfo, ZEND_ACC_PUBLIC)

    PHP_ME(asf_absadapter, getCount, asf_db_adapterinterface_getcount_arginfo, ZEND_ACC_PUBLIC)

    PHP_ME(asf_absadapter, setTable, asf_db_adapterinterface_settable_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(asf_absadapter, getTable, NULL, ZEND_ACC_PUBLIC)

    PHP_ME(asf_absadapter, getLastSql, NULL, ZEND_ACC_PUBLIC)

    PHP_ME(asf_absadapter, __call, asf_absadapter_call_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(asf_absadapter, close, NULL, ZEND_ACC_PUBLIC)
    PHP_FE_END
};
/* }}} */

ASF_INIT_CLASS(db_abstractadapter) /* {{{ */
{
    ASF_REGISTER_CLASS_PARENT(asf_absadapter, Asf_Db_AbstractAdapter, Asf\\Db\\AbstractAdapter, ZEND_ACC_EXPLICIT_ABSTRACT_CLASS);

    zend_declare_property_null(asf_absadapter_ce, ZEND_STRL("_type"),  ZEND_ACC_PROTECTED);
    zend_declare_property_null(asf_absadapter_ce, ZEND_STRL("_dbh"),   ZEND_ACC_PROTECTED);
    //zend_declare_property_null(asf_absadapter_ce, ZEND_STRL("_logh"),  ZEND_ACC_PROTECTED);
    zend_declare_property_string(asf_absadapter_ce, ZEND_STRL("_table"), "", ZEND_ACC_PROTECTED);
    
    zend_declare_property_string(asf_absadapter_ce, ZEND_STRL("_sql"), "", ZEND_ACC_PROTECTED);
    zend_declare_property_string(asf_absadapter_ce, ZEND_STRL("_value"), "", ZEND_ACC_PROTECTED);

    zend_class_implements(asf_absadapter_ce, 1, asf_db_adapterinterface_ce);

    return SUCCESS;
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
