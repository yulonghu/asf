--TEST--
Check for Asf_Http_Response
--SKIPIF--
<?php if (!extension_loaded("asf")) print "skip"; ?>
--INI--
asf.ctype_id=1
asf.use_namespace=0
--FILE--
<?php
ini_set('asf.ctype_id', 1);

$rep = new Asf_Http_Response();
var_dump($rep->setContentType("text/plain", "UTF-8"));
var_dump($rep->setContent("Hello"));

$rep->send(); echo PHP_EOL;

var_dump($rep->appendContent(" World"));
$rep->send(); echo PHP_EOL;

var_dump($rep->appendContent(" , Come on"));
var_dump($rep->getContent());

$rep->send();
?>
--EXPECT--
bool(true)
bool(true)
{"errno":0,"errmsg":"","data":"Hello"}
bool(true)
{"errno":0,"errmsg":"","data":"Hello World"}
bool(true)
string(21) "Hello World , Come on"
{"errno":0,"errmsg":"","data":"Hello World , Come on"}

