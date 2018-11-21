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
#include "kernel/asf_namespace.h"
#include "asf_exception.h"
#include "asf_log_adapter_file.h"
#include "log/asf_log_adapter.h"
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

    zend_string *s_path = file_path ? file_path : ASF_G(log_path);

    if (!s_path) {
        asf_trigger_error(ASF_ERR_NOTFOUND_LOGGER_PATH, "Parameter 'file_path/asf.log_path' must be a string");
        return;
    }

    zend_update_property_str(asf_log_adapter_file_ce, this_ptr
            , ZEND_STRL(ASF_LOG_ADAPTER_FILE_PRONAME_FILE_PATH), s_path);
    zend_update_property_str(asf_log_adapter_file_ce, this_ptr
            , ZEND_STRL(ASF_LOG_ADAPTER_FILE_PRONAME_FILE_NAME), file_name);

    /* load local file */
    if (!ASF_G(use_lcache)) {
        php_stream *stream = NULL;
        char *fpath = NULL;
        int fpath_len = 0;
        zval zstream;

        fpath_len = spprintf(&fpath, 0, "%s%c%s", ZSTR_VAL(s_path), DEFAULT_SLASH, ZSTR_VAL(file_name));
        stream = asf_func_fopen(fpath, fpath_len, s_path);

        if (stream) {
            php_stream_to_zval(stream, &zstream);
            zend_update_property(Z_OBJCE_P(this_ptr), this_ptr, ZEND_STRL(ASF_LOG_ADAPTER_FILE_PRONAME_STREAM), &zstream);
            ASF_FUNC_REGISTER_SHUTDOWN_FUNCTION_CLOSE(this_ptr, 1);
        }

        efree(fpath);
    }
}
/* }}} */

/* [full_path1 => data, full_path2 => data] */
static _Bool asf_func_buffer_cache(zend_string *message, const char *path, const char *fname) /* {{{ */
{
    zval *zbuf = NULL;
    zval new_array;
    char *fpath = NULL;
    size_t fpath_len = 0;

    fpath_len = spprintf(&fpath, 0, "%s%c%s", path, DEFAULT_SLASH, fname);

    do {
        if (IS_ARRAY != Z_TYPE(ASF_G(log_buffer))) {
            array_init(&ASF_G(log_buffer));
            break;
        }

        if ((zbuf = zend_hash_str_find_ptr(Z_ARRVAL(ASF_G(log_buffer)), fpath, fpath_len)) != NULL) {
            add_next_index_stringl((void *)&zbuf, ZSTR_VAL(message), ZSTR_LEN(message));
            efree(fpath);
            return 1;
        }
    } while(0);

    array_init(&new_array);
    
    add_next_index_stringl(&new_array, ZSTR_VAL(message), ZSTR_LEN(message));
    add_assoc_zval_ex(&ASF_G(log_buffer), fpath, fpath_len, &new_array);
    
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

/* {{{ proto bool Asf_Log_Adapter_File::doLog(string $message)
*/
PHP_METHOD(asf_log_adapter_file, doLog)
{
    zend_string *message = NULL;
    _Bool ret = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &message) == FAILURE) {
        return;
    }

    zval *self = getThis();

    if (ASF_G(use_lcache)) {
        zval *path = zend_read_property(asf_log_adapter_file_ce, self, ZEND_STRL(ASF_LOG_ADAPTER_FILE_PRONAME_FILE_PATH), 1, NULL);
        zval *fname = zend_read_property(asf_log_adapter_file_ce, self, ZEND_STRL(ASF_LOG_ADAPTER_FILE_PRONAME_FILE_NAME), 1, NULL);

        /* Fix file_name is empty issue */
        if (UNEXPECTED(Z_ISNULL_P(fname))) {
            RETURN_FALSE;
        }
        ret = asf_func_buffer_cache(message, Z_STRVAL_P(path), Z_STRVAL_P(fname));
    } else {
        php_stream *stream = NULL;
        zval *zstream = zend_read_property(asf_log_adapter_file_ce, self, ZEND_STRL(ASF_LOG_ADAPTER_FILE_PRONAME_STREAM), 1, NULL);
        if (IS_RESOURCE == Z_TYPE_P(zstream)) {
            php_stream_from_zval(stream, zstream);
            ret = php_stream_write(stream, ZSTR_VAL(message), ZSTR_LEN(message));
        }
    }

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
    RETURN_BOOL(asf_func_shutdown_buffer());
}
/* }}} */

/* {{{ asf_log_adapter_file_methods[]
*/
zend_function_entry asf_log_adapter_file_methods[] = {
    PHP_ME(asf_log_adapter_file, __construct,  asf_log_adapter_file_construct_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(asf_log_adapter_file, flushBuffer,  NULL, ZEND_ACC_PUBLIC)
    PHP_ME(asf_log_adapter_file, doLog,		   asf_log_adapter_file_dolog_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(asf_log_adapter_file, close,		   NULL, ZEND_ACC_PUBLIC)
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
