--TEST--
Check for SQL Query Builder for Asf_Db_QueryBuilder_Update
--SKIPIF--
<?php if (!extension_loaded("asf")) print "skip"; ?>
--INI--
asf.use_namespace=0
asf.ctype_id=0
--FILE--
<?php
$update = new Asf_Db_QueryBuilder_Update();

$update
->table('test')
->set('user', 'lisi1')
->set('pass', '123456')
->where('id', 100);

var_dump($update->show());
$update->clear();

$update = Asf_Db::qbu();
var_dump($update->table('test')->set('user', 'lisi2')->like('pass', '%11111%')->show());

var_dump($update->getSql());
var_dump(serialize($update->getValues()));
$update->clear();

var_dump($update->update()->show());

$update = Asf_Db::qbu();
$update->table(' test')->set(' user', 'lisi2')->like('pass', '%11111%')->show();
var_dump($update->update()->show());
$update->clear();


?>
--EXPECTF--
string(%d) "%s"
string(%d) "%s"
string(%d) "%s"
string(%d) "%s"
string(%d) "%s"
string(%d) "%s"

