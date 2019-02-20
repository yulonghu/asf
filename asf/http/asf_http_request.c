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
#include "ext/standard/url.h" /* for php_url */
#include "standard/php_string.h" /* for php_basename */

#include "php_asf.h"
#include "kernel/asf_namespace.h"
#include "asf_exception.h"
#include "http/asf_http_requestinterface.h"
#include "http/asf_http_request.h"

zend_class_entry *asf_http_request_ce;

/* Excerpt from the yaf, http://yaf.laruence.com/manual/ */
asf_http_req_t *asf_http_req_instance(asf_http_req_t *this_ptr, zend_string *base_uri, zend_string *request_uri) /* {{{ */
{
    zval params;
    zend_string *settled_uri = NULL;
    char *method = NULL; uint method_len = 0;
    zval *uri = NULL;

    if (Z_ISUNDEF_P(this_ptr)) {
        object_init_ex(this_ptr, asf_http_request_ce);
    }

    zend_update_property_str(asf_http_request_ce, this_ptr, ZEND_STRL(ASF_HTTP_REQ_PRONAME_MODULE), ASF_G(default_module));
    zend_update_property_str(asf_http_request_ce, this_ptr, ZEND_STRL(ASF_HTTP_REQ_PRONAME_SERVICE), ASF_G(default_service));
    zend_update_property_str(asf_http_request_ce, this_ptr, ZEND_STRL(ASF_HTTP_REQ_PRONAME_ACTION), ASF_G(default_action));

    if (SG(request_info).request_method) {
        method_len = strlen((char *)SG(request_info).request_method);
        method = estrndup((char *)SG(request_info).request_method, method_len);
    } else if (ASF_G(cli)) {
        method_len = strlen(sapi_module.name);
        method = estrndup(sapi_module.name, method_len);
    } else {
        method_len = 6;
        method = estrndup("unkonw", method_len);
    }

    zend_update_property_stringl(asf_http_request_ce, this_ptr, ZEND_STRL(ASF_HTTP_REQ_PRONAME_METHOD), method, method_len);
    efree(method);

    do {
        if (request_uri) {
            settled_uri = zend_string_copy(request_uri); 
            break;
        }

#ifdef PHP_WIN32
        zval *rewrited = NULL;
        /* check this first so IIS will catch */
        uri = asf_http_req_pg_find_len(TRACK_VARS_SERVER, "HTTP_X_REWRITE_URL", sizeof("HTTP_X_REWRITE_URL") - 1);
        if (uri) {
            if (EXPECTED(Z_TYPE_P(uri) == IS_STRING))  {
                settled_uri = zend_string_copy(Z_STR_P(uri));
                break;
            }
        }

        /* IIS7 with URL Rewrite: make sure we get the unencoded url (double slash problem) */
        rewrited = asf_http_req_pg_find_len(TRACK_VARS_SERVER, "IIS_WasUrlRewritten", sizeof("IIS_WasUrlRewritten") - 1);
        if (rewrited) {
            if (zend_is_true(rewrited)) {
                uri = asf_http_req_pg_find_len(TRACK_VARS_SERVER, "UNENCODED_URL", sizeof("UNENCODED_URL") - 1);
                if (uri) {
                    if (EXPECTED(Z_TYPE_P(uri) == IS_STRING && Z_STRLEN_P(uri))) {
                        settled_uri = zend_string_copy(Z_STR_P(uri));
                        break;
                    }
                }
            }
        }
#endif
        uri = asf_http_req_pg_find_len(TRACK_VARS_SERVER, "REQUEST_URI", sizeof("REQUEST_URI") - 1);
        if (uri && EXPECTED(Z_TYPE_P(uri) == IS_STRING)) {
            /* Http proxy reqs setup request uri with scheme and host [and port] + the url path,
             * only use url path */
            if (strncasecmp(Z_STRVAL_P(uri), "http", sizeof("http") - 1) == 0) {
                php_url *url_info = php_url_parse_ex(Z_STRVAL_P(uri), Z_STRLEN_P(uri));
                if (url_info && url_info->path) {
                    settled_uri = zend_string_init(url_info->path, strlen(url_info->path), 0);
                }
                php_url_free(url_info);
            } else {
                char *pos = NULL;
                if ((pos = strstr(Z_STRVAL_P(uri), "?"))) {
                    settled_uri = zend_string_init(Z_STRVAL_P(uri), pos - Z_STRVAL_P(uri), 0);
                } else {
                    settled_uri = zend_string_copy(Z_STR_P(uri));
                }
            }
            break;
        }

        uri = asf_http_req_pg_find_len(TRACK_VARS_SERVER, "PATH_INFO", sizeof("PATH_INFO") - 1);
        if (uri && EXPECTED(Z_TYPE_P(uri) == IS_STRING)) {
            settled_uri = zend_string_copy(Z_STR_P(uri));
            break;
        }

        uri = asf_http_req_pg_find_len(TRACK_VARS_SERVER, "ORIG_PATH_INFO", sizeof("ORIG_PATH_INFO") - 1);
        if (uri && EXPECTED(Z_TYPE_P(uri) == IS_STRING)) {
            settled_uri = zend_string_copy(Z_STR_P(uri));
            break;
        }
    } while (0);

    if (settled_uri) {
        register char *p = ZSTR_VAL(settled_uri);

        while (*p == '/' && *(p + 1) == '/') {
            p++;
        }

        if (p != ZSTR_VAL(settled_uri)) {
            zend_string *garbage = settled_uri;
            settled_uri = zend_string_init(p, ZSTR_LEN(settled_uri) - (p - ZSTR_VAL(settled_uri)), 0);
            zend_string_release(garbage);
        }

        zend_update_property_str(asf_http_request_ce, this_ptr, ZEND_STRL(ASF_HTTP_REQ_PRONAME_URI), settled_uri);
        if (base_uri) {
            zend_update_property_str(asf_http_request_ce, this_ptr, ZEND_STRL(ASF_HTTP_REQ_PRONAME_BASE), base_uri);
        }

        zend_string_release(settled_uri);
    }

    array_init(&params);
    zend_update_property(asf_http_request_ce, this_ptr, ZEND_STRL(ASF_HTTP_REQ_PRONAME_PARAMS), &params);
    zval_ptr_dtor(&params);

    return this_ptr;
}
/* }}} */

