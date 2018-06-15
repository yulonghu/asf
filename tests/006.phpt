--TEST--
Check for init Asf_Loader::get()/clean()/getInstance()
--SKIPIF--
<?php if (!extension_loaded("asf")) print "skip"; ?>
--INI--
asf.use_namespace=0
--FILE--
<?php
class xxx {
}

Asf_Loader::getInstance('/tmp');

var_dump(Asf_Loader::get('xxx'));

$loader = new Asf_Loader();

try {
    $loader = Asf_Loader::get('xxxx');
} catch (Asf_Exception $e) {
	var_dump($e->getMessage());
}

var_dump(Asf_Loader::clean('xxx'));

try {
    $loader = Asf_Loader::get('');
} catch (Asf_Exception $e) {
	var_dump($e->getMessage());
}
?>
--EXPECTF--
object(xxx)#%d (%d) {
}
string(%d) "%s"
bool(true)
string(%d) "%s"

