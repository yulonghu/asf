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

#include "php_asf.h"
#include "asf_namespace.h"
#include "asf_dispatcher.h" /* asf_dispatcher_is_right_module */
#include "http/asf_http_request.h"

#include "asf_router.h"
#include "routes/asf_route_pathinfo.h"
#include "zend_smart_str.h" /* for smart_str */

zend_class_entry * asf_route_pathinfo_ce;

void asf_route_pathinfo_instance(asf_route_t *this_ptr) /* {{{ */
{
	if (Z_ISUNDEF_P(this_ptr)) {
		object_init_ex(this_ptr, asf_route_pathinfo_ce);
	}
}
/* }}} */

static void asf_router_parse_parameters_pathinfo(char *uri, zval *params) /* {{{ */
{
	char *key = NULL, *ptrptr = NULL, *tmp = NULL, *value = NULL;
	zval val;
	uint key_len;

	array_init(params);

	tmp = estrdup(uri);
	key = php_strtok_r(tmp, ASF_ROUTER_DELIMITER, &ptrptr);
	while (key) {
		key_len = strlen(key);
		if (key_len) {
			value = php_strtok_r(NULL, ASF_ROUTER_DELIMITER, &ptrptr);
			if (value && strlen(value)) {
				ZVAL_STRING(&val, value);
			} else {
				ZVAL_NULL(&val);
			}
			zend_hash_str_update(Z_ARRVAL_P(params), key, key_len, &val);
		}

		key = php_strtok_r(NULL, ASF_ROUTER_DELIMITER, &ptrptr);
	}

	efree(tmp);
}
/* }}} */

/* Excerpt from the yaf, http://yaf.laruence.com/manual/ */
_Bool asf_route_pathinfo_run(asf_route_t *route, asf_http_req_t *request) /* {{{ */
{
	zval params;
	char *module = NULL, *service = NULL, *action = NULL, *rest = NULL;
	size_t module_len = 0, service_len = 0, action_len = 0;

	zval *zuri = NULL, *base_uri = NULL;
	zend_string *req_uri = NULL;

	zuri = zend_read_property(Z_OBJCE_P(request), request, ZEND_STRL(ASF_HTTP_REQ_PRONAME_URI), 1, NULL);
	base_uri = zend_read_property(Z_OBJCE_P(request), request, ZEND_STRL(ASF_HTTP_REQ_PRONAME_BASE), 1, NULL);

	if (base_uri && IS_STRING == Z_TYPE_P(base_uri)
			&& !strncasecmp(Z_STRVAL_P(zuri), Z_STRVAL_P(base_uri), Z_STRLEN_P(base_uri))) {
		req_uri = zend_string_init(Z_STRVAL_P(zuri) + Z_STRLEN_P(base_uri), Z_STRLEN_P(zuri) - Z_STRLEN_P(base_uri), 0);
	} else {
		req_uri = zend_string_init(Z_STRVAL_P(zuri), Z_STRLEN_P(zuri), 0);
	}

    /**
     * urls
     *
     * req_uri = /
     * req_uri = /index.php
     * req_uri = /dir/index.php
     * req_uri = /index/
     * req_uri = /index/index/
     * req_uri = /index/index/index/
     * req_uri = /admin/index/index/
     */

    do {
        if (ZSTR_LEN(req_uri) == 0 ||
                (ZSTR_LEN(req_uri) == 1 && *ZSTR_VAL(req_uri) == '/')) {
            break;
        }

		/* Notice: Suffix name must be lowercase */
		if (strncmp(ZSTR_VAL(req_uri) + (ZSTR_LEN(req_uri) - 4), ".php", 4) == 0) {
			break;
		}

#define strip_slashs(p) while (*p == ' ' || *p == '/') { ++p; }
        
		char *uri = ZSTR_VAL(req_uri);
        char *a, *b = NULL;

        a = uri;
		strip_slashs(a);

        if ((b = strstr(a, ASF_ROUTER_DELIMITER)) != NULL) {
            if (asf_dispatcher_is_right_module(a, b - a)) {
                module = a;
                module_len = b - a;
				a = b + 1;
				strip_slashs(a);
				if ((b = strstr(a, ASF_ROUTER_DELIMITER)) != NULL) {
					service = a;
					service_len = b - a;			
					a = b + 1;
				}
            } else {
                service = a;
                service_len = b - a;
				a = b + 1;
            }
        }

		strip_slashs(a);

		if ((b = strstr(a, ASF_ROUTER_SLASH)) != NULL) {
			action = a;
			action_len = b - a;
			a  = b + 1;
		}

		if (*a != '\0') {
            do {
				if (!module && !service && !action) {
                    if (asf_dispatcher_is_right_module(a, strlen(a))) {
                        module = a;
                        module_len = strlen(a);
                        break;
                    }
                }

                if (!service) {
                    service = a;
                    service_len = strlen(a);
                    break;
                }

                if (!action) {
                    action = a;
                    action_len = strlen(a);
                    break;
                }

                rest = a;
            } while (0);
        }

    } while (0);
    
	if (module) {
		zend_str_tolower(module, module_len);
		zend_update_property_stringl(Z_OBJCE_P(request),
				request, ZEND_STRL(ASF_HTTP_REQ_PRONAME_MODULE), module, module_len);
	}

	if (service) {
		zend_str_tolower(service, service_len);
		zend_update_property_stringl(Z_OBJCE_P(request),
				request, ZEND_STRL(ASF_HTTP_REQ_PRONAME_SERVICE), service, service_len);
	}

	if (action) {
		zend_str_tolower(action, action_len);
		zend_update_property_stringl(Z_OBJCE_P(request),
				request, ZEND_STRL(ASF_HTTP_REQ_PRONAME_ACTION), action, action_len);
	}

	if (rest) {
		(void)asf_router_parse_parameters_pathinfo(rest, &params);
		(void)asf_http_req_set_params_multi(request, &params);
		zval_ptr_dtor(&params);
	}

	zend_string_release(req_uri);

	return 1;
}
/* }}} */

ASF_INIT_CLASS(route_pathinfo) /* {{{ */
{
	ASF_REGISTER_CLASS_INTERNAL(asf_route_pathinfo, Asf_Route_Pathinfo, Asf\\Route\\Pathinfo, asf_router_ce, NULL);

	return SUCCESS;
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
