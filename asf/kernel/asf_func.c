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

#ifndef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_asf.h"
#include "asf_func.h"
#include "Zend/zend_stream.h"
#include "ext/standard/php_filestat.h" /* php_stat */
#include "zend_smart_str.h" /* smart_str */
#include "ext/standard/php_string.h" /* php_basename */
#include "ext/standard/php_math.h" /* _php_math_number_format */
#include "asf_exception.h"
#include <sys/time.h> /* gettimeofday */

const char *TRACE_NAME[] = {"MySQL", "Redis", "Memcached", "PgSQL", "SQLite", "CURL", "SCRIPT"};

_Bool asf_func_isempty(const char *s) /* {{{ */
{
    register const char *c = s;

    while (*c != '\0') {
        if (!isspace(*c)) {
            return 0;
        }
        c++;
    }
    return 1;
}
/* }}} */

void asf_func_set_cur_module(char *module) /* {{{ */
{
    char *path = NULL;
    size_t path_len = 0;

    path_len = spprintf(&path, 0, "%s%c%s%c", ZSTR_VAL(ASF_G(root_path)), DEFAULT_SLASH, module, DEFAULT_SLASH);

    if (EXPECTED(path_len)) {
        ASF_G(root_path_route) = zend_string_init(path, path_len, 0);
    }

    efree(path);
}
/* }}} */

php_stream *asf_func_fopen(const char *fpath, size_t fpath_len, zend_string *dpath) /* {{{ */
{
    zval exists_flag;
    char *dirname = NULL;
    size_t dirname_len = 0;
    int ret = 1;

    if (dpath == NULL) {
        dirname = estrndup(fpath, fpath_len);
        dirname_len = php_dirname(dirname, fpath_len);
    } else {
        dirname = ZSTR_VAL(dpath);
        dirname_len = ZSTR_LEN(dpath);
    }

    php_stat(dirname, dirname_len, FS_IS_DIR, &exists_flag);

    if (Z_TYPE(exists_flag) == IS_FALSE) {
        ret = php_stream_mkdir(dirname, 0755, PHP_STREAM_MKDIR_RECURSIVE, NULL);
    }

    if (dpath == NULL) {
        efree(dirname);
    }

    if (!ret) {
        asf_trigger_error(ASF_ERR_LOGGER_STREAM_ERROR, "Directory mkdir failed: %s", fpath);
        return NULL;
    }

    /* If the file does not exist, attempt to create it. */
    const char *mode = "ab";
    php_stream *stream = php_stream_fopen(fpath, mode, NULL);

    if (NULL == stream) {
        asf_trigger_error(ASF_ERR_LOGGER_STREAM_ERROR, "File open failed: %s", fpath);
    }

    return stream;
}
/* }}} */

_Bool asf_func_shutdown_buffer() /* {{{ */
{
    if (!ASF_G(use_lcache) || IS_ARRAY != Z_TYPE(ASF_G(log_buffer))) {
        return 0;
    }

    zend_string *key = NULL;
    zval *entry_1 = NULL, *entry_2 = NULL;
    php_stream *stream = NULL;
    HashTable *ht = Z_ARRVAL(ASF_G(log_buffer));

    /* ['file_path1' =>  [xx, yyy], 'file_path2' => [xxx, yyy]] */
    ZEND_HASH_FOREACH_STR_KEY_VAL(ht, key, entry_1) {
        if (NULL == (stream = asf_func_fopen(ZSTR_VAL(key), ZSTR_LEN(key), NULL))) {
            /* Close Logger Buffer, Write directly to a local file */
            ASF_G(use_lcache) = 0;
            php_error_docref(NULL, E_WARNING, "File open failed: %s", ZSTR_VAL(key));
            continue;
        }

        ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(entry_1), entry_2) {
            php_stream_write(stream, Z_STRVAL_P(entry_2), Z_STRLEN_P(entry_2));
        } ZEND_HASH_FOREACH_END();

        php_stream_close(stream);
        stream = NULL;
    } ZEND_HASH_FOREACH_END();

    zval_ptr_dtor(&ASF_G(log_buffer));
    ZVAL_UNDEF(&ASF_G(log_buffer));

    /* Restore settings */
    ASF_G(use_lcache) = 1;

    return 1;
}
/* }}} */

