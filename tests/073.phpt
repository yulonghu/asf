--TEST--
Check for SQL Query Builder for Asf_Db_QueryBuilder_Delete
--SKIPIF--
<?php if (!extension_loaded("asf")) print "skip"; ?>
--INI--
asf.use_namespace=0
asf.ctype_id=0
--FILE--
<?php
$delete = new Asf_Db_QueryBuilder_Delete();

$delete
->from('test')
->where('id', 100);

var_dump($delete->show());
$delete->clear();

$delete = Asf_Db::qbd();
var_dump($delete->from('test')->where('user', 'lisi2')->like('pass', '%11111%')->show());

var_dump($delete->getSql());
var_dump(serialize($delete->getValues()));

$delete->clear();

var_dump($delete->delete()->show());

?>
--EXPECTF--
string(%d) "%s"
string(%d) "%s"
string(%d) "%s"
string(%d) "%s"
string(%d) "%s"

