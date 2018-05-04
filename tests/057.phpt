--TEST--
Check for Asf_Logger_Adapter_File
--SKIPIF--
<?php if (!extension_loaded("asf")) print "skip"; ?>
--INI--
asf.use_namespace=0
asf.use_lcache=0
--FILE--
<?php
define('ROOT_PATH', __DIR__);
require ROOT_PATH . "/build.inc";
start();

ini_set('asf.use_lcache', 0);

echo '------A-dd_log output-------', PHP_EOL;

$logger_a = new Asf_Log_Adapter_File('A-dd_log', LOG_PATH);

var_dump($logger_a->info('A-dd_log xxxxx'));
var_dump($logger_a->info('A-dd_log ccccc'));

function testfile($logger_a)
{
    $logger_a->info('A-dd_log testfile');
}

testfile($logger_a);

echo '------A-dd_log close-------', PHP_EOL;
var_dump($logger_a->close());

var_dump($logger_a->info('A-dd_log ccccc'));

$logger_a = new Asf_Log_Adapter_File('A-dd_log', LOG_PATH);
var_dump($logger_a->info('A-dd_log xxxxx'));

echo '------B-dd_log output-------', PHP_EOL;

$logger_b = new Asf_Log_Adapter_File('B-dd_log', LOG_PATH);
var_dump($logger_b->info('B-dd_log xxxxx'));
var_dump($logger_a->info('A-dd_log yyyyy'));

var_dump($logger_a->close());
var_dump($logger_b->close());

$logger = new Asf_Log_Adapter_File('biz-dd_log', LOG_PATH);

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

shutdown();

?>
--EXPECT--
------A-dd_log output-------
bool(true)
bool(true)
------A-dd_log close-------
bool(true)
bool(false)
bool(true)
------B-dd_log output-------
bool(true)
bool(true)
bool(true)
bool(true)
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

