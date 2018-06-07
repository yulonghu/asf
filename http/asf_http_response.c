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
#include "main/SAPI.h" /* for sapi_header_line */
#include "ext/json/php_json.h" /* php_json_encode */
#include "ext/standard/php_var.h" /* php_serialize_data_t */
#include "zend_smart_str.h" /* for smart_str */
#include "Zend/zend_interfaces.h" /* zend_call_method* */

#include "php_asf.h"
#include "asf_namespace.h"
#include "asf_exception.h"
#include "asf_sg.h"

#include "http/asf_http_responseinterface.h"
#include "http/asf_http_response.h"

zend_class_entry *asf_http_response_ce;

void asf_http_rep_instance(asf_http_rep_t *this_ptr) /* {{{ */
{
    if (Z_ISUNDEF_P(this_ptr)) {
        object_init_ex(this_ptr, asf_http_response_ce);
    }
}
/* }}} */

static void asf_http_response_ctype_json_ex(size_t code, zval *body, _Bool flag) /* {{{ */
{
    smart_str buf = {0};
    zval regs;
    uint ret = SUCCESS;

    /* Exclude cli mode, 'request_method' value is NULL in cli mode */
    if (EXPECTED(SG(request_info).request_method && !ASF_G(debug_dump))) {
        sapi_add_header("Content-Type: application/json", 30, 1);
    }

    switch (Z_TYPE_P(body)) {
        case IS_NULL:
default_body:
            ASF_HTTP_REP_JSON_FORMAT_TXT("%d", code, "%s", "", flag);
            ret = SUCCESS;
            break;

        case IS_FALSE:
            ASF_HTTP_REP_JSON_FORMAT_TXT("%d", code, "%d", 0, flag);
            break;

        case IS_TRUE:
            ASF_HTTP_REP_JSON_FORMAT_TXT("%d", code, "%d", 1, flag);
            break;

        case IS_LONG:
            ASF_HTTP_REP_JSON_FORMAT_TXT("%d", code, ZEND_LONG_FMT, Z_LVAL_P(body), flag);
            break;

        case IS_DOUBLE:
            ASF_HTTP_REP_JSON_FORMAT_TXT("%d", code, "%g", Z_DVAL_P(body), flag);
            break;

        case IS_OBJECT:
            ret = FAILURE;
            break;

        case IS_ARRAY:
        case IS_STRING:
        case IS_REFERENCE:
            /* In PHP 7.0, someone times regs refcount = 1, why? So here initialization to null. */
            ZVAL_UNDEF(&regs);
            array_init(&regs);

            HashTable *ht = Z_ARRVAL(regs);
            Z_TRY_ADDREF_P(body);

            add_assoc_long_ex(&regs, "errno", 5, code);
            
            /* Save traffic */
            if (EXPECTED(flag)) {
                zend_hash_str_update(ht, "data", 4, body);
            } else {
                if (code == 0) {
                    add_assoc_string_ex(&regs, "errmsg", 6, "");
                    zend_hash_str_update(ht, "data", 4, body);
                } else {
                    zend_hash_str_update(ht, "errmsg", 6, body);
                    add_assoc_string_ex(&regs, "data", 4, "");
                }
            }

#if ZEND_MODULE_API_NO >= 20160303
            ret = php_json_encode(&buf, &regs, PHP_JSON_UNESCAPED_UNICODE);
#else
            php_json_encode(&buf, &regs, PHP_JSON_UNESCAPED_UNICODE);
#endif
            smart_str_0(&buf);

            if (EXPECTED(ret == SUCCESS)) {
                PHPWRITE(ZSTR_VAL(buf.s), ZSTR_LEN(buf.s));
            } else {
                ret = FAILURE;
            }

            smart_str_free(&buf);
            zval_ptr_dtor(&regs);
            break;

        default:
            ret = FAILURE;
            break;
    }

    if (UNEXPECTED(ret == FAILURE)) {
        goto default_body;
    }
}
/* }}} */

