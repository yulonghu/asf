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
#include "main/SAPI.h" /* SG, PG */
#include "php_asf.h"
#include "Zend/zend_interfaces.h" /* for zend_call_method_with_* */
#include "Zend/zend_string.h" /* ZSTR_CHAR */
#include "ext/standard/php_string.h"
#include "ext/standard/php_rand.h"
#include "ext/standard/html.h" /* htmlspecialchars */

#include "kernel/asf_namespace.h"
#include "http/asf_http_request.h"
#include "kernel/asf_func.h"
#include "asf_util.h"
#include <math.h>

zend_class_entry *asf_util_ce;

/* {{{ ARG_INFO
*/
ZEND_BEGIN_ARG_INFO_EX(asf_util_bytes2string_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, size)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(asf_util_string2bytes_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, data)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(asf_util_trimarray_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, data)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(asf_util_filterxss_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, data)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(asf_util_sets_arginfo, 0, 0, 1)
    ZEND_ARG_ARRAY_INFO(0, ini, 1)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(asf_util_ua_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, ua)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(asf_util_pub_str_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, str)
ZEND_END_ARG_INFO()
/* }}}*/

/* {{{ get_default_charset
*/
static inline char *get_default_charset(void)
{
    if (PG(internal_encoding) && PG(internal_encoding)[0]) {
        return PG(internal_encoding);
    } else if (SG(default_charset) && SG(default_charset)[0] ) {
        return SG(default_charset);
    }

    return SG(default_charset);
}
/* }}} */

static inline void asf_util_atrim_callback(zval **cb) /* {{{ */
{
    zend_string *key = NULL, *st = NULL;
    zval *entry = NULL;

    if (Z_TYPE_P(*cb) == IS_ARRAY) {
        ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(*cb), key, entry) {
            ZVAL_DEREF(entry);
            (void)asf_util_atrim_callback(& entry);
        } ZEND_HASH_FOREACH_END();
    } else {
        if (Z_TYPE_P(*cb) == IS_STRING) {
            st = php_trim(Z_STR_P(*cb), NULL, 0, 3);
            if (ZSTR_LEN(st) != Z_STRLEN_P(*cb)) {
                memset(Z_STRVAL_P(*cb), '\0', Z_STRLEN_P(*cb));
                Z_STRLEN_P(*cb) = ZSTR_LEN(st);
                memcpy(Z_STRVAL_P(*cb), ZSTR_VAL(st), ZSTR_LEN(st));
                Z_STRVAL_P(*cb)[ZSTR_LEN(st)] = '\0';
                zend_string_forget_hash_val(Z_STR_P(*cb));
            }
            zend_string_release(st);
            st = NULL;
        }
    }
}
/* }}} */

static inline void asf_util_filterxss_callback(zval **cb, char *default_charset) /* {{{ */
{
    zend_string *key = NULL, *st = NULL, *rst = NULL;
    zval *entry = NULL;

    if (Z_TYPE_P(*cb) == IS_ARRAY) {
        ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(*cb), key, entry) { 
            ZVAL_DEREF(entry);
            (void)asf_util_filterxss_callback(& entry, default_charset);
        } ZEND_HASH_FOREACH_END();
    } else {
        if (Z_TYPE_P(*cb) == IS_STRING) {
            st = php_escape_html_entities_ex((unsigned char*)Z_STRVAL_P(*cb), Z_STRLEN_P(*cb)
                    , 0, (int) ENT_QUOTES, default_charset, 1);
            zval_dtor(*cb);
            rst = zend_string_init(ZSTR_VAL(st), ZSTR_LEN(st), 0);
            ZVAL_STR(*cb, rst);
            zend_string_release(st);
        }
    }
}
/* }}} */

