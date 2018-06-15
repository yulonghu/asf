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
#include "asf_exception.h"
#include "asf_dispatcher.h"
#include "kernel/asf_func.h"

#include "asf_router.h"
#include "routes/asf_route_pathinfo.h"
#include "routes/asf_route_query.h"
#include "routes/asf_route_dot.h"

zend_class_entry *asf_router_ce;

/* {{{ ARG_INFO
*/
ZEND_BEGIN_ARG_INFO_EX(asf_route_route_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, uri)
ZEND_END_ARG_INFO()
/* }}} */

void asf_router_instance(asf_router_t *this_ptr) /* {{{ */
{
    zval *config = ASF_G(route);
    zval *pzval = NULL;

    if (!config || IS_ARRAY != Z_TYPE_P(config)) {
default_route:
        (void)asf_route_pathinfo_instance(this_ptr);
        return;
    }

    if ((pzval = zend_hash_str_find(Z_ARRVAL_P(config), ZEND_STRL("type"))) == NULL ||
            IS_STRING != Z_TYPE_P(pzval)) {
        goto default_route;
    }

    if (0 == strncasecmp(Z_STRVAL_P(pzval), ZEND_STRL("query"))) {
        zval *module_name	= zend_hash_str_find(Z_ARRVAL_P(config), ZEND_STRL("module_name"));
        zval *service_name  = zend_hash_str_find(Z_ARRVAL_P(config), ZEND_STRL("service_name"));
        zval *action_name	= zend_hash_str_find(Z_ARRVAL_P(config), ZEND_STRL("action_name"));

        (void)asf_route_query_instance(this_ptr, module_name, service_name, action_name);
    } else if (0 == strncasecmp(Z_STRVAL_P(pzval), ZEND_STRL("dot"))) {
        zval *match	= zend_hash_str_find(Z_ARRVAL_P(config), ZEND_STRL("dot_name"));
        (void)asf_route_dot_instance(this_ptr, match);
    }

    if (IS_OBJECT != Z_TYPE_P(this_ptr)) {
        goto default_route;
    }
}
/* }}} */

/* {{{ proto bool Asf_Router::addRoute(string $name, array $data)
*/
PHP_METHOD(asf_router, addRoute)
{
    zend_string *name = NULL;
    zval *match = NULL;
    zval route;
    _Bool ret = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "Sa", &name, &match) == FAILURE) {
        return;
    }

    if (UNEXPECTED(asf_func_isempty(ZSTR_VAL(name)))) {
        asf_trigger_error(ASF_ERR_ROUTE_FAILED, "Parameter 'name' must be a string");
        return;
    }

    if (zend_hash_num_elements(Z_ARRVAL_P(match)) < 1) {
        asf_trigger_error(ASF_ERR_ROUTE_FAILED, "Parameter 'match' is array elements and more than zero");
        return;
    }

    zval *dispatcher = zend_read_static_property(asf_dispatcher_ce, ZEND_STRL(ASF_DISP_PRONAME_INS), 1);
    if (UNEXPECTED(!dispatcher)) {
        asf_trigger_error(ASF_ERR_ROUTE_FAILED, "Unkown Error");
        return;
    }

    zend_class_entry *ce = Z_OBJCE_P(dispatcher);
    zval *routes = zend_read_property(ce, dispatcher, ZEND_STRL(ASF_DISP_PRONAME_ROUTES), 1, NULL);

    if (!routes || Z_TYPE_P(routes) != IS_ARRAY) {
        array_init(&route);
        ret = zend_hash_update(Z_ARRVAL(route), name, match);
        zend_update_property(ce, dispatcher, ZEND_STRL(ASF_DISP_PRONAME_ROUTES), &route);
        zval_ptr_dtor(&route);
    } else {
        ret = zend_hash_update(Z_ARRVAL_P(routes), name, match);
    }

    Z_TRY_ADDREF_P(match);

    RETURN_BOOL(ret);
}
/* }}} */

/* {{{ proto array Asf_Router::getRoutes(void)
*/
PHP_METHOD(asf_router, getRoutes)
{
    zval *dispatcher = zend_read_static_property(asf_dispatcher_ce, ZEND_STRL(ASF_DISP_PRONAME_INS), 1);
    ZVAL_COPY(return_value, zend_read_property(Z_OBJCE_P(dispatcher), dispatcher,
                ZEND_STRL(ASF_DISP_PRONAME_ROUTES), 1, NULL));
}
/*}}}*/

/* {{{ asf_router_methods[]
*/
zend_function_entry asf_router_methods[] = {
    PHP_ME(asf_router, addRoute,    NULL, ZEND_ACC_PUBLIC)
    PHP_ME(asf_router, getRoutes,   NULL, ZEND_ACC_PUBLIC)
    PHP_FE_END
};
/* }}} */

ASF_INIT_CLASS(router) /* {{{ */
{
    ASF_REGISTER_CLASS_PARENT(asf_router, Asf_Router_AbstractRoute, Asf\\Router\\AbstractRoute, ZEND_ACC_EXPLICIT_ABSTRACT_CLASS);

    ASF_INIT(route_pathinfo);
    ASF_INIT(route_query);
    ASF_INIT(route_dot);

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