static void asf_http_response_ctype_serialize_ex(size_t code, zval *body) /* {{{ */
{
    smart_str buf = {0};

    php_serialize_data_t var_hash;
    php_serialize_data_t var_code;

    PHP_VAR_SERIALIZE_INIT(var_hash);
    smart_str_appendl(&buf, "i:", 2);
    smart_str_append_long(&buf, code);
    smart_str_appendc(&buf, ';');
    php_var_serialize(&buf, body, &var_hash);

    PHP_VAR_SERIALIZE_DESTROY(var_hash);

    smart_str_0(&buf);

    PHPWRITE(ZSTR_VAL(buf.s), ZSTR_LEN(buf.s));
    smart_str_free(&buf);
}
/* }}} */

static void asf_http_response_display(asf_http_rep_t *response, size_t code, zval *errmsg) /* {{{ */
{
    zval *body = NULL;
    if (code == 0 && response != NULL) {
        body = zend_read_property(Z_OBJCE_P(response), response, ZEND_STRL(ASF_HTTP_REP_PRONAME_VALUE), 1, NULL);
    } else {
        body = errmsg;
    }

    zend_ulong ctype_id = ASF_G(ctype_id);

    switch (ctype_id) {
        case ASF_HTTP_REP_CTYPE_NONE:
            break;

        case ASF_HTTP_REP_CTYPE_JSON_DATA:
            (void)asf_http_response_ctype_json_ex(code, body, 1);
            break;

        case ASF_HTTP_REP_CTYPE_JSON_ERRMSG:
            (void)asf_http_response_ctype_json_ex(code, body, 0);
            break;

        case ASF_HTTP_REP_CTYPE_SERIALIZE:
            (void)asf_http_response_ctype_serialize_ex(code, body);
            break;

        case ASF_HTTP_REP_CTYPE_VAR_DUMP:
            php_var_dump(body, 1);
            break;
    }

    if (code != 0) {
        zend_bailout();
    }
}
/* }}} */

void asf_http_rep_display_success(asf_http_rep_t *response) /* {{{ */
{
    (void)asf_http_response_display(response, 0, NULL);
}
/* }}} */

void asf_http_rep_display_error(size_t code, zval *errmsg) /* {{{ */
{
    (void)asf_http_response_display(NULL, code, errmsg);
}
/* }}} */

/* {{{ proto object Asf_Http_Response::__construct(void)
*/
PHP_METHOD(asf_http_response, __construct)
{
    (void)asf_http_rep_instance(getThis());
}
/* }}} */

PHP_METHOD(asf_http_response, __clone) /* {{{ */
{
}
/* }}} */

/* {{{ proto bool Asf_Http_Response::setContentType(string $ctype, string $charset)
*/
PHP_METHOD(asf_http_response, setContentType)
{
    char *ctype = NULL, *charset = NULL;
    size_t ctype_len = 0, charset_len = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "ss", &ctype, &ctype_len, &charset, &charset_len) == FAILURE) {
        return;
    }

    char *content_type = NULL;
    size_t content_type_len = 0;

    content_type_len = spprintf(&content_type, 0, "Content-Type:%s; charset=%s", ctype, charset);

    zend_update_property_stringl(asf_http_response_ce, getThis(), ZEND_STRL(ASF_HTTP_REP_PRONAME_HEADER), content_type, content_type_len);

    efree(content_type);

    RETURN_TRUE;
}
/* }}} */

/* {{{ proto void Asf_Http_Response::redirect(string $url [, int $status_code = 301])
*/
PHP_METHOD(asf_http_response, redirect)
{
    char *url = NULL;
    size_t url_len = 0, status_code = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|l", &url, &url_len, &status_code) == FAILURE) {
        return;
    }

    if (UNEXPECTED(!(SG(request_info).request_method))) {
        RETURN_FALSE;
    }

    sapi_header_line ctr = {0};
    ctr.line_len = 30;

    switch (status_code) {
        case 302:
            ctr.line = "HTTP/1.1 302 Moved Temporarily";
            break;	
        default:
            ctr.line_len = 30;
            ctr.line = "HTTP/1.1 301 Moved Permanently";
            break;
    }

    ctr.response_code = status_code;
    sapi_header_op(SAPI_HEADER_REPLACE, &ctr);

    ctr.response_code = 0;
    ctr.line_len  = spprintf(&(ctr.line), 0, "%s %s", "Location:", url);

    if(sapi_header_op(SAPI_HEADER_REPLACE, &ctr) == SUCCESS) {
        sapi_send_headers();
    }

    efree(ctr.line);
    zend_bailout();
}
/* }}} */

