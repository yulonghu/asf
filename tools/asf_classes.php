<?php
final class Asf_Application
{/*{{{*/
    protected $config       = NULL;
    protected $dispatcher   = NULL;
    protected $_env         = NULL;
    protected static $_ins  = NULL;

    public function __construct(mixed $config, string $section)
    public function init(mixed $config, string $section)
    public function bootstrap(void)
    public function constants(void)
    public function run(void)
    public function task(void)
    public function getInstance(void)
    public function getConfig(void)
    public function getModules(void)
    public function getRootPath(void)
    public function getEnv(void)
    public function getMode(void)
    public function setLogErrFileName(string $filename)
}
/*}}}*/

final class Asf_Dispatcher
{/*{{{*/
    protected static $_ins  = NULL;
    protected $_request     = NULL;

    public function getInstance(void)
    public function getRouter(void)
    public function getRequest(void)
    public function setDefaultModule(string $name)
    public function setDefaultService(string $name)
    public function setDefaultAction(string $name)
    public function throwException(bool $start)
}
/*}}}*/

final class Asf_Loader
{/*{{{*/
    protected static $_ins = NULL;

    public function get(string $class_name, string $module_name = '')
    public function clean(string $class_name)
}
/*}}}*/

final class Asf_Sg
{/*{{{*/
    protected static $inputs = NULL;
}
/*}}}*/

/* {{{ config */
abstract class Asf_Config_AbstractConfig
{
    protected $_conf = NULL;

    public function get(mixed $name = '', mixed $default = '')
    public function toArray(void)
}

final class Asf_Config_Ini extends Asf_Config_AbstractConfig
{
    public function __construct(string $file_path, string $section = '')
}

final class Asf_Config_Php extends Asf_Config_AbstractConfig
{
    public function __construct(string $file_path)
}

final class Asf_Config_Simple extends Asf_Config_AbstractConfig
{
    public function __construct(array $values)
}

final class Asf_Config implements Countable, Iterator, ArrayAccess
{
    public static function get(string $name = '', mixed $default = '')
    public function getIns(string $adapter, array $options)
}
/* }}} config */

final class Asf_Ensure
{/*{{{*/
    public static function isNull(mixed $data, int $errno)
    public static function notNull(mixed $data, int $errno)
    public static function isEmpty(mixed $data, int $errno)
    public static function NotEmpty(mixed $data, int $errno)
    public static function isFalse(mixed $data, int $errno)
    public static function notFalse(mixed $data, int $errno)
    public static function isTrue(mixed $data, int $errno)
}/*}}}*/

abstract class Asf_Router_AbstractRoute
{/*{{{*/
    public function addRoute(string $name, array $data)
    public function getRoutes(void)
}/*}}}*/

/* {{{ logger */
interface Asf_Log_LoggerInterface
{
    public function emergency(mixed $message, array $content = array())
    public function alert(mixed $message, array $content = array())
    public function critical(mixed $message, array $content = array())
    public function error(mixed $message, array $content = array())
    public function warning(mixed $message, array $content = array())
    public function notice(mixed $message, array $content = array())
    public function info(mixed $message, array $content = array())
    public function debug(mixed $message, array $content = array())
    public function log(int $level = 8, mixed $message, array $content = array())
}

final class Asf_Log_Level
{
    const SPECIAL       = 'special';
    const CUSTOM        = 'custom';
    const DEBUG         = 'debug';
    const INFO          = 'info';
    const NOTICE        = 'notice';
    const WARNING       = 'warning';
    const ERROR         = 'error';
    const CRITICAL      = 'critical';
    const ALERT         = 'alert';
    const EMERGENCY     = 'emergency';
}

