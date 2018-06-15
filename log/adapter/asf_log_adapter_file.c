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
#include "main/SAPI.h"
#include "zend_smart_str.h"
#include "Zend/zend_interfaces.h" /* zend_call_ */
#include "ext/standard/php_filestat.h" /* php_stat */
#include "ext/standard/php_fopen_wrappers.h" /* expand_filepath,php_stream_* */
#include "ext/standard/php_string.h" /* memcpy */
#include "ext/standard/basic_functions.h"

#include "php_asf.h"
#include "asf_namespace.h"
#include "asf_exception.h"
#include "asf_log_adapter_file.h"
#include "log/asf_log_adapter.h"
#include "log/formatter/asf_log_formatter_file.h"
#include "kernel/asf_func.h"

zend_class_entry *asf_log_adapter_file_ce;

/* {{{ ARG_INFO
*/
ZEND_BEGIN_ARG_INFO_EX(asf_log_adapter_file_construct_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, file_name)
    ZEND_ARG_INFO(0, file_path)
    ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(asf_log_adapter_file_dolog_arginfo, 0, 0, 3)
    ZEND_ARG_INFO(0, level)
    ZEND_ARG_INFO(0, time)
    ZEND_ARG_INFO(0, message)
ZEND_END_ARG_INFO()
/* }}}*/

void asf_log_adapter_file_instance(asf_logger_t *this_ptr, zend_string *file_name, zend_string *file_path) /* {{{ */
{
    if (!this_ptr || Z_TYPE_P(this_ptr) != IS_OBJECT) {
        object_init_ex(this_ptr, asf_log_adapter_file_ce);
    }

    if (!ASF_G(log_path) && !file_path) {
        asf_trigger_error(ASF_ERR_NOTFOUND_LOGGER_PATH, "Parameter 'file_path/asf.log_path' must be a string");
        return;
    }

    zend_update_property_str(asf_log_adapter_file_ce, this_ptr
            , ZEND_STRL(ASF_LOG_ADAPTER_FILE_PRONAME_FILE_PATH), file_path ? file_path : ASF_G(log_path));
    zend_update_property_str(asf_log_adapter_file_ce, this_ptr, ZEND_STRL(ASF_LOG_ADAPTER_FILE_PRONAME_FILE_NAME), file_name);

    /* load local file */
    if (!ASF_G(use_lcache)) {
        php_stream *stream = NULL;
        char *fpath = NULL;
        int fpath_len = 0;
        zval zstream;

        if (file_path == NULL) {
            fpath_len = spprintf(&fpath, 0, "%s%c%s", ZSTR_VAL(ASF_G(log_path)), DEFAULT_SLASH, ZSTR_VAL(file_name));
            stream = asf_func_fopen(fpath, fpath_len, ASF_G(log_path), 1);
        } else {
            fpath_len = spprintf(&fpath, 0, "%s%c%s", ZSTR_VAL(file_path), DEFAULT_SLASH, ZSTR_VAL(file_name));
            stream = asf_func_fopen(fpath, fpath_len, file_path, 1);
        }

        if (stream) {
            php_stream_to_zval(stream, &zstream);
            zend_update_property(Z_OBJCE_P(this_ptr), this_ptr, ZEND_STRL(ASF_LOG_ADAPTER_FILE_PRONAME_STREAM), &zstream);
        }

        ASF_FUNC_REGISTER_SHUTDOWN_FUNCTION_CLOSE(this_ptr, 1);

        efree(fpath);
    }
}
/* }}} */

