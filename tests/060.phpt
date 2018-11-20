--TEST--
Check for Asf_Logger_Formatter_File
--SKIPIF--
<?php if (!extension_loaded("asf")) print "skip"; ?>
--INI--
asf.use_namespace=0
--FILE--
<?php
/*
$formatter = new Asf_Log_Formatter_File();

$data = $formatter->format('info', time(), 'This is test');
$data = trim($data);
var_dump($data);
*/
?>
--EXPECTF--