static _Bool asf_util_request_url(zend_string *http_url, _Bool http_ispost, zval *http_data, zval *http_opts, zval *return_value) /* {{{ */
{
    /* Check for CURL extension */
    if (!zend_hash_str_exists(EG(function_table), "curl_init", 9)) {
        return 0;
    }
    
    if (UNEXPECTED(!http_url || asf_func_isempty(ZSTR_VAL(http_url)))) {
        return 0;
    }

    zval url;
    zval curl_init, opts, opts_retval, err_retval;

    ZVAL_STR(&url, http_url);
    zend_call_method_with_1_params(NULL, NULL, NULL, "curl_init", &curl_init, &url);
    if (Z_TYPE(curl_init) == IS_FALSE) {
        return 0;
    }

    array_init(&opts);
    add_index_long(&opts, 13, 1); /* CURLOPT_TIMEOUT */
    add_index_long(&opts, 78, 1); /* CURLOPT_CONNECTTIMEOUT */
    add_index_long(&opts, 19913, 1); /* CURLOPT_RETURNTRANSFER */
    add_index_long(&opts, 64, 0); /* CURLOPT_SSL_VERIFYPEER */
    add_index_long(&opts, 81, 0); /* CURLOPT_SSL_VERIFYHOST */
    add_index_long(&opts, 52, 1); /* CURLOPT_FOLLOWLOCATION */

    if (http_ispost && http_data && Z_TYPE_P(http_data) == IS_STRING) {
        add_index_zval(&opts, 10015, http_data); /* CURLOPT_POSTFIELDS */
    } else {
        add_index_long(&opts, 80, 1); /* CURLOPT_HTTPGET */
    }

    /* From user custom http_opts */
    if (http_opts && Z_TYPE_P(http_opts) == IS_ARRAY && zend_hash_num_elements(Z_ARRVAL_P(http_opts))) {
        zend_ulong id = 0; zval *entry = NULL;
        ZEND_HASH_FOREACH_NUM_KEY_VAL(Z_ARRVAL_P(http_opts), id, entry) {
            if (!id || !entry) {
                continue;
            }

            ZVAL_DEREF(entry);
            if (Z_TYPE_P(entry) == IS_LONG || Z_TYPE_P(entry) == IS_TRUE || Z_TYPE_P(entry) == IS_FALSE) {
                add_index_long(&opts, id, zend_is_true(entry));
            } else {
                Z_TRY_ADDREF_P(entry);
                add_index_zval(&opts, id, entry);
            }
        } ZEND_HASH_FOREACH_END();
    }

    zend_call_method_with_2_params(NULL, NULL, NULL, "curl_setopt_array", &opts_retval, &curl_init, &opts);
    if (Z_TYPE(opts_retval) == IS_FALSE) {
        zend_call_method_with_1_params(NULL, NULL, NULL, "curl_close", NULL, &curl_init);
        return 0;
    }

    /* execute */
    zend_call_method_with_1_params(NULL, NULL, NULL, "curl_exec", return_value, &curl_init);
    if (Z_TYPE_P(return_value) == IS_FALSE) {
        /* errno, error */
        zval error;
        array_init(return_value);
        zend_call_method_with_1_params(NULL, NULL, NULL, "curl_errno", &error, &curl_init);
        add_assoc_long_ex(return_value, "errno", 5, Z_LVAL(error));
        zend_call_method_with_1_params(NULL, NULL, NULL, "curl_error", &error, &curl_init);
        add_assoc_zval_ex(return_value, "error", 5, &error);
        zval_ptr_dtor(&error);
    }

    zval_ptr_dtor(&opts);
    zend_call_method_with_1_params(NULL, NULL, NULL, "curl_close", NULL, &curl_init);
    zval_ptr_dtor(&curl_init);

    return 1;
}
/* }}} */

/* {{{ proto string Asf_Util::ip(void)
*/
PHP_METHOD(asf_util, ip)
{
    char *ip = NULL;
    const char *nil = "unknown";
    zval *retval = NULL, *addr = NULL, zfilter, zip;
    int filter = 0x0113;

#define FOUNDIT(ip, txt) { \
    ip = getenv(txt); \
    if (ip && strcasecmp(ip, nil)) { \
        break; \
    } \
}
    do {
        FOUNDIT(ip, "HTTP_CLIENT_IP");
        FOUNDIT(ip, "HTTP_X_FORWARDED_FOR");
        FOUNDIT(ip, "REMOTE_ADDR");

        retval = asf_http_req_pg_find(TRACK_VARS_SERVER);
        if (retval && (addr = zend_hash_str_find(Z_ARRVAL_P(retval), "REMOTE_ADDR", 11))) {
            ip = Z_STRVAL_P(addr);
            break;
        }
    } while (0);

    if (ip) {
        ZVAL_LONG(&zfilter, filter);
        ZVAL_STRING(&zip, ip);
        zend_call_method_with_2_params(NULL, NULL, NULL, "filter_var", return_value, &zip, &zfilter);
        zval_ptr_dtor(&zip);
    } else {
        RETURN_FALSE;
    }
}
/* }}} */

/* {{{ proto string Asf_Util::bytes2string(int/float $size, bool $space = 1)
*/
PHP_METHOD(asf_util, bytes2string)
{
    zval *size = NULL;
    char *ret = NULL, *u = NULL;
    int ret_len = 0;
    _Bool space = 1;
    double ds = 0.0, dt = 0.0;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "z|b", &size, &space) == FAILURE) {
        return;
    }

    convert_to_double(size);
    ds = Z_DVAL_P(size);

    do {
        if (ds - 1073741824.0 >= 0) {
            dt = (ds / 1073741824.0); u = "GB"; break;
        }
        if (ds - 1048576.0 >= 0) {
            dt = (ds / 1048576.0); u = "GB"; break;
        }
        if (ds - 1024.0 >= 0) {
            dt = (ds / 1024.0); u = "KB"; break;
        }
        dt = ds; u = "Bytes"; break;
    } while(0);

    ret_len = spprintf(&ret, 0, "%G%s%s", dt, (space ? " " : ""), u);
    ZVAL_STRINGL(return_value, ret, ret_len);

    efree(ret);
}
/* }}} */