static _Bool asf_func_buffer_cache(zval *data, const char *path, const char *fname) /* {{{ */
{
    zval *zbuf = NULL;
    zval new_array;
    char *fpath = NULL;
    size_t fpath_len = 0;

    fpath_len = spprintf(&fpath, 0, "%s%c%s", path, DEFAULT_SLASH, fname);

    if (IS_ARRAY != Z_TYPE(ASF_G(log_buffer))) {
        array_init(&new_array);
        Z_TRY_ADDREF_P(data);
        add_next_index_zval(&new_array, data);
        
        array_init(&ASF_G(log_buffer));
        Z_TRY_ADDREF(new_array);

        add_assoc_zval_ex(&ASF_G(log_buffer), fpath, fpath_len, &new_array);
    } else {
        //zend_hash_str_find_ptr
        if ((zbuf = zend_hash_str_find_ptr(Z_ARRVAL(ASF_G(log_buffer)), fpath, fpath_len)) != NULL) {
            Z_TRY_ADDREF_P(data);
            add_next_index_zval((void *)&zbuf, data);
        } else {
            array_init(&new_array);
            Z_TRY_ADDREF_P(data);
            add_next_index_zval(&new_array, data);

            Z_TRY_ADDREF(new_array);
            add_assoc_zval_ex(&ASF_G(log_buffer), fpath, fpath_len, &new_array);
        }
    }

    if (IS_ARRAY == Z_TYPE(new_array)) {
        zval_ptr_dtor(&new_array);
    }

    efree(fpath);

    return 1;
}
/* }}} */

/* {{{ proto object Asf_Log_Adapter_File::__construct(string $file_name [, string $file_path = ''])
*/
PHP_METHOD(asf_log_adapter_file, __construct)
{
    zend_string *file_name = NULL, *file_path = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|S", &file_name, &file_path) == FAILURE) {
        return;
    }

    (void)asf_log_adapter_file_instance(getThis(), file_name, file_path);
}
/* }}} */

/* {{{ proto object Asf_Log_Adapter_File::getFormatter(void)
*/
PHP_METHOD(asf_log_adapter_file, getFormatter)
{
    zval *zformatter = zend_read_property(asf_log_adapter_file_ce, getThis(), ZEND_STRL(ASF_LOG_ADAPTER_PRONAME_FORMATTER), 1, NULL);

    if (IS_OBJECT == Z_TYPE_P(zformatter)) {
        RETURN_ZVAL(zformatter, 1, 0);
    }

    object_init_ex(return_value, asf_log_formatter_file_ce);

    zend_update_property(asf_log_adapter_file_ce, getThis(), ZEND_STRL(ASF_LOG_ADAPTER_PRONAME_FORMATTER), return_value);
}
/* }}} */

/* {{{ proto bool Asf_Log_Adapter_File::doLog(string $level, int time, string $message)
*/
PHP_METHOD(asf_log_adapter_file, doLog)
{
    zval *message = NULL, *level = NULL;
    zend_long time = 0;

    _Bool ret = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "zlz", &level, &time, &message) == FAILURE) {
        return;
    }

    zval *self = getThis();

    zval *path = zend_read_property(asf_log_adapter_file_ce, self, ZEND_STRL(ASF_LOG_ADAPTER_FILE_PRONAME_FILE_PATH), 1, NULL);
    zval *fname = zend_read_property(asf_log_adapter_file_ce, self, ZEND_STRL(ASF_LOG_ADAPTER_FILE_PRONAME_FILE_NAME), 1, NULL);

    if (!fname) {
        return;
    }

    zval zmn_1, zret_1;
    ZVAL_STRING(&zmn_1, "getFormatter");

    call_user_function_ex(&(asf_log_adapter_file_ce)->function_table, self, &zmn_1, &zret_1, 0, 0, 1, NULL);
    zval_ptr_dtor(&zmn_1);

    if (IS_OBJECT != Z_TYPE(zret_1)) {
        RETURN_FALSE;
    }

    zval args[3], zret_2, zmn_2;

    ZVAL_STRING(&zmn_2, "format");
    ZVAL_COPY_VALUE(&args[0], level);
    ZVAL_LONG(&args[1], time);
    ZVAL_COPY_VALUE(&args[2], message);

    call_user_function_ex(&Z_OBJCE(zret_1)->function_table, &zret_1, &zmn_2, &zret_2, 3, args, 1, NULL);

    zval_ptr_dtor(&zret_1);
    zval_ptr_dtor(&zmn_2);

    if (IS_STRING != Z_TYPE(zret_2)) {
        RETURN_FALSE;
    }

    if (ASF_G(use_lcache)) {
        ret = asf_func_buffer_cache(&zret_2, Z_STRVAL_P(path), Z_STRVAL_P(fname));
    } else {
        php_stream *stream = NULL;
        zval *zstream = zend_read_property(asf_log_adapter_file_ce, self, ZEND_STRL(ASF_LOG_ADAPTER_FILE_PRONAME_STREAM), 1, NULL);
        if (IS_RESOURCE == Z_TYPE_P(zstream)) {
            php_stream_from_zval(stream, zstream);
            ret = php_stream_write(stream, Z_STRVAL(zret_2), Z_STRLEN(zret_2));
        }
    }

    zval_ptr_dtor(&zret_2);
    RETURN_BOOL(ret);
}
/* }}} */