zval *asf_http_req_pg_find_ex(uint vars, char *name, size_t len) /* {{{ */
{
    zval *array_ptr = NULL, *retval = NULL;

    switch (vars) {
        case TRACK_VARS_GET:
        case TRACK_VARS_POST:
        case TRACK_VARS_COOKIE:
        case TRACK_VARS_FILES:
            array_ptr = &PG(http_globals)[vars];
            break;
        case TRACK_VARS_ENV:
            if (PG(auto_globals_jit)) {
                zend_is_auto_global_str(ZEND_STRL("_ENV"));
            }
            array_ptr = &PG(http_globals)[vars];
            break;
        case TRACK_VARS_SERVER:
            if (PG(auto_globals_jit)) {
                zend_is_auto_global_str(ZEND_STRL("_SERVER"));
            }
            array_ptr = &PG(http_globals)[vars];
            break;
        case TRACK_VARS_REQUEST:
            if (PG(auto_globals_jit)) {
                zend_is_auto_global_str(ZEND_STRL("_REQUEST"));
            }
            array_ptr = zend_hash_str_find(&EG(symbol_table), ZEND_STRL("_REQUEST"));
            break;
        default:
            break;
    }

    if (!array_ptr) {
        return NULL;
    }

    if (!name) {
        return array_ptr;
    }

    retval = zend_hash_str_find(Z_ARRVAL_P(array_ptr), name, len);

    return retval;
}
/* }}} */

int asf_http_req_set_params_multi(zval *request, zval *values) /* {{{ */
{
    zval *params = zend_read_property(Z_OBJCE_P(request), request, ZEND_STRL(ASF_HTTP_REQ_PRONAME_PARAMS), 1, NULL);
    if (values && Z_TYPE_P(values) == IS_ARRAY) {
        zend_hash_copy(Z_ARRVAL_P(params), Z_ARRVAL_P(values), (copy_ctor_func_t) zval_add_ref);
        return 1;
    }
    return 0;
}
/* }}} */

/* {{{ proto object Asf_Http_Request::__construct(string $request_uri, string $base_uri)
*/
PHP_METHOD(asf_http_request, __construct)
{
    zend_string *request_uri = NULL;
    zend_string *base_uri = NULL;
    asf_http_req_t *self = getThis();

    if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "|SS", &request_uri, &base_uri) == FAILURE) {
        return;
    }

    (void)asf_http_req_instance(self, base_uri, request_uri);
}
/* }}} */

/* {{{ proto bool Asf_Http_Request::isGet(void)
*/
ASF_HTTP_REQ_IS_METHOD(Get);
/* }}} */

