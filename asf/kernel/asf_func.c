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
#include "asf_exception.h"

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
            ZVAL_DEREF(param);
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

void asf_func_add_trace() /*{{{*/
{
    /*
    if (ASF_G(trace_enable)) {
        if (Z_TYPE(ASF_G(trace_buf)) != IS_ARRAY) {
            array_init(&ASF_G(trace_buf));
        }

        zval line;
        array_init(&line);

        add_assoc_str_ex(&line, "s", 1, sql);
        if (bind_value) {
            Z_TRY_ADDREF_P(bind_value);
            add_assoc_zval_ex(&line, "v", 1, bind_value);
        }
        add_assoc_double_ex(&line, "t", 1, exec_time);
        add_assoc_zval_ex(&line, "r", 1, &zret_2); 
        add_next_index_zval(&ASF_G(trace_buf), &line);
    }
*/
}
/* }}} */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