void asf_func_config_persistent_hash_destroy(HashTable *ht) /* {{{ */
{
    zend_string *key = NULL;
    zval *element = NULL;

    if (((ht)->u.flags & HASH_FLAG_INITIALIZED)) {
        ZEND_HASH_FOREACH_STR_KEY_VAL(ht, key, element) {
            if (key) {
                free(key);
            }
            switch (Z_TYPE_P(element)) {
                case IS_PTR:
                case IS_STRING:
                    free(Z_PTR_P(element));
                    break;
                case IS_ARRAY:
                    (void)asf_func_config_persistent_hash_destroy(Z_ARRVAL_P(element));
                    break;
            }
        } ZEND_HASH_FOREACH_END();
        free(HT_GET_DATA_ADDR(ht));
    }
    free(ht);
}
/* }}} */

int asf_func_array_isset(const zval *arr, const zval *index) /* {{{ */
{
    HashTable *ht = NULL;

    if (Z_TYPE_P(arr) != IS_ARRAY) {
        return 0;
    }

    ht = Z_ARRVAL_P(arr);
    switch (Z_TYPE_P(index)) {
        case IS_DOUBLE:
            return zend_hash_index_exists(ht, (ulong)Z_DVAL_P(index));
            
        case IS_LONG:
            return zend_hash_index_exists(ht, Z_LVAL_P(index));
           
        case IS_STRING:
            return zend_symtable_exists(ht, Z_STR_P(index));
           
        default:
            zend_error(E_WARNING, "Illegal offset type");
            return 0;
    }
}
/* }}} */

int asf_func_array_del(const zval *arr, const zval *index) /* {{{ */
{
    HashTable *ht = NULL;

    if (Z_TYPE_P(arr) != IS_ARRAY) {
        return 0;
    }

    ht = Z_ARRVAL_P(arr);
    switch (Z_TYPE_P(index)) {
        case IS_DOUBLE:
            return zend_hash_index_del(ht, (ulong)Z_DVAL_P(index));

        case IS_LONG:
            return zend_hash_index_del(ht, Z_LVAL_P(index));

        case IS_STRING:
            return zend_symtable_del(ht, Z_STR_P(index));

        default:
            zend_error(E_WARNING, "Illegal offset type");
            return 0;
    }
}
/* }}} */

zval *asf_func_array_fetch(const zval *arr, const zval *index) /* {{{ */
{
    HashTable *ht = NULL;

    if (Z_TYPE_P(arr) != IS_ARRAY) {
        return NULL;
    }

    ht = Z_ARRVAL_P(arr);
    switch (Z_TYPE_P(index)) {
        case IS_DOUBLE:
            return zend_hash_index_find(ht, (ulong)Z_DVAL_P(index));

        case IS_LONG:
            return zend_hash_index_find(ht, Z_LVAL_P(index));

        case IS_STRING:
            return zend_symtable_find(ht, Z_STR_P(index));

        default:
            zend_error(E_WARNING, "Illegal offset type");
            return NULL;

    }
}
/* }}} */

zend_class_entry *asf_find_driver(const char *class_name, unsigned int class_name_len) /*{{{*/
{
    zend_class_entry *ce = NULL;

    if (UNEXPECTED((ce = zend_hash_str_find_ptr(CG(class_table), class_name, class_name_len)) == NULL)) {
        asf_trigger_error(ASF_ERR_CACHE_MODULE, "The '%s' not supported", class_name);
        return NULL;
    }

    return ce;
}/*}}}*/

void asf_func_format_args(zval *args, zval **retval, size_t *arg_count) /*{{{*/
{
    HashTable *ht = Z_ARRVAL_P(args);
    *arg_count = zend_hash_num_elements(ht);

    if (*arg_count > 0) {
        zval *param = NULL;
        int i = 0;
        *retval = safe_emalloc(sizeof(zval), *arg_count, 0);

        ZEND_HASH_FOREACH_VAL(ht, param) {
            ZVAL_COPY_VALUE(&((*retval)[i++]), param);
        } ZEND_HASH_FOREACH_END();
    }
}/*}}}*/

