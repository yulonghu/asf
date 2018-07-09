--TEST--
Check for Asf_Sg
--SKIPIF--
<?php if (!extension_loaded("asf")) print "skip"; ?>
--INI--
asf.use_namespace=0
--FILE--
<?php
$str = 'A apple';

Asf_Sg::$inputs['what'] = $str;

var_dump(Asf_Sg::$inputs['what']);

$name = 'test';

var_dump(Asf_Sg::get($name));

var_dump(Asf_Sg::set($name, 'abc123'));

var_dump(Asf_Sg::get($name));

var_dump(Asf_Sg::del($name));

var_dump(Asf_Sg::get($name));

unset($name);

?>
--EXPECT--
string(7) "A apple"
NULL
bool(true)
string(6) "abc123"
bool(true)
NULL

