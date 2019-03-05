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
#include "http/asf_http_request.h"
#include "kernel/asf_func.h"
#include "asf_logger.h"
#include "asf_service.h"
#include "asf_exception.h"
#include "asf_logger.h"

zend_class_entry *asf_service_ce;

/* {{{ ARG_INFO
*/
ZEND_BEGIN_ARG_INFO_EX(asf_service_logger_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, file_name)
ZEND_END_ARG_INFO()
/* }}} */

void asf_service_construct(zend_class_entry *ce, asf_service_t *self, asf_http_req_t *request, asf_http_rep_t *response) /* {{{ */
{
    zend_update_property(ce, self, ZEND_STRL(ASF_SERVICE_PROPERTY_NAME_REQUEST), request);
    zend_update_property(ce, self, ZEND_STRL(ASF_SERVICE_PROPERTY_NAME_RESPONSE), response);
}
/* }}} */

/* {{{ proto object Asf_AbstractService::getRequest(void)
    Return the current request object */
PHP_METHOD(asf_service, getRequest)
{
    ZVAL_COPY(return_value, zend_read_property(asf_service_ce,
                getThis(), ZEND_STRL(ASF_SERVICE_PROPERTY_NAME_REQUEST), 1, NULL));
}
/* }}} */

/* {{{ proto object Asf_AbstractService::getResponse(void)
    Return the current response object */
PHP_METHOD(asf_service, getResponse)
{
    ZVAL_COPY(return_value, zend_read_property(asf_service_ce,
                getThis(), ZEND_STRL(ASF_SERVICE_PROPERTY_NAME_RESPONSE), 1, NULL));
}
/* }}} */

/* {{{ proto object Asf_AbstractService::getFileLogger(string $file_name)
    Return the File Logger object */
PHP_METHOD(asf_service, getFileLogger)
{
    zend_string *file_name = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &file_name) == FAILURE) {
        return;
    }

    if (UNEXPECTED(asf_func_isempty(ZSTR_VAL(file_name)))) {
        php_error_docref(NULL, E_WARNING, "Parameter 'filename' must be a string");
        return;
    }

    (void)asf_logger_instance(return_value, file_name, NULL);
}
/* }}} */

PHP_METHOD(asf_service, __clone) /* {{{ */
{
}
/* }}} */

/* {{{ asf_service_methods[]
*/
zend_function_entry asf_service_methods[] = {
    PHP_ME(asf_service, getRequest,	 NULL, ZEND_ACC_PUBLIC)
    PHP_ME(asf_service, getResponse, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(asf_service, __clone,     NULL, ZEND_ACC_PRIVATE)
    PHP_ME(asf_service, getFileLogger, asf_service_logger_arginfo, ZEND_ACC_PUBLIC)
    PHP_FE_END
};
/* }}} */

ASF_INIT_CLASS(service) /* {{{ */
{
    ASF_REGISTER_CLASS_PARENT(asf_service, Asf_AbstractService, Asf\\AbstractService, ZEND_ACC_EXPLICIT_ABSTRACT_CLASS);

    zend_declare_property_null(asf_service_ce, ZEND_STRL(ASF_SERVICE_PROPERTY_NAME_REQUEST), ZEND_ACC_PROTECTED);
    zend_declare_property_null(asf_service_ce, ZEND_STRL(ASF_SERVICE_PROPERTY_NAME_RESPONSE), ZEND_ACC_PROTECTED);
    zend_declare_property_null(asf_service_ce, ZEND_STRL(ASF_SERVICE_PROPERTY_NAME_LOGGER), ZEND_ACC_PROTECTED);

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
