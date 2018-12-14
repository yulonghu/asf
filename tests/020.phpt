--TEST--
Check for php.ini settings
--SKIPIF--
<?php if (!extension_loaded("asf")) print "skip"; ?>
--INI--
asf.env=develop
asf.ctype_id=1
--FILE--
<?php
var_dump(ini_get('asf.env'));
var_dump(ini_get('asf.ctype_id'));
?>
--EXPECTF--
string(%d) "%s"
string(%d) "%d"