double asf_func_gettimeofday() /* {{{ */
{
    struct timeval tp = {0};

    if (gettimeofday(&tp, NULL)) {
        return 0;
    }

    return (double)(tp.tv_sec + tp.tv_usec / 1000000.00);
}
/* }}} */

void asf_func_trace_zval_add(uint trace_id, double start_time, zval *method,
        uint32_t param_count, zval params[], zval *retval) /*{{{*/
{
    if (!ASF_G(trace_enable)) {
        return;
    }

    double exec_time = asf_func_gettimeofday() - start_time;

    /* Limit length */
    if (Z_TYPE(ASF_G(trace_buf)) == IS_ARRAY) {
        if (zend_hash_num_elements(Z_ARRVAL(ASF_G(trace_buf))) > 100) {
            asf_func_trace_clear();
            goto init;
        }
    } else {
init:
        array_init(&ASF_G(trace_buf));
    }

    zval line;
    array_init(&line);
    Z_TRY_ADDREF_P(method);

    /**
     * i => id (enum TRACE_ID)
     * s => sql
     * v => value
     * t => execute_time
     * r => result
     */
    add_assoc_long_ex(&line, "i", 1, trace_id);
    add_assoc_zval_ex(&line, "s", 1, method);

    do {
        if (param_count < 1) {
            add_assoc_null_ex(&line, "v", 1);
            break;
        }

        zval regs; uint i = 0;
        array_init(&regs);

        if (param_count == 1) {
            Z_TRY_ADDREF_P(params);
            zend_hash_next_index_insert_new(Z_ARRVAL(regs), &params[i]);
        } else {
            while (param_count--) {
                Z_TRY_ADDREF(params[i]);
                zend_hash_next_index_insert_new(Z_ARRVAL(regs), &params[i]);
                i++;
            }
        }
        add_assoc_zval_ex(&line, "v", 1, &regs);
    } while (0);
    
    /* 6 decimal places */
    add_assoc_str_ex(&line, "t", 1, _php_math_number_format(exec_time, 5, '.', ' '));
    /* Exclude 'retval' return value 'UNKNOWN:0' and 'NULL' */
    if (retval && !Z_ISUNDEF_P(retval)) {
        Z_TRY_ADDREF_P(retval);
        add_assoc_zval_ex(&line, "r", 1, retval);
    } else {
        add_assoc_null_ex(&line, "r", 1);
    }
    add_next_index_zval(&ASF_G(trace_buf), &line);
}
/* }}} */

void asf_func_trace_str_add(uint trace_id, double start_time, char *method, size_t method_len,
        uint32_t param_count, zval params[], zval *retval) /*{{{*/
{
    zval line, name;

    ZVAL_STRINGL(&name, method, method_len);
    asf_func_trace_zval_add(trace_id, start_time, &name, param_count, params, retval);
    ASF_FAST_STRING_PTR_DTOR(name);
}
/* }}} */

double asf_func_trace_gettime() /* {{{ */
{
    return asf_func_gettimeofday();
}
/* }}} */

_Bool asf_func_trace_clear() /* {{{ */
{
    if (Z_TYPE(ASF_G(trace_buf)) == IS_ARRAY) {
        zval_ptr_dtor(&ASF_G(trace_buf));
        ZVAL_UNDEF(&ASF_G(trace_buf));
        return 1;
    }
    return 0;
}
/* }}} */

/* Alarm call user func */
void asf_func_call_user_alarm_func(zend_long err_no, zend_string *err_str, zend_string *err_file, zend_long err_line) /* {{{ */
{
    zval error_retval, params[4];
    ZVAL_LONG(&params[0], err_no);
    ZVAL_STR(&params[1], err_str);
    ZVAL_STR(&params[2], err_file);
    ZVAL_LONG(&params[3], err_line);
    if (call_user_function_ex(CG(function_table), NULL, &ASF_G(err_handler_func), &error_retval, 4, params, 1, NULL) == SUCCESS) {
        zval_ptr_dtor(&error_retval);
    }
}
/* }}} */