/* {{{ proto bool Asf_Http_Response::setContent(mixed $content)
*/
PHP_METHOD(asf_http_response, setContent)
{
    zval *zret = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &zret) == FAILURE) {
        return;
    }

    zend_update_property(asf_http_response_ce, getThis(), ZEND_STRL(ASF_HTTP_REP_PRONAME_VALUE), zret);

    RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool Asf_Http_Response::appendContent(mixed $content)
*/
PHP_METHOD(asf_http_response, appendContent)
{
    zend_string *pbody = NULL, *rbody = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &pbody) == FAILURE) {
        return;
    }

    zval *value = zend_read_property(asf_http_response_ce, getThis(), ZEND_STRL(ASF_HTTP_REP_PRONAME_VALUE), 1, NULL);

    if (value && Z_TYPE_P(value) == IS_STRING) {
        size_t len = ZSTR_LEN(pbody) + Z_STRLEN_P(value);
        zend_string *ret = zend_string_realloc(Z_STR_P(value), len, 0);
        memcpy(ZSTR_VAL(ret) + ZSTR_LEN(ret) - ZSTR_LEN(pbody), ZSTR_VAL(pbody), ZSTR_LEN(pbody));
        ZSTR_VAL(ret)[ZSTR_LEN(ret)] = '\0'; 
        ZVAL_NEW_STR(value, ret);
    } else {
        zend_update_property_str(asf_http_response_ce, getThis(), ZEND_STRL(ASF_HTTP_REP_PRONAME_VALUE), pbody);   
    }

    RETURN_TRUE;
}
/* }}} */

/* {{{ proto string Asf_Http_Response::getContent(void)
*/
PHP_METHOD(asf_http_response, getContent)
{
    ZVAL_COPY(return_value, zend_read_property(asf_http_response_ce,
                getThis(), ZEND_STRL(ASF_HTTP_REP_PRONAME_VALUE), 1, NULL));
}
/* }}} */

/* {{{ proto bool Asf_Http_Response::send(void)
*/
PHP_METHOD(asf_http_response, send)
{
    (void)asf_http_rep_display_success(getThis()); 

    RETURN_TRUE;
}
/* }}} */

/* {{{ asf_http_response_methods[]
*/
zend_function_entry asf_http_response_methods[] = {
    PHP_ME(asf_http_response, __construct,		NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(asf_http_response, __clone,			NULL, ZEND_ACC_PRIVATE)
    PHP_ME(asf_http_response, setContentType,	asf_http_repinterface_setcontenttype_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(asf_http_response, redirect,			asf_http_repinterface_redirect_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(asf_http_response, setContent,		asf_http_repinterface_setcontent_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(asf_http_response, appendContent,		asf_http_repinterface_appendcontent_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(asf_http_response, getContent,		NULL, ZEND_ACC_PUBLIC)
    PHP_ME(asf_http_response, send,				NULL, ZEND_ACC_PUBLIC)
    PHP_FE_END
};
/* }}} */

ASF_INIT_CLASS(http_response) /* {{{ */
{
    ASF_REGISTER_CLASS_PARENT(asf_http_response, Asf_Http_Response, Asf\\HTTP\\Response, ZEND_ACC_FINAL);

    zend_declare_property_null(asf_http_response_ce, ZEND_STRL(ASF_HTTP_REP_PRONAME_VALUE), ZEND_ACC_PROTECTED);
    zend_declare_property_null(asf_http_response_ce, ZEND_STRL(ASF_HTTP_REP_PRONAME_HEADER), ZEND_ACC_PROTECTED);

    ASF_INIT(responseinterface);

    zend_class_implements(asf_http_response_ce, 1, asf_http_responseinterface_ce);

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