/* {{{ proto string Asf_Util::guid(void)
*/
PHP_METHOD(asf_util, guid)
{
    zval ret1, ret2, ret3;
    zval arg1;
    char *vs = NULL;

    /* Check openssl_random_pseudo_bytes function */
    if (!zend_hash_str_exists(EG(function_table), "openssl_random_pseudo_bytes", 27)) {
        RETURN_FALSE;
    }

    ZVAL_LONG(&arg1, 16);
    zend_call_method_with_1_params(NULL, NULL, NULL, "openssl_random_pseudo_bytes", &ret1, &arg1);

    if (Z_TYPE(ret1) == IS_FALSE) {
        RETURN_FALSE;
    }

#define ORD(str) (unsigned char)str

    Z_STRVAL_P(&ret1)[6] = (ORD(Z_STRVAL_P(&ret1)[6]) & 0x0f) | 0x40;
    Z_STRVAL_P(&ret1)[8] = (ORD(Z_STRVAL_P(&ret1)[8]) & 0x0f) | 0x40;

    zend_call_method_with_1_params(NULL, NULL, NULL, "bin2hex", &ret2, &ret1);

    ZVAL_LONG(&arg1, 4);
    zend_call_method_with_2_params(NULL, NULL, NULL, "str_split", &ret3, &ret2, &arg1);

    zval_ptr_dtor(&ret1);
    zval_ptr_dtor(&ret2);

    if (Z_TYPE(ret3) == IS_FALSE) {
        RETURN_FALSE;
    }

    ZVAL_STRINGL(&arg1, "%s%s-%s-%s-%s-%s%s%s", 20);
    zend_call_method_with_2_params(NULL, NULL, NULL, "vsprintf", return_value, &arg1, &ret3);

    zval_ptr_dtor(&arg1);
    zval_ptr_dtor(&ret3);
}
/* }}} */

/* {{{ proto mixed Asf_Util::atrim(mixed $data)
*/
PHP_METHOD(asf_util, atrim)
{
    zval *cb = NULL, self, method;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &cb) == FAILURE) {
        return;
    }

    (void)asf_util_atrim_callback(&cb);

    RETURN_ZVAL(cb, 1, 0);
}
/* }}} */

/* {{{ proto mixed Asf_Util::filterXss(mixed $data)
    filter 0x01,0x02,0x03,0x04,0x05 */
PHP_METHOD(asf_util, filterXss)
{
    zval *xss = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &xss) == FAILURE) {
        return;
    }

    char *default_charset = default_charset = get_default_charset();
    (void)asf_util_filterxss_callback(&xss, default_charset);

    RETURN_ZVAL(xss, 1, 0);
}
/* }}} */

/* {{{ proto array Asf_Util::cpass(string $data [, string $salt = ''])
*/
PHP_METHOD(asf_util, cpass)
{
    zval *data = NULL;
    zval arg, ret1, ret2;
    zend_string *s_salt = NULL;
    char *salt = NULL, *n_salt = NULL;
    int n_salt_len = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "z|S", &data, &s_salt) == FAILURE) {
        return;
    }

    if (s_salt && ZSTR_LEN(s_salt)) {
        salt = estrndup(ZSTR_VAL(s_salt), 6);
    } else {
        zend_long rand = php_rand();

        ZVAL_LONG(&arg, rand);
        zend_call_method_with_1_params(NULL, NULL, NULL, "md5", &ret1, &arg);

        salt = estrndup(Z_STRVAL(ret1), 6);
        zval_ptr_dtor(&ret1);
    }

    zend_call_method_with_1_params(NULL, NULL, NULL, "md5", &ret1, data);
    n_salt_len = spprintf(&n_salt, 0, "%s%s", Z_STRVAL(ret1), salt);
    zval_ptr_dtor(&ret1);

    ZVAL_STRINGL(&arg, n_salt, n_salt_len);
    zend_call_method_with_1_params(NULL, NULL, NULL, "md5", &ret1, &arg);

    array_init(return_value);
    zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &ret1);
    add_next_index_stringl(return_value, salt, 6);

    efree(n_salt);
    efree(salt);
    zval_ptr_dtor(&arg);
}
/* }}} */

