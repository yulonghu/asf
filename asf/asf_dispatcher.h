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

#ifndef ASF_DISPATCHER_H
#define ASF_DISPATCHER_H

#define ASF_DISP_PRONAME_INS    "_ins"
#define ASF_DISP_PRONAME_REQ	"_request"
#define ASF_DISP_PRONAME_ROUTER "_router"
#define ASF_DISP_PRONAME_ROUTES "_r"
#define ASF_APP_PRONAME_ERR_HANDLER "_eh"

#define ASF_DISPATCHER_ROUTER_A 18
#define ASF_DISPATCHER_ROUTER_B 15
#define ASF_DISPATCHER_ROUTER_C 13

extern zend_class_entry *asf_dispatcher_ce;

void asf_dispatcher_instance(zval *this_ptr, asf_http_req_t *request);
_Bool asf_dispatcher_ondispatch(asf_disp_t *dispatcher, asf_http_rep_t *response_ptr);
_Bool asf_dispatcher_is_right_module(char *module_name, size_t module_name_len);

ASF_INIT_CLASS(dispatcher);

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