/* {{{ proto bool Asf_Http_Request::isPost(void)
*/
ASF_HTTP_REQ_IS_METHOD(Post);
/* }}} */

/* {{{ proto bool Asf_Http_Request::isPut(void)
*/
ASF_HTTP_REQ_IS_METHOD(Put);
/* }}} */

/* {{{ proto bool Asf_Http_Request::isDelete(void)
*/
ASF_HTTP_REQ_IS_METHOD(Delete);
/* }}} */

/* {{{ proto bool Asf_Http_Request::isPatch(void)
*/
ASF_HTTP_REQ_IS_METHOD(Patch);
/* }}} */

/* {{{ proto bool Asf_Http_Request::isHead(void)
*/
ASF_HTTP_REQ_IS_METHOD(Head);
/* }}} */

/* {{{ proto bool Asf_Http_Request::isOptions(void)
*/
ASF_HTTP_REQ_IS_METHOD(Options);
/* }}} */

/* {{{ proto bool Asf_Http_Request::isCli(void)
*/
ASF_HTTP_REQ_IS_METHOD(Cli);
/* }}} */

/* {{{ proto bool Asf_Http_Request::isXmlHttpRequest(void)
*/
PHP_METHOD(asf_http_request, isXmlHttpRequest)
{
    zval *header = asf_http_req_pg_find_len(TRACK_VARS_SERVER, "HTTP_X_REQUESTED_WITH", 21);
    if (header && Z_TYPE_P(header) == IS_STRING
            && strncasecmp("XMLHttpRequest", Z_STRVAL_P(header), Z_STRLEN_P(header)) == 0) {
        RETURN_TRUE;
    }

    RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool Asf_Http_Request::isTrace(void)
*/
ASF_HTTP_REQ_IS_METHOD(Trace);
/* }}} */

/* {{{ proto bool Asf_Http_Request::isConnect(void)
*/
ASF_HTTP_REQ_IS_METHOD(Connect);
/* }}} */

/* {{{ proto bool Asf_Http_Request::isFlashRequest(void)
*/
ASF_HTTP_REQ_IS_METHOD(FlashRequest);
/* }}} */

/* {{{ proto bool Asf_Http_Request::isPropFind(void)
*/
ASF_HTTP_REQ_IS_METHOD(PropFind);
/* }}} */

/* {{{ proto array Asf_Http_Request::getParams(void)
*/
PHP_METHOD(asf_http_request, getParams)
{
    zval *params = zend_read_property(asf_http_request_ce,
            getThis(), ZEND_STRL(ASF_HTTP_REQ_PRONAME_PARAMS), 1, NULL);
    RETURN_ZVAL(params, 1, 0);
}
/* }}} */

/* {{{ proto mixed Asf_Http_Request::getParam(string $name [, $mixed $default = NULL])
*/
PHP_METHOD(asf_http_request, getParam)
{
    zend_string *name = NULL;
    zval *def = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|z", &name, &def) == FAILURE) {
        return;
    } else {
        zval *params = zend_read_property(asf_http_request_ce, getThis(), ZEND_STRL(ASF_HTTP_REQ_PRONAME_PARAMS), 1, NULL);
        zval *value = zend_hash_find(Z_ARRVAL_P(params), name);
        if (value) {
            RETURN_ZVAL(value, 1, 0);
        }
        if (def) {
            RETURN_ZVAL(def, 1, 0);
        }
    }

    RETURN_NULL();
}
/* }}} */

/* {{{ proto string Asf_Http_Request::getModuleName(void)
*/
PHP_METHOD(asf_http_request, getModuleName)
{
    zval *module = zend_read_property(asf_http_request_ce, getThis(), ZEND_STRL(ASF_HTTP_REQ_PRONAME_MODULE), 1, NULL);
    RETVAL_ZVAL(module, 1, 0);
}
/* }}} */

/* {{{ proto string Asf_Http_Request::getServiceName(void)
*/
PHP_METHOD(asf_http_request, getServiceName)
{
    zval *server = zend_read_property(asf_http_request_ce,
            getThis(), ZEND_STRL(ASF_HTTP_REQ_PRONAME_SERVICE), 1, NULL);
    RETVAL_ZVAL(server, 1, 0);
}
/* }}} */

/* {{{ proto string Asf_Http_Request::getActionName(void)
*/
PHP_METHOD(asf_http_request, getActionName)
{
    zval *action = zend_read_property(asf_http_request_ce,
            getThis(), ZEND_STRL(ASF_HTTP_REQ_PRONAME_ACTION), 1, NULL);
    RETVAL_ZVAL(action, 1, 0);
}
/* }}} */

