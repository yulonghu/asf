--TEST--
Check for Asf_Http_Response::redirect
--SKIPIF--
<?php if (!extension_loaded("asf")) print "skip"; ?>
--INI--
asf.ctype_id=1
asf.use_namespace=0
--FILE--
<?php
ini_set('asf.ctype_id', 1);

$rep = new Asf_Http_Response();
var_dump($rep->redirect("http://www.box3.cn", 302));

?>
--EXPECT--
bool(false)