/* {{{ proto bool Asf_Util::iniSets(array $ini)
*/
PHP_METHOD(asf_util, iniSets)
{
    zval *sets = NULL, *entry = NULL, arg;
    zend_string *key = NULL;
    _Bool bret = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "a", &sets) == FAILURE) {
        return;
    }

    if (UNEXPECTED(zend_hash_num_elements(Z_ARRVAL_P(sets)) < 1)) {
        RETURN_FALSE;
    }

    ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(sets), key, entry) {
        if (!key) {
            continue;
        }

        bret = 1;
        ZVAL_STR_COPY(&arg, key);
        ZVAL_DEREF(entry);

        zend_call_method_with_2_params(NULL, NULL, NULL, "ini_set", NULL, &arg, entry);
        zend_string_release(key);
    } ZEND_HASH_FOREACH_END();

    RETURN_BOOL(bret);
}
/* }}} */

/* {{{ proto bool Asf_Util::isEmail(string $email)
*/
PHP_METHOD(asf_util, isEmail)
{
    zval *email = NULL;
    zval arg, retval;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &email) == FAILURE) {
        return;
    }

    if (UNEXPECTED(Z_TYPE_P(email) != IS_STRING
                || Z_STRLEN_P(email) <= 6 || Z_STRLEN_P(email) > 32)) {
        RETURN_FALSE;
    }

    ZVAL_STRINGL(&arg, "/^([A-Za-z0-9-_.+]+)@([A-Za-z0-9-]+[.][A-Za-z0-9-.]+)$/", 55);
    zend_call_method_with_2_params(NULL, NULL, NULL, "preg_match", &retval, &arg, email);
    zval_ptr_dtor(&arg);

    ZVAL_BOOL(return_value, Z_LVAL(retval) ? 1 : 0);
}
/* }}} */

/* {{{ proto string Asf_Util::fileExt(string $filename)
*/
PHP_METHOD(asf_util, fileExt)
{
    zval *filename = NULL;
    zval arg;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &filename) == FAILURE) {
        return;
    }

    if (UNEXPECTED(Z_TYPE_P(filename) != IS_STRING
                || Z_STRLEN_P(filename) < 2 || Z_STRLEN_P(filename) > 254)) {
        RETURN_FALSE;
    }

    char *p = NULL; int p_len = 0;
    if ((p = strrchr(Z_STRVAL_P(filename), '.'))) {
        p_len = strlen(p) - 1;
        p = php_strtolower(p + 1, p_len);
    } else {
        RETURN_FALSE;
    }

    RETURN_STRINGL(p, p_len);
}
/* }}} */

/* {{{ proto string Asf_Util::getUrl(string $url [, array $opts = array()])
*/
PHP_METHOD(asf_util, getUrl)
{
    zend_string *url = NULL;
    zval *opts = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|a", &url, &opts) == FAILURE) {
        return;
    }

    (void)asf_util_request_url(url, 0, NULL, opts, return_value);
}
/* }}} */

/* {{{ proto string Asf_Util::postUrl(string $url [, mixed $data [, array $opts = array()]])
*/
PHP_METHOD(asf_util, postUrl)
{
    zend_string *url = NULL;
    zval *data = NULL, *opts = NULL, retval;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|zz", &url, &data, &opts) == FAILURE) {
        return;
    }

    ZVAL_UNDEF(&retval);
    if (data && Z_TYPE_P(data) == IS_ARRAY && zend_hash_num_elements(Z_ARRVAL_P(data))) {
        zend_call_method_with_1_params(NULL, NULL, NULL, "http_build_query", &retval, data);
    }

    (void)asf_util_request_url(url, 1, &retval, opts, return_value);

    if (Z_TYPE(retval) == IS_STRING) {
        zval_ptr_dtor(&retval);
    }
}
/* }}} */

/* {{{ asf_util_methods[]
*/
const zend_function_entry asf_util_methods[] = {
    PHP_ME(asf_util, ip,                NULL,                           ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(asf_util, bytes2string,      asf_util_bytes2string_arginfo,  ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(asf_util, guid,              NULL,                           ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(asf_util, atrim,             asf_util_trimarray_arginfo,     ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(asf_util, filterXss,         asf_util_filterxss_arginfo,     ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(asf_util, iniSets,           asf_util_sets_arginfo,          ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(asf_util, isEmail,           asf_util_pub_str_arginfo,       ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(asf_util, fileExt,           asf_util_pub_str_arginfo,       ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(asf_util, cpass,             asf_util_pub_str_arginfo,       ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(asf_util, getUrl,            asf_util_pub_str_arginfo,       ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(asf_util, postUrl,           asf_util_pub_str_arginfo,       ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_FE_END
};
/* }}} */

ASF_INIT_CLASS(util) /* {{{ */
{
    ASF_REGISTER_CLASS_PARENT(asf_util, Asf_Util, Asf\\Util, ZEND_ACC_FINAL);

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
