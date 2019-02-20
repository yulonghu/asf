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
#include "kernel/asf_namespace.h"
#include "asf_router.h"
#include "http/asf_http_request.h"
#include "routes/asf_route_dot.h"
#include "routes/asf_route_query.h"

zend_class_entry *asf_route_dot_ce;

_Bool asf_route_dot_run(asf_route_t *route, asf_http_req_t *request) /* {{{ */
{
	zval *dot_name = NULL, *uri = NULL;
	zend_string *req_uri = NULL;
	int tok_count = 0;
	zval *module = NULL, *service = NULL, *action = NULL;
	zval args;

	dot_name = zend_read_property(asf_route_dot_ce, route, ZEND_STRL(ASF_ROUTE_DOT_PROPETY_NAME), 1, NULL);
	uri = asf_http_req_pg_find_len(TRACK_VARS_GET, Z_STRVAL_P(dot_name), Z_STRLEN_P(dot_name));

	if (uri && strstr(Z_STRVAL_P(uri), ".") != NULL) {
		char *seg = NULL, *saveptr = NULL, *skey = NULL;
		size_t len = 0;

		array_init(&args);
		ASF_G(settled_uri) = zend_string_copy(Z_STR_P(uri));

        /* Create string the 'skey' */
        skey = estrndup(Z_STRVAL_P(uri), Z_STRLEN_P(uri));
		seg = php_strtok_r(skey, ".", &saveptr);

		while(seg) {
			if (tok_count > 2) {
				break;
			}

			len = strlen(seg);
			if (len) {
				add_index_string(&args, tok_count, seg);
				tok_count++;
			}

			seg = php_strtok_r(NULL, ".", &saveptr);
		}

        efree(skey);

		switch (tok_count) {
			case 2:
				service = zend_hash_index_find(Z_ARRVAL(args), 0);
				action = zend_hash_index_find(Z_ARRVAL(args), 1);
				break;
			case 3:
				module = zend_hash_index_find(Z_ARRVAL(args), 0);
				service = zend_hash_index_find(Z_ARRVAL(args), 1);
				action = zend_hash_index_find(Z_ARRVAL(args), 2);
				break;
		}
	}

	tok_count = asf_route_query_set(request, module, service, action);

	if (IS_ARRAY == Z_TYPE(args)) {
		zval_ptr_dtor(&args);
	}

	return tok_count;
}
/* }}} */

void asf_route_dot_instance(asf_route_t *this_ptr, zval *name) /* {{{ */
{
    if (Z_ISUNDEF_P(this_ptr)) {
		object_init_ex(this_ptr, asf_route_dot_ce);
	}

	if (!name || IS_STRING != Z_TYPE_P(name) || Z_STRLEN_P(name) < 1) {
		zend_update_property_stringl(asf_route_dot_ce, this_ptr, ZEND_STRL(ASF_ROUTE_DOT_PROPETY_NAME), "method", 6);
	} else {
		zend_update_property(asf_route_dot_ce, this_ptr, ZEND_STRL(ASF_ROUTE_DOT_PROPETY_NAME), name);
	}
}
/* }}} */

ASF_INIT_CLASS(route_dot) /* {{{ */
{
	ASF_REGISTER_CLASS_INTERNAL(asf_route_dot, Asf_Route_Dot, Asf\\Route\\Dot, asf_router_ce, NULL);

	zend_declare_property_null(asf_route_dot_ce, ZEND_STRL(ASF_ROUTE_DOT_PROPETY_NAME), ZEND_ACC_PROTECTED);

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
