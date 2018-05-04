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

#ifndef ASF_HTTP_REQUEST_H
#define ASF_HTTP_REQUEST_H

#define ASF_HTTP_REQ_PRONAME_METHOD		"_method"
#define ASF_HTTP_REQ_PRONAME_URI		"_uri"
#define ASF_HTTP_REQ_PRONAME_PARAMS		"_params"
#define ASF_HTTP_REQ_PRONAME_MODULE		"_module"
#define ASF_HTTP_REQ_PRONAME_ACTION		"_action"
#define ASF_HTTP_REQ_PRONAME_SERVICE	"_service"
#define ASF_HTTP_REQ_PRONAME_BASE		"_base_uri"

zval *asf_http_req_instance(asf_http_req_t *this_ptr, zend_string *base_uri, zend_string *request_uri);
zval *asf_http_req_pg_find_ex(uint vars, char *name, size_t len);

int asf_http_req_set_params_multi(asf_http_req_t *request, zval *values);

#define asf_http_req_pg_find(vars) asf_http_req_pg_find_ex((vars), NULL, 0)
#define asf_http_req_pg_find_len(vars, type, len) asf_http_req_pg_find_ex((vars), (type), (len))

#define ASF_HTTP_REQ_IS_METHOD(type) \
    PHP_METHOD(asf_http_request, is##type) {\
        zval *method = zend_read_property(Z_OBJCE_P(getThis()), \
                getThis(), ZEND_STRL(ASF_HTTP_REQ_PRONAME_METHOD), 0, NULL); \
        if (zend_string_equals_literal_ci(Z_STR_P(method), #type)) { \
            RETURN_TRUE; \
        }\
        RETURN_FALSE; \
    }

#define ASF_HTTP_REQ_METHOD(type, vars) \
    PHP_METHOD(asf_http_request, get##type) { \
        char *name = NULL; zval *retval, *def = NULL; size_t name_len = 0; \
        if (ZEND_NUM_ARGS() == 0) { \
            retval = asf_http_req_pg_find(vars); \
        } else if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|z", &name, &name_len, &def) == FAILURE) { \
            return; \
        } else { \
            retval = asf_http_req_pg_find_len(vars, name, name_len); \
            if (!retval && def != NULL) { \
                RETURN_ZVAL(def, 1, 0); \
            } \
        } \
        if (retval) { \
            RETURN_ZVAL(retval, 1, 0); \
        } else { \
            RETURN_NULL(); \
        } \
    }

#define ASF_HTTP_REQ_HAS(type, vars) \
    PHP_METHOD(asf_http_request, has##type) { \
        char *name = NULL; size_t name_len = 0; zval *retval = NULL; \
        if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &name, &name_len) == FAILURE) { \
            return; \
        } \
        retval = asf_http_req_pg_find_len(vars, name, name_len); \
        RETURN_BOOL(retval); \
    }

extern zend_class_entry *asf_http_request_ce;

ASF_INIT_CLASS(http_request);

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