/* {{{ proto string Asf_Http_Request::getMethod(void)
*/
PHP_METHOD(asf_http_request, getMethod)
{
    zval *method = zend_read_property(asf_http_request_ce, getThis(), ZEND_STRL(ASF_HTTP_REQ_PRONAME_METHOD), 1, NULL);
    RETURN_ZVAL(method, 1, 0);
}
/* }}} */

/* {{{ proto string Asf_Http_Request::getBaseUri(string $name)
*/
PHP_METHOD(asf_http_request, getBaseUri)
{
    zval *uri = zend_read_property(asf_http_request_ce,
            getThis(), ZEND_STRL(ASF_HTTP_REQ_PRONAME_BASE), 1, NULL);
    RETURN_ZVAL(uri, 1, 0);
}
/* }}} */

/* {{{ proto string Asf_Http_Request::getRequestUri(string $name)
*/
PHP_METHOD(asf_http_request, getRequestUri)
{
    zval *uri = zend_read_property(asf_http_request_ce, getThis(), ZEND_STRL(ASF_HTTP_REQ_PRONAME_URI), 1, NULL);
    RETURN_ZVAL(uri, 1, 0);
}
/* }}} */

/* {{{ proto array Asf_Http_Request::getQuery(mixed $name [, mixed $default = NULL])
*/
ASF_HTTP_REQ_METHOD(Query, TRACK_VARS_GET);
/* }}} */

/* {{{ proto array Asf_Http_Request::getPost(mixed $name [, mixed $default = NULL])
*/
ASF_HTTP_REQ_METHOD(Post, TRACK_VARS_POST);
/* }}} */

/* {{{ proto array Asf_Http_Request::getRequet(mixed $name [, mixed $default = NULL])
*/
ASF_HTTP_REQ_METHOD(Request, TRACK_VARS_REQUEST);
/* }}} */

/* {{{ proto array Asf_Http_Request::getFiles(mixed $name [, mixed $default = NULL])
*/
ASF_HTTP_REQ_METHOD(Files, TRACK_VARS_FILES);
/* }}} */

/* {{{ proto array Asf_Http_Request::getCookie(mixed $name [, mixed $default = NULL])
*/
ASF_HTTP_REQ_METHOD(Cookie,	TRACK_VARS_COOKIE);
/* }}} */

/* {{{ proto array Asf_Http_Request::getServer(mixed $name [, mixed $default = NULL])
*/
ASF_HTTP_REQ_METHOD(Server, TRACK_VARS_SERVER);
/* }}} */

/* {{{ proto string Asf_Http_Request::getEnv(mixed $name [, mixed $default = NULL])
*/
ASF_HTTP_REQ_METHOD(Env, TRACK_VARS_ENV);
/* }}} */

/* {{{ proto bool Asf_Http_Request::hasPost(string $name)
*/
ASF_HTTP_REQ_HAS(Post, TRACK_VARS_POST);
/* }}} */

/* {{{ proto bool Asf_Http_Request::hasQuery(string $name)
*/
ASF_HTTP_REQ_HAS(Query, TRACK_VARS_GET);
/* }}} */

/* {{{ proto bool Asf_Http_Request::hasServer(string $name)
*/
ASF_HTTP_REQ_HAS(Server, TRACK_VARS_SERVER);
/* }}} */

/* {{{ proto bool Asf_Http_Request::hasFiles(string $name)
*/
ASF_HTTP_REQ_HAS(Files, TRACK_VARS_FILES);
/* }}} */

