--TEST--
Check for php.ini settings
--SKIPIF--
<?php if (!extension_loaded("asf")) print "skip"; ?>
--FILE--
<?php
var_dump(ini_get('asf.lowcase_path'));
var_dump(ini_get('asf.env'));
var_dump(ini_get('asf.file_suffix'));
var_dump(ini_get('asf.ctype_id'));
var_dump(ini_get('asf.action_suffix'));
?>
--EXPECTF--
string(%d) "%d"
string(%d) "%s"
string(%d) "%d"
string(%d) "%d"
string(%d) "%d"

