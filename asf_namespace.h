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

#ifndef ASF_NAMESPACE_H
#define ASF_NAMESPACE_H

#define ASF_INIT_CLASS_ENTRY(ce, class_name, ns_name, methods) \
    if(ASF_G(use_namespace)) { \
        INIT_CLASS_ENTRY(ce, ns_name, methods); \
    } else { \
        INIT_CLASS_ENTRY(ce, class_name, methods); \
    }

#define ASF_REGISTER_CLASS_PARENT(zclass_entry, class_name, ns_name, flags) \
{ \
    zend_class_entry ce; \
    memset(&ce, 0, sizeof(zend_class_entry)); \
    ASF_INIT_CLASS_ENTRY(ce, #class_name, #ns_name, zclass_entry## _methods); \
    zclass_entry## _ce = zend_register_internal_class_ex(&ce, NULL); \
    zclass_entry## _ce->ce_flags |= flags; \
}

#define ASF_REGISTER_CLASS_PARENT_FINAL_NO_ENTRY(zclass_entry, class_name, ns_name) \
{ \
    zend_class_entry ce; \
    memset(&ce, 0, sizeof(zend_class_entry)); \
    ASF_INIT_CLASS_ENTRY(ce, #class_name, #ns_name, NULL); \
    zclass_entry## _ce = zend_register_internal_class_ex(&ce, NULL); \
    zclass_entry## _ce->ce_flags |= ZEND_ACC_FINAL; \
}

#define ASF_REGISTER_CLASS_INTERNAL(zclass_entry, class_name, ns_name, parent_ce, methods) \
{ \
    zend_class_entry ce; \
    memset(&ce, 0, sizeof(zend_class_entry)); \
    ASF_INIT_CLASS_ENTRY(ce, #class_name, #ns_name, methods); \
    zclass_entry## _ce = zend_register_internal_class_ex(&ce, parent_ce); \
    zclass_entry## _ce->ce_flags |= ZEND_ACC_FINAL; \
}

#define ASF_REGISTER_CLASS_INTERFACE(zclass_entry, class_name, ns_name) \
{ \
    zend_class_entry ce; \
    memset(&ce, 0, sizeof(zend_class_entry)); \
    ASF_INIT_CLASS_ENTRY(ce, #class_name, #ns_name, zclass_entry## _methods); \
    zclass_entry## _ce = zend_register_internal_interface(&ce); \
}

#define ASF_EG_EXCEPTION(errno, errmsg) \
    if (!EG(exception)) { \
        asf_trigger_error(errno, errmsg); \
    } \

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