/* {{{ asf_http_request_methods[]
*/
zend_function_entry asf_http_request_methods[] = {
    PHP_ME(asf_http_request, __construct,	NULL, ZEND_ACC_PUBLIC  | ZEND_ACC_CTOR)
    PHP_ME(asf_http_request, isGet,			NULL, ZEND_ACC_PUBLIC)
    PHP_ME(asf_http_request, isPost,		NULL, ZEND_ACC_PUBLIC)
    PHP_ME(asf_http_request, isPut,			NULL, ZEND_ACC_PUBLIC)
    PHP_ME(asf_http_request, isDelete,		NULL, ZEND_ACC_PUBLIC)
    PHP_ME(asf_http_request, isPatch,		NULL, ZEND_ACC_PUBLIC)
    PHP_ME(asf_http_request, isHead,		NULL, ZEND_ACC_PUBLIC)
    PHP_ME(asf_http_request, isOptions,		NULL, ZEND_ACC_PUBLIC)
    PHP_ME(asf_http_request, isCli,			NULL, ZEND_ACC_PUBLIC)
    PHP_ME(asf_http_request, isXmlHttpRequest,NULL, ZEND_ACC_PUBLIC)
    PHP_ME(asf_http_request, isTrace,		NULL, ZEND_ACC_PUBLIC)
    PHP_ME(asf_http_request, isConnect,		NULL, ZEND_ACC_PUBLIC)
    PHP_ME(asf_http_request, isFlashRequest,NULL, ZEND_ACC_PUBLIC)
    PHP_ME(asf_http_request, isPropFind,	NULL, ZEND_ACC_PUBLIC)

    PHP_ME(asf_http_request, getParam,		asf_http_requestinterface_get_param_arginfo,  ZEND_ACC_PUBLIC)
    PHP_ME(asf_http_request, getParams,		NULL, ZEND_ACC_PUBLIC)
    PHP_ME(asf_http_request, getModuleName,	NULL, ZEND_ACC_PUBLIC)
    PHP_ME(asf_http_request, getServiceName,NULL, ZEND_ACC_PUBLIC)
    PHP_ME(asf_http_request, getActionName,	NULL, ZEND_ACC_PUBLIC)
    PHP_ME(asf_http_request, getMethod,		NULL, ZEND_ACC_PUBLIC)
    PHP_ME(asf_http_request, getBaseUri,	NULL, ZEND_ACC_PUBLIC)
    PHP_ME(asf_http_request, getRequestUri,	NULL, ZEND_ACC_PUBLIC)

    PHP_ME(asf_http_request, getQuery,		NULL, ZEND_ACC_PUBLIC)
    PHP_ME(asf_http_request, getPost,		NULL, ZEND_ACC_PUBLIC)
    PHP_ME(asf_http_request, getRequest,	NULL, ZEND_ACC_PUBLIC)
    PHP_ME(asf_http_request, getFiles,		NULL, ZEND_ACC_PUBLIC)
    PHP_ME(asf_http_request, getCookie,		NULL, ZEND_ACC_PUBLIC)
    PHP_ME(asf_http_request, getServer,		asf_http_requestinterface_getserver_arginfo,  ZEND_ACC_PUBLIC)
    PHP_ME(asf_http_request, getEnv,		asf_http_requestinterface_getenv_arginfo,		ZEND_ACC_PUBLIC)

    PHP_ME(asf_http_request, hasPost,		asf_http_requestinterface_hasname_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(asf_http_request, hasQuery,		asf_http_requestinterface_hasname_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(asf_http_request, hasServer,		asf_http_requestinterface_hasname_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(asf_http_request, hasFiles,		asf_http_requestinterface_hasname_arginfo, ZEND_ACC_PUBLIC)

    PHP_FE_END
};
/* }}} */

ASF_INIT_CLASS(http_request) /* {{{ */
{
    ASF_REGISTER_CLASS_PARENT(asf_http_request, Asf_Http_Request, Asf\\Http\\Request, ZEND_ACC_FINAL);

    zend_declare_property_null(asf_http_request_ce, ZEND_STRL(ASF_HTTP_REQ_PRONAME_MODULE), ZEND_ACC_PROTECTED);
    zend_declare_property_null(asf_http_request_ce, ZEND_STRL(ASF_HTTP_REQ_PRONAME_ACTION), ZEND_ACC_PROTECTED);
    zend_declare_property_null(asf_http_request_ce, ZEND_STRL(ASF_HTTP_REQ_PRONAME_SERVICE), ZEND_ACC_PROTECTED);
    zend_declare_property_null(asf_http_request_ce, ZEND_STRL(ASF_HTTP_REQ_PRONAME_METHOD), ZEND_ACC_PROTECTED);
    zend_declare_property_null(asf_http_request_ce, ZEND_STRL(ASF_HTTP_REQ_PRONAME_PARAMS), ZEND_ACC_PROTECTED);

    zend_declare_property_string(asf_http_request_ce, ZEND_STRL(ASF_HTTP_REQ_PRONAME_URI), "", ZEND_ACC_PROTECTED);
    zend_declare_property_string(asf_http_request_ce, ZEND_STRL(ASF_HTTP_REQ_PRONAME_BASE), "", ZEND_ACC_PROTECTED);

    ASF_INIT(http_requestinterface);

    zend_class_implements(asf_http_request_ce, 1, asf_http_requestinterface_ce);

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
