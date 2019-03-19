--TEST--
Check for Asf_Sg
--SKIPIF--
<?php if (!extension_loaded("asf")) print "skip"; ?>
--INI--
asf.use_namespace=0
--FILE--
<?php

$name = 'test';
$str = 'A apple';

Asf_Sg::$inputs['what'] = $str;

var_dump(Asf_Sg::$inputs['what']);
var_dump(Asf_Sg::get($name));
var_dump(Asf_Sg::set($name, 'abc123'));
var_dump(Asf_Sg::has($name));
var_dump(Asf_Sg::get($name));
var_dump(Asf_Sg::del($name));
var_dump(Asf_Sg::get($name));
var_dump(Asf_Sg::has($name));

Asf_Sg::$inputs['post']['list']['product'] = 'asf';
var_dump(Asf_Sg::get('post.list.product'));
var_dump(Asf_Sg::get('post.list.product', null));
var_dump(Asf_Sg::has('post.list.product'));
var_dump(Asf_Sg::has('post.list.product'));
var_dump(Asf_Sg::get('post.list.cat', 'dvalue'));

$a = 'hello ';
Asf_Sg::set($name, ' trim_' . $a);
var_dump(Asf_Sg::get($name));
var_dump(Asf_Sg::get($name));

unset($name);

Asf_Sg::$inputs['cat'][0] = 100;
Asf_Sg::$inputs['cat'][1] = 200;
Asf_Sg::$inputs['cat']['title'] = ' title ';
var_dump(Asf_Sg::get('cat.0'));
var_dump(Asf_Sg::get('cat.1'));
var_dump(Asf_Sg::get('cat.title'));

Asf_Sg::$inputs['cat'][2][0][1] = 1000;
Asf_Sg::set('cat.0.1.2', 300);
Asf_Sg::set('cat.user.1.php', 500);
Asf_Sg::set('cat.2.0', 'change_100');
Asf_Sg::set('cat.pass', 'union_'. $a);
Asf_Sg::set('cat.pass.0.1', 654321);
Asf_Sg::set('0', '0000');
Asf_Sg::set(0, 'first');

Asf_Sg::$inputs['cat']['check'] = 1;
Asf_Sg::set('cat.check', 'a') ;

Asf_Sg::del('cat');

?>
--EXPECT--
string(7) "A apple"
NULL
bool(true)
bool(true)
string(6) "abc123"
bool(true)
NULL
bool(false)
string(3) "asf"
string(3) "asf"
bool(true)
bool(true)
string(6) "dvalue"
string(10) "trim_hello"
string(10) "trim_hello"
int(100)
int(200)
string(5) "title"

