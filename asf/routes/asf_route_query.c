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
#include "asf_dispatcher.h" /* asf_dispatcher_is_right_module */
#include "asf_router.h"
#include "http/asf_http_request.h"
#include "routes/asf_route_query.h"

zend_class_entry *asf_route_query_ce;

int asf_route_query_set(asf_http_req_t *request, zval *module, zval *service, zval *action) /* {{{ */
{
	zend_class_entry *req_ce = Z_OBJCE_P(request);

	if (module && Z_TYPE_P(module) == IS_STRING && asf_dispatcher_is_right_module(Z_STRVAL_P(module), Z_STRLEN_P(module))) {
		zend_update_property(req_ce, request, ZEND_STRL(ASF_HTTP_REQ_PRONAME_MODULE), module);
	}

	if (service) {
		zend_update_property(req_ce, request, ZEND_STRL(ASF_HTTP_REQ_PRONAME_SERVICE), service);
	}

	if (action) {
		zend_update_property(req_ce, request, ZEND_STRL(ASF_HTTP_REQ_PRONAME_ACTION), action);
	}

    /* request->getParam(), request->getParams() */
	zval params;
    HashTable *get_ht = NULL;
	zval *pzval  = NULL;
	zend_string *key = NULL;

	array_init(&params);
	get_ht = Z_ARRVAL_P(&PG(http_globals)[TRACK_VARS_GET]);

	ZEND_HASH_FOREACH_STR_KEY_VAL(get_ht, key, pzval) {
		if (!key) {
			continue;
		}

		ZVAL_DEREF(pzval);
		Z_TRY_ADDREF_P(pzval);
		zend_hash_update(Z_ARRVAL(params), key, pzval);

	} ZEND_HASH_FOREACH_END();

	(void)asf_http_req_set_params_multi(request, &params);
    zval_ptr_dtor(&params);

	return 1;
}
/* }}} */

_Bool asf_route_query_run(asf_route_t *route, asf_http_req_t *request) /* {{{ */
{
	zval *module = NULL, *service = NULL, *action = NULL;
	zval *nmodule = NULL, *nservice = NULL, *naction = NULL;

	nmodule	= zend_read_property(asf_route_query_ce,
			route, ZEND_STRL(ASF_ROUTE_PROPERY_QUERY_MODULE_NAME), 1, NULL);
	nservice = zend_read_property(asf_route_query_ce,
			route, ZEND_STRL(ASF_ROUTE_PROPERY_QUERY_SERVICE_NAME), 1, NULL);
	naction = zend_read_property(asf_route_query_ce,
			route, ZEND_STRL(ASF_ROUTE_PROPERY_QUERY_ACTION_NAME), 1, NULL);

	module		= asf_http_req_pg_find_len(TRACK_VARS_GET, Z_STRVAL_P(nmodule), Z_STRLEN_P(nmodule));
	service		= asf_http_req_pg_find_len(TRACK_VARS_GET, Z_STRVAL_P(nservice), Z_STRLEN_P(nservice));
	action		= asf_http_req_pg_find_len(TRACK_VARS_GET, Z_STRVAL_P(naction), Z_STRLEN_P(naction));

	zval *q_string = asf_http_req_pg_find_len(TRACK_VARS_SERVER, "QUERY_STRING", sizeof("QUERY_STRING") - 1);
	if (q_string) {
		ASF_G(settled_uri) = zend_string_copy(Z_STR_P(q_string));
	}

	return asf_route_query_set(request, module, service, action);
}
/* }}} */

void asf_route_query_instance(asf_route_t *this_ptr, zval *module_name, zval *service_name, zval *action_name) /* {{{ */
{
	if (Z_ISUNDEF_P(this_ptr)) {
		object_init_ex(this_ptr, asf_route_query_ce);
	}

	if (module_name) {
		zend_update_property(asf_route_query_ce, this_ptr, ZEND_STRL(ASF_ROUTE_PROPERY_QUERY_MODULE_NAME), module_name);
	} else {
		zend_update_property_stringl(asf_route_query_ce, this_ptr, ZEND_STRL(ASF_ROUTE_PROPERY_QUERY_MODULE_NAME), "m", 1);
	}

	if (service_name) {
		zend_update_property(asf_route_query_ce, this_ptr, ZEND_STRL(ASF_ROUTE_PROPERY_QUERY_SERVICE_NAME), service_name);
	} else {
		zend_update_property_stringl(asf_route_query_ce, this_ptr, ZEND_STRL(ASF_ROUTE_PROPERY_QUERY_SERVICE_NAME), "s", 1);
	}

	if (action_name) {
		zend_update_property(asf_route_query_ce, this_ptr, ZEND_STRL(ASF_ROUTE_PROPERY_QUERY_ACTION_NAME), action_name);
	} else {
		zend_update_property_stringl(asf_route_query_ce, this_ptr, ZEND_STRL(ASF_ROUTE_PROPERY_QUERY_ACTION_NAME), "a", 1);
	}
}
/* }}} */

ASF_INIT_CLASS(route_query) /* {{{ */
{
	ASF_REGISTER_CLASS_INTERNAL(asf_route_query, Asf_Route_Query, Asf\\Route\\Query, asf_router_ce, NULL);

	zend_declare_property_null(asf_route_query_ce, ZEND_STRL(ASF_ROUTE_PROPERY_QUERY_MODULE_NAME), ZEND_ACC_PROTECTED);
	zend_declare_property_null(asf_route_query_ce, ZEND_STRL(ASF_ROUTE_PROPERY_QUERY_SERVICE_NAME), ZEND_ACC_PROTECTED);
	zend_declare_property_null(asf_route_query_ce, ZEND_STRL(ASF_ROUTE_PROPERY_QUERY_ACTION_NAME), ZEND_ACC_PROTECTED);

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
