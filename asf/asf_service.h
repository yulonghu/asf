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

#ifndef ASF_SERVICE_H
#define ASF_SERVICE_H

#define ASF_SERVICE_PROPERTY_NAME_REQUEST	"_req"
#define ASF_SERVICE_PROPERTY_NAME_RESPONSE	"_res"
#define ASF_SERVICE_PROPERTY_NAME_LOGGER	"_log"

extern zend_class_entry *asf_service_ce;

void asf_service_construct(zend_class_entry *ce, asf_service_t *self, asf_http_req_t *request, asf_http_rep_t *response);

ASF_INIT_CLASS(service);

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
