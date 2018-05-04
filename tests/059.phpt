--TEST--
Check for Asf_Logger_Adapter_Syslog
--SKIPIF--
<?php if (!extension_loaded("asf")) print "skip"; ?>
--INI--
asf.use_namespace=0
--FILE--
<?php
define('ROOT_PATH', __DIR__);
require ROOT_PATH . "/build.inc";
start();

$logger = new Asf_Log_Adapter_Syslog('biz-dd_log');

echo '------normal output-------', PHP_EOL;
var_dump($logger->info('xxxxx'));

var_dump($logger->info(array('a', 'b', 'c' => 123)));
var_dump($logger->info(json_encode(array('a', 'b', 'c' => 123))));
var_dump($logger->emergency('xxxxx'));
var_dump($logger->alert('xxxxx'));
var_dump($logger->critical('xxxxx'));
var_dump($logger->error('xxxxx'));
var_dump($logger->warning('xxxxx'));
var_dump($logger->notice('xxxxx'));
var_dump($logger->info('xxxxx'));
var_dump($logger->debug('xxxxx'));

echo '------replace output-------', PHP_EOL;
var_dump($logger->info('xxxx{xy}y{yz}zz', array('xy' => 'AA', 'yz' => 'MM')));
var_dump($logger->emergency('xxxx{xy}y{yz}zz', array('xy' => 'AA', 'yz' => 'MM')));
var_dump($logger->alert('xxxx{xy}y{yz}zz', array('xy' => 'AA', 'yz' => 'MM')));
var_dump($logger->critical('xxxx{xy}y{yz}zz', array('xy' => 'AA', 'yz' => 'MM')));
var_dump($logger->error('xxxx{xy}y{yz}zz', array('xy' => 'AA', 'yz' => 'MM')));
var_dump($logger->warning('xxxx{xy}y{yz}zz', array('xy' => 'AA', 'yz' => 'MM')));
var_dump($logger->notice('xxxx{xy}y{yz}zz', array('xy' => 'AA', 'yz' => 'MM')));
var_dump($logger->info('xxxx{xy}y{yz}zz', array('xy' => 'AA', 'yz' => 'MM')));
var_dump($logger->debug('xxxx{xy}y{yz}zz', array('xy' => 'AA', 'yz' => 'MM')));

echo '------log function-------', PHP_EOL;
var_dump($logger->log(1, json_encode(array("aaa", 'bbb'))));
var_dump($logger->log('INFO', json_encode(array("aaa", 'bbb'))));
var_dump($logger->log('INFO', 'xxxx{xy}y{yz}zz', array('xy' => 'AA', 'yz' => 'MM')));

echo '------close log-------', PHP_EOL;
var_dump($logger->close());

shutdown();

?>
--EXPECT--
------normal output-------
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
------replace output-------
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
------log function-------
NULL
bool(true)
bool(true)
------close log-------
bool(true)

