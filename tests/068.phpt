--TEST--
Check for Asf_Logger_Adapter_File use buffer
--SKIPIF--
<?php if (!extension_loaded("asf")) print "skip"; ?>
--INI--
asf.use_namespace=0
asf.use_lcache=1
--FILE--
<?php
define('ROOT_PATH', __DIR__);
require ROOT_PATH . "/build.inc";
start();

ini_set('asf.use_lcache', 1);
ini_set('asf.log_buffer_size', 3);

$logger = new Asf_Log_Adapter_File('A-dd_log', LOG_PATH);

$i = 0;
while ($i < 10) {
    var_dump($logger->info('A-dd_log xxxxx' . time()));
    $i++;
}

var_dump($logger->close());

shutdown();

?>
--EXPECT--
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
bool(false)

