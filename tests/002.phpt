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

?>
--EXPECT--
string(7) "A apple"