abstract class Asf_Log_AbstractLogger implements Asf_Log_LoggerInterface
{
    public function emergency(mixed $message, array $content = array())
    public function alert(mixed $message, array $content = array())
    public function critical(mixed $message, array $content = array())
    public function error(mixed $message, array $content = array())
    public function warning(mixed $message, array $content = array())
    public function notice(mixed $message, array $content = array())
    public function info(mixed $message, array $content = array())
    public function debug(mixed $message, array $content = array())
    public function log(string $level = 'info', mixed $message, array $content = array())
    public function getFormatter(void)
    public function doLog(string $level, int time, string $message)
}

final class Asf_Log_Adapter_File extends Asf_Log_AbstractLogger
{
    public function __construct(string $file_name, string $file_path = '')
    public function close()
    public function getFormatter(void)
    public function doLog(string $level, int time, string $message)
}

final class Asf_Log_Adapter_Syslog extends Asf_Log_AbstractLogger
{
    public function __construct(string $ident, int $options = LOG_PID, int $facility = LOG_LOCAL0)
    public function close(void)
    public function getFormatter(void)
    public function doLog(string $level, int time, string $message)
}

interface Asf_Log_FormatterInterface
{
    public function format(string $level, int $time, string $message)
}

final class Asf_Log_Formatter_File implements Asf_Log_FormatterInterface
{
    protected $_format      = '{date} [{type}] {message}';
    protected $_date_format = 'd-M-Y H:i:s e';

    public function format(string $level, int $time, string $message)
}

final class Asf_Log_Formatter_Syslog implements Asf_Log_FormatterInterface
{
    public function format(string $level, int $time, string $message)
}

final class Asf_Logger extends Asf_Log_AbstractLogger
{
    public static function adapter(int $adapter_id, string $file_name)
}
/* }}} logger */

abstract class Asf_AbstractService
{/*{{{*/
    protected $_req = NULL;
    protected $_res = NULL;

    public function getRequest(void)
    public function getResponse(void)
    public function getFileLogger(string $file_name)
}/*}}}*/

/* {{{ db */
interface Asf_Db_AdapterInterface
{
    public function insert(array $data, bool $retrun_insert_id = false)
    public function insertIgnore(array $data, bool $retrun_insert_id = true)
    public function update(array $data, array $condition = array(), int $limit = 1)
    public function delete(array $condition, int $limit = 1)
    public function findOne(string $sql, array $bind_value = array(), int $mode = PDO::FETCH_ASSOC)
    public function findAll(string $sql, array $bind_value = array(), int $mode = PDO::FETCH_ASSOC)
    public function findOneBy(array $data, array $fields = array(), int $mode = PDO::FETCH_ASSOC)
    public function findAllBy(array $data, array $fields = array(), int $mode = PDO::FETCH_ASSOC)
    public function exeNoquery(string $sql, array $bind_value = array())
    public function exequery(string $sql, array $bind_value = array())
    public function getRowByCmd(string $sql, array $bind_value = array())
    public function getRowsByCmd(string $sql, array $bind_value = array())
    public function addByArray(array $data)
    public function addDupByArray(array $data, array $update_cols)
    public function getCount(string $sql_where = '', array $bind_value = array())
    public function setTable(string $name)
    public function getTable(void)
    public function close(void)
}

abstract class Asf_Db_AbstractAdapter implements Asf_Db_AdapterInterface
{
    protected $_type  = NULL;
    protected $_dbh   = NULL;
    protected $_table = NULL;

    public function __construct(array $configs, bool $reconnection = false);
    public function insert(array $data, bool $retrun_insert_id = true)
    public function insertIgnore(array $data, bool $retrun_insert_id = true)
    public function update(array $data, array $condition = array(), int $limit = 1)
    public function delete(array $condition, int $limit = 1)
    public function findOne(string $sql, array $bind_value = array(), int $mode = PDO::FETCH_ASSOC)
    public function findAll(string $sql, array $bind_value = array(), int $mode = PDO::FETCH_ASSOC)
    public function findOneBy(array $data, array $fields = array(), int $mode = PDO::FETCH_ASSOC)
    public function findAllBy(array $data, array $fields = array(), int $mode = PDO::FETCH_ASSOC)
    public function exeNoquery(string $sql, array $bind_value = array())
    public function exequery(string $sql, array $bind_value = array())
    public function getRowByCmd(string $sql, array $bind_value = array())
    public function getRowsByCmd(string $sql, array $bind_value = array())
    public function addByArray(array $data)
    public function addDupByArray(array $data, array $update_cols)
    public function getCount(string $sql_where = '', array $bind_value = array())
    public function setTable(string $name)
    public function getTable(void)
    public function close(void)
    public function __call(string $function_name, array $args)
}

