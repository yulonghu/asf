--TEST--
Check for SQL Query Builder for Asf_Db_QueryBuilder_Insert
--SKIPIF--
<?php if (!extension_loaded("asf")) print "skip"; ?>
--INI--
asf.use_namespace=0
asf.ctype_id=0
--FILE--
<?php
$insert = new Asf_Db_QueryBuilder_Insert();

$insert
->table('test')
->set('user', 'lisi1')
->set('pass', '123456');

var_dump($insert->show());
$insert->clear();

$insert = Asf_Db::qbi();
var_dump($insert->table('test')->set('user', 'lisi2')->show());

var_dump($insert->getSql());
var_dump(serialize($insert->getValues()));
$insert->clear();

$insert = Asf_Db::qbi();
var_dump($insert->table('test')->set('user', 'lisi2')->set('pass', 1234)->where('user', 'lisi', '!=')->show());
$insert->clear();

$insert = Asf_Db::qbi();
var_dump($insert->table('test')->set('user', 'user+1')->set('pass', 123456)->show());
$insert->clear();

$insert = Asf_Db::qbi(1);
var_dump($insert->table('test')->set('user', 'user+1')->set('pass', 123456)->ondku('status', 1)->show());
$insert->clear();

$insert = Asf_Db::qbi();
var_dump($insert->table('test')->sets(array('user' => 'zs', 'pass' => 123456))->where('user', 'lisi', '!=')->show());
$insert->clear();

var_dump($insert->insert()->show());

?>
--EXPECTF--
string(%d) "%s"
string(%d) "%s"
string(%d) "%s"
string(%d) "%s"
string(%d) "%s"
string(%d) "%s"
string(%d) "%s"
string(%d) "%s"
string(%d) "%s"

