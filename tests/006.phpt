--TEST--
Check for Asf_Loader
--SKIPIF--
<?php if (!extension_loaded("asf")) print "skip"; ?>
--INI--
asf.use_namespace=0
--FILE--
<?php
var_dump(Asf_Loader::getInstance('/tmp'));

var_dump(Asf_Loader::import('Notfound.php'));

try {
    var_dump(Asf_Loader::get('Notfound'));
} catch (Exception $e) {
    var_dump($e->getMessage());
}

try {
    var_dump(Asf_Loader::logic('Notfound'));
} catch (Exception $e) {
    var_dump($e->getMessage());
}

try {
    var_dump(Asf_Loader::dao('notfound'));
} catch (Exception $e) {
    var_dump($e->getMessage());
}

try {
    $error = Asf_Loader::get('');
} catch (Asf_Exception $e) {
	var_dump($e->getMessage());
}

var_dump(Asf_Loader::clean('xxx'));

try {
   new Asf_Loader();
} catch (Error $e) {
   var_dump($e->getMessage());
}

?>
--EXPECTF--
object(Asf_Loader)#%d (%d) {
  ["library":protected]=>
  string(%d) "%s"
}
bool(false)
string(%d) "%s"
string(%d) "%s"
string(%d) "%s"
string(%d) "%s"
bool(false)
string(%d) "%s"