final class Asf_Db_Adapter_Mysql extends Asf_Db_AbstractAdapter
{
    protected $_type = 'mysql';
}

final class Asf_Db_Adapter_Sqlite extends Asf_Db_AbstractAdapter
{
    protected $_type = 'sqlite';
}

final class Asf_Db_Adapter_Pgsql extends Asf_Db_AbstractAdapter
{
    protected $_type = 'pgsql';
}

final class Asf_Db
{
    public static $_links = NULL;
    public static $_clink  = NULL;

    public static function init(array $configs, int $adapter_id = 0, bool $reset = false)
    public static function initMysql(array $configs, bool $reset = false)
    public static function __callStatic(string $function_name, array $args)
    public static function getLinks(void)
    public static function QBS(void)
    public static function QBI(bool $ignore = false)
    public static function QBU(void)
    public static function QBD(void)
}
/* }}} db */

/* {{{ SQLQueryBuilder */
abstract class Asf_Db_AbstractQueryBuilder
{
    public function where(string $columns, mixed $value, string $operator = '=')
    public function wheres(array $cols(string $key => mixed $value))
    public function orwhere(string $columns, mixed $value, string $operator = '=')
    public function whereIn(string $columns, array $value)
    public function orWhereIn(string $columns, array $value)
    public function table(string $name, string $alias_name = '')
    public function from(string $name, string $alias_name = '')
    public function set(string $name, string $value)
    public function sets(array $cols(string $key => mixed $value))
    public function limit(int $start_page, int $end_page)

    public function cm(string $data)

    public function like(string $columns, mixed $value)
    public function notLike(string $columns, mixed $value)
    public function between(string $columns, mixed $a, mixed $b)
    public function notBetween(string $columns, mixed $a, mixed $b)

    public function show(void)
    public function exec(bool $flags = 0)
    public function clear(void)

    public function getSql(void)
    public function getValues(void)
}

final class Asf_Db_QueryBuilder_Select extends Asf_Db_AbstractQueryBuilder
{
    public function join(string $columns, string $alias_columns = '')
    public function leftJoin(string $columns, string $alias_columns = '')
    public function rightJoin(string $columns, string $alias_columns = '')
    public function innerJoin(string $columns, string $alias_columns = '')

    public function cols(mixed $columns, ...)
    public function having(string $columns, mixed $value, string $operator = '=')
   
    public function max(string $cloumn, string $alias_columns = '')
    public function min(string $cloumn, string $alias_columns = '')
    public function count(string $columns, string $alias_columns = '')
    public function sum(string $columns, string $alias_columns = '')
    public function on(string $cloumn_a, string $cloumn_b)
    public function distinct(string $columns, string $alias_columns = '')

    public function groupBy(string $columns)
    public function orderBy(string $columns, string $operator = 'DESC')
   
    public function union(void)
    public function unionAll(void)
    public function __construct(void)
    /* alias __construct */
    public function select(void)
}

final class Asf_Db_QueryBuilder_Insert extends Asf_Db_AbstractQueryBuilder
{
    public function __construct(bool $ignore = false)
    /* alias __construct */
    public function insert(bool $ignore = false)
    public function onDku(string $columns, mixed $value)
}

final class Asf_Db_QueryBuilder_UpdateQueryBuilder extends Asf_Db_AbstractQueryBuilder
{
    public function __construct(void)
    /* alias __construct */
    public function update(void)
}