/* {{{ proto bool Asf_Log_Adapter_File::close(void)
*/
PHP_METHOD(asf_log_adapter_file, close)
{
    php_stream *stream = NULL;
    int ret = 1;
    zval *self = getThis();

    zval *zstream = zend_read_property(asf_log_adapter_file_ce, self, ZEND_STRL(ASF_LOG_ADAPTER_FILE_PRONAME_STREAM), 1, NULL);

    if (IS_RESOURCE == Z_TYPE_P(zstream)) {
        php_stream_from_zval(stream, zstream);
        ret = php_stream_close(stream);
        zend_update_property_null(asf_log_adapter_file_ce, self, ZEND_STRL(ASF_LOG_ADAPTER_FILE_PRONAME_STREAM));
    }

    RETURN_BOOL(!ret);
}
/* }}} */

/* {{{ proto bool Asf_Log_Adapter_File::flushBuffer(void)
    In most cases on CLI mode */
PHP_METHOD(asf_log_adapter_file, flushBuffer)
{
    if (!ASF_G(use_lcache) || IS_ARRAY != Z_TYPE(ASF_G(log_buffer))) {
        RETURN_FALSE;
    }

    (void)asf_func_shutdown_buffer(0);

    RETURN_TRUE;
}
/* }}} */

/* {{{ asf_log_adapter_file_methods[]
*/
zend_function_entry asf_log_adapter_file_methods[] = {
    PHP_ME(asf_log_adapter_file, __construct,  asf_log_adapter_file_construct_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(asf_log_adapter_file, close,		   NULL, ZEND_ACC_PUBLIC)
    PHP_ME(asf_log_adapter_file, flushBuffer,  NULL, ZEND_ACC_PUBLIC)
    PHP_ME(asf_log_adapter_file, doLog,		   asf_log_adapter_file_dolog_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(asf_log_adapter_file, getFormatter, NULL, ZEND_ACC_PUBLIC)
    PHP_FE_END
};
/* }}} */

ASF_INIT_CLASS(log_adapter_file) /* {{{ */
{
    ASF_REGISTER_CLASS_INTERNAL(asf_log_adapter_file, Asf_Log_Adapter_File, Asf\\Log\\Adapter\\File, asf_log_adapter_ce, asf_log_adapter_file_methods);

    zend_declare_property_null(asf_log_adapter_file_ce, ZEND_STRL(ASF_LOG_ADAPTER_FILE_PRONAME_FILE_PATH), ZEND_ACC_PROTECTED);
    zend_declare_property_null(asf_log_adapter_file_ce, ZEND_STRL(ASF_LOG_ADAPTER_FILE_PRONAME_FILE_NAME), ZEND_ACC_PROTECTED);
    zend_declare_property_null(asf_log_adapter_file_ce, ZEND_STRL(ASF_LOG_ADAPTER_FILE_PRONAME_STREAM), ZEND_ACC_PROTECTED);

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
