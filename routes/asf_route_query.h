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

#ifndef ASF_ROUTE_QUERY_H
#define ASF_ROUTE_QUERY_H

#define ASF_ROUTE_PROPERY_QUERY_MODULE_NAME		"module"
#define	ASF_ROUTE_PROPERY_QUERY_SERVICE_NAME 	"service"
#define ASF_ROUTE_PROPERY_QUERY_ACTION_NAME		"action"

extern zend_class_entry *asf_route_query_ce;

void asf_route_query_instance(asf_route_t *this_ptr, zval *module_name, zval *service_name, zval *action_name);
int asf_route_query_set(asf_http_req_t *request, zval *module, zval *service, zval *action);
_Bool asf_route_query_run(asf_route_t *route, asf_http_req_t *request);

ASF_INIT_CLASS(route_query);

#endif
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
