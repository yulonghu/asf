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

#ifndef ASF_EXCEPTION_H
#define ASF_EXCEPTION_H

#define ASF_ERR_STARTUP_FAILED 		500
#define ASF_ERR_ROUTE_FAILED 		501
#define ASF_ERR_DISPATCH_FAILED 	502
#define ASF_ERR_AUTOLOAD_FAILED 	503
#define ASF_ERR_BOOTSTRAP_FAILED    504

#define ASF_ERR_LOADER_FAILED		505
#define ASF_ERR_LOADER_CLASSNAME	506

#define ASF_ERR_NOTFOUND_MODULE 	507
#define ASF_ERR_NOTFOUND_SERVICE	508
#define ASF_ERR_NOTFOUND_ACTION 	509
#define ASF_ERR_DIS_SERVICE         510
#define ASF_ERR_DIS_ACTION          511

#define ASF_ERR_CONSTANTS_FAILED 	512

#define ASF_ERR_NOTFOUND_CONSTANTS_CLASS 	513
#define ASF_ERR_NOTFOUND_CONSTANTS_TABLE 	514
#define ASF_ERR_NOTFOUND_CONSTANTS_PROP		515

#define ASF_ERR_NOTFOUND_LOGGER_PATH	516
#define ASF_ERR_NOTFOUND_LOGGER_LEVEL	517
#define ASF_ERR_LOGGER_STREAM_ERROR		518
#define ASF_ERR_LOGGER_FILENAME		    519

#define ASF_ERR_DB_OPTIONS_DSN			520
#define ASF_ERR_DB_OPTIONS_USERNAME		521
#define ASF_ERR_DB_PDO_CE				522
//#define ASF_ERR_DB_BINDVALUE			519
#define ASF_ERR_DB_PREPARE				523
#define ASF_ERR_DB_EXECUTE				524
#define ASF_ERR_DB_TABLE				525
#define ASF_ERR_DB_SET_COLUMN			526
#define ASF_ERR_DB_PARAM				527

extern zend_class_entry *asf_exception_ce;

void asf_trigger_error(int code, char *format, ...);

ASF_INIT_CLASS(exception);

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