final class Asf_Db_QueryBuilder_DeleteQueryBuilder extends Asf_Db_AbstractQueryBuilder
{
    public function __construct(void)
    /* alias __construct */
    public function delete(void)
}
/* }}} SQLQueryBuilder */

/* {{{ http */
interface Asf_Http_ResponseInterface
{
    public function setContentType(string $ctype, string $charset)
    public function redirect(string $url, int $status_code)
    public function setContent(mixed $content)
    public function appendContent(mixed $content)
    public function appendContent(mixed $content)
    public function getContent(void)
    public function send(void)
}

final class Asf_Http_Response implements Asf_Http_ResponseInterface
{
    protected $_header  = NUll;
    protected $_value   = NUll;

    public function __construct(void)
    public function setContentType(string $ctype, string $charset)
    public function redirect(string $url, int $status_code)
    public function setContent(mixed $content)
    public function appendContent(mixed $content)
    public function appendContent(mixed $content)
    public function getContent(void)
    public function send(void)
}

interface Asf_Http_RequestInterface
{
    public function isGet(void)
    public function isPost(void)
    public function isPut(void)
    public function isDelete(void)
    public function isPatch(void)
    public function isHead(void)
    public function isOptions(void)
    public function isCli(void)
    public function isXmlHttpRequest(void)
    public function isTrace(void)
    public function isConnect(void)
    public function isFlashRequest(void)
    public function isPropFind(void)

    public function getParam(string $name, mixed $default)
    public function getParams(void)
    public function getModuleName(void)
    public function getServiceName(void)
    public function getActionName(void)
    public function getMethod(void)
    public function getBaseUri(void)
    public function getRequestUri(void)

    public function getQuery(void)
    public function getPost(void)
    public function getRequest(void)
    public function getFiles(void)
    public function getCookie(void)
    public function getServer(void)
    public function getEnv(void)

    public function hasPost(void)
    public function hasQuery(void)
    public function hasServer(void)
    public function hasFiles(void)
}

final class Asf_Http_Request implements Asf_Http_RequestInterface
{
    protected $method       = NULL;
    protected $uri          = NULL;
    protected $params       = NULL;
    protected $module       = NULL;
    protected $service      = NULL;
    protected $action       = NULL;
    protected $_base_uri    = NULL;

    public function __construct(string $base_uri = '', string $request_uri = '')
    public function isGet(void)
    public function isPost(void)
    public function isPut(void)
    public function isDelete(void)
    public function isPatch(void)
    public function isHead(void)
    public function isOptions(void)
    public function isCli(void)
    public function isXmlHttpRequest(void)
    public function isTrace(void)
    public function isConnect(void)
    public function isFlashRequest(void)
    public function isPropFind(void)

    public function getParam(string $name, mixed $default)
    public function getParams(void)
    public function getModuleName(void)
    public function getServiceName(void)
    public function getActionName(void)
    public function getMethod(void)
    public function getBaseUri(void)
    public function getRequestUri(void)

    public function getQuery(void)
    public function getPost(void)
    public function getRequest(void)
    public function getFiles(void)
    public function getCookie(void)
    public function getServer(void)
    public function getEnv(void)

    public function hasPost(void)
    public function hasQuery(void)
    public function hasServer(void)
    public function hasFiles(void)
}
/* }}} http */

/* {{{ Asf_Util */
class Asf_Util
{
    public static function ip(void)
    public static function bytes2string(int $size, bool $space = 1)
    public static function guid(void)
    public static function atrim(array $data)
    public static function filterXss(array $data)
    public static function cpass(string $data, string $cpass = '')
    public static function iniSets(array $ini)
    public static function isEmail(string $email)
    public static function fileExt(string $filename)
    public static function getUrl(string $url, array $opts = array())
    public static function postUrl(string $url, array $data = array(), array $opts = array())
}
/* }}} Asf_Util */

