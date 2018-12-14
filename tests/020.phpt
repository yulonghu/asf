--TEST--
Check for php.ini settings
--SKIPIF--
<?php if (!extension_loaded("asf")) print "skip"; ?>
--INI--
asf.lowcase_path=0
asf.env=develop
asf.ctype_id=1
--FILE--
<?php
var_dump(ini_get('asf.lowcase_path'));
var_dump(ini_get('asf.env'));
var_dump(ini_get('asf.ctype_id'));
?>
--EXPECTF--
string(%d) "%d"
string(%d) "%s"
string(%d) "%d"