/* Alarm Stats, Errno Range: 970 ~ 989 */
_Bool asf_func_alarm_stats(uint trace_id, double start_time, char *method, zval *params, zval *this_ptr) /* {{{ */
{
    if (ASF_G(cli) || Z_ISUNDEF(ASF_G(err_handler_func))) {
        return 0;
    }

    double max_exec_time = 0, exec_time = 0;

    switch (trace_id) {
        case ASF_TRACE_SCRIPT:
            max_exec_time = ASF_G(max_script_time);
            break;
        case ASF_TRACE_REDIS:
        case ASF_TRACE_MEMCACHED:
            max_exec_time = ASF_G(max_cache_time);
            break;
        case ASF_TRACE_MYSQL:
        case ASF_TRACE_PGSQL:
        case ASF_TRACE_SQLITE:
            max_exec_time = ASF_G(max_db_time);
            break;
        case ASF_TRACE_CURL:
            max_exec_time = ASF_G(max_curl_time);
            break;
        default:
            max_exec_time = ASF_G(max_script_time);
            break;
    }

    exec_time = asf_func_gettimeofday() - start_time;

    if ((exec_time - max_exec_time) > 0.000001) {
        char *errmsg = NULL; size_t errmsg_len = 0;
        
        switch (trace_id) {
            /* RSHUTDOWN */
            case ASF_TRACE_SCRIPT:
                errmsg_len = spprintf(&errmsg, 0, "%s executing too slow %f sec", ZSTR_VAL(ASF_G(settled_uri)), exec_time);
                break;

            case ASF_TRACE_REDIS:
            case ASF_TRACE_MEMCACHED:
                {
                    /* No parameters */
                    char *host = NULL; zend_long port = 6379;
                    zval *connect_info = zend_read_property(Z_OBJCE_P(this_ptr), this_ptr, ZEND_STRL(ASF_FUNC_PRONAME_CONNECT_INFO), 1, NULL);
                    zend_string *key = (params) ? zval_get_string(params) : zend_string_init("void", 4, 0);

                    /* Fault tolerance mechanism */
                    if (UNEXPECTED(!Z_ISNULL_P(connect_info))) {
                        host = Z_STRVAL_P(zend_hash_str_find(Z_ARRVAL_P(connect_info), "host", 4));
                        port = zval_get_long(zend_hash_str_find(Z_ARRVAL_P(connect_info), "port", 4));
                    } else {
                        host = "Unknown";
                    }

                    errmsg_len = spprintf(&errmsg, 0, "%s:%d %s::%s(%s) executing too slow %f sec",
                            host, port, TRACE_NAME[trace_id], method, ZSTR_VAL(key), exec_time);
                    zend_string_release(key);
                }
                break;

            case ASF_TRACE_MYSQL:
            case ASF_TRACE_PGSQL:
            case ASF_TRACE_SQLITE:
                {
                    /* IS_STRING */
                    zval *connect_info = zend_read_property(Z_OBJCE_P(this_ptr), this_ptr, ZEND_STRL(ASF_FUNC_PRONAME_CONNECT_INFO), 1, NULL);
                    zend_string *key = (params) ? zval_get_string(params) : zend_string_init("void", 4, 0);
                    errmsg_len = spprintf(&errmsg, 0, "%s %s:%s(%s) executing too slow %f sec",
                            Z_STRVAL_P(connect_info), TRACE_NAME[trace_id], method, ZSTR_VAL(key), exec_time);
                    zend_string_release(key);
                }
                break;

            case ASF_TRACE_CURL:
                errmsg_len = spprintf(&errmsg, 0, "CURL::%s(%s) executing too slow %f sec", method, Z_STRVAL_P(params), exec_time);
                break;
        }

        zend_string *err_str = zend_string_init(errmsg, errmsg_len, 0);
        zend_string *err_file = ZSTR_EMPTY_ALLOC();

        (void)asf_func_call_user_alarm_func(970 + trace_id, err_str, err_file, 0);

        efree(errmsg);
        zend_string_release(err_str);
        zend_string_release(err_file);

        return 1;
    }

    return 0;
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
