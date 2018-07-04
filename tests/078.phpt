--TEST--
Check for Asf_Http_Cookie
--SKIPIF--
<?php if (!extension_loaded("asf")) print "skip"; ?>
--INI--
asf.use_namespace=0
--FILE--
<?php

//$cookie = new Asf_Http_Cookie(['path' => '/', 'domain' => 'box3.cn', 'expire' => 3600, 'secure' => 1, 'httponly' => 1, 'prefix' => 'Asf_']);
$cookie = new Asf_Http_Cookie(['path' => '/', 'domain' => 'box3.cn', 'expire' => 3600, 'secure' => 1, 'httponly' => 1]);

var_dump($cookie->prefix('Asf_'));

print_r($cookie);

var_dump($cookie->has('a'));
var_dump($cookie->has(123));

var_dump($cookie->get('a'));
var_dump($cookie->get(123));

//var_dump($cookie->set('a', 123456));
//var_dump($cookie->set(123, 'string'));

var_dump($cookie->del('a', 123));
var_dump($cookie->clear());

//var_dump($cookie->forever('a', 123456));
//var_dump($cookie->forever(123, 'string'));

?>
--EXPECTF--
bool(true)
Asf_Http_Cookie Object
(
    [prefix:protected] => Asf_
    [path:protected] => /
    [domain:protected] => box3.cn
    [expire:protected] => 3600
    [secure:protected] => 1
    [httponly:protected] => 1
)
bool(false)
bool(false)
NULL
NULL
bool(false)
bool(false)

