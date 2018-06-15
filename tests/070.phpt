--TEST--
Check for SQL Query Builder for Asf_Db_QueryBuilder_Select
--SKIPIF--
<?php if (!extension_loaded("asf")) print "skip"; ?>
--INI--
asf.use_namespace=0
asf.ctype_id=0
--FILE--
<?php
$select = new Asf_Db_QueryBuilder_Select();

$select
->cols()
->from('test')
->Where('', '');
var_dump($select->show());
$select->clear();

$select
->select()
->cols()
->from('test')
->OrWhere(0, 1)
->OrWhere('user', 1);
var_dump($select->show());
$select->clear();

$select->cm('select * from test where cm = 1');
var_dump($select->show());
$select->clear();

$select
->select()
->cols()
->from('test')
->where('user', 'zhangsan');
var_dump($select->show());
$select->clear();

$select
->select()
->cols()
->from('test')
->where('where_user', 'zhangsan')
->where(0, 1)
->orWhere('orWhere_user', 'zhangsan')
->OrWhere(0, 1)
->OrWhere(0, 1)
->whereIn('WhereInstatus', array('100', -1))
->orWhereIn('OrWhereInstatus', array('100', -1))
->having('id', 1, '>')
->limit(10, 20);

var_dump($select->show());
$select->clear();

$select
->select()
->from('test')
->whereIn('pid', array('100', 200))
->orWhereIn('cid', array('10', 20));

var_dump($select->show());
$select->clear();

$select
->select()
->cols()
->from('test', 'a')
->leftJoin('image', 'b')
->InnerJoin('image', 'b')
->on('a.id', 'b.id')
->groupBy('a.id')
->orderBy('a.id', 'ASC');

var_dump($select->show());
$select->clear();

$select
->select()
->cols()
->from('test', 'a')
->rightJoin('image', 'b')
->on('a.id', 'b.id')
->having('b.id', 100, '>');

var_dump($select->show());
$select->clear();

$select
->select()
->max('username')
->cm(',')
->min('username')
->cm(',')
->sum('username')
->cm(',')
->count('username')
->cm(',')
->distinct('username')
->cm(',')
->from('test', 'a');

var_dump($select->show());
$select->clear();

$select
->cm('select * from test')
->unionAll()
->cm('select * from test1');

var_dump($select->show());
$select->clear();

$select
->select()
->cols(array(
    'b.*',
    'd.factory',
    'd.model',
    'd.version',
    'd.kver_mtime',
    'd.kver_id'
))
->from('bd', 'b')
->join('device', 'd')
->where('b.kbid', 'd.kbid')
->where('b.status', 0, '!=')
->limit(0, 10);

var_dump($select->show());
$select->clear();

$select
->select()
->cols()
->from('test', 'a');

var_dump($select->show());
$select->clear();
$select = NULL;

$select = Asf_Db::qbs();
$select->cols()->from('test1')->where('ids', 100);

var_dump($select->getSql());
//var_dump($select->getValues());

var_dump($select->show());
$select->clear();

$select = Asf_Db::qbs();
$select->cols()->from('test1')->like('user', '%sanmao%')->notLike('pass', '%123%');

var_dump($select->show());
$select->clear();

$select = Asf_Db::qbs();
$select->cols()->from('test1')->between('status', 0, 2)->notBetween('status', 1, 3);

var_dump($select->show());
$select->clear();

$select = Asf_Db::qbs();
var_dump($select->cols()->from('test')->limit(0, 10)->show());

$select = Asf_Db::qbs();
$select->cols()->from('test1')->where('id', '100', '>')->where('status', 2)
->unionAll()
->select()
->cols()->from('test1')->where('id', '50', '<');

var_dump($select->show());
$select->clear();

$select = Asf_Db::qbs();
$select->distinct('model')->cols(',id as ids')->from('test1');
var_dump($select->show());
$select->clear();

$select = Asf_Db::qbs();
$select->count('*', 'cnt')->from('test1')->where('s', 0);
var_dump($select->show());
$select->clear();

$select = Asf_Db::qbs();
$select->cm('a, b, c, d')->from('test')->where('k', 1);
var_dump($select->show());
$select->clear();

$select = Asf_Db::qbs();
$select->cols()->from('test')->where('k', 1)->orderBy('id');
var_dump($select->show());
$select->clear();

$select = Asf_Db::qbs();
$select->cols()->from('test')->wheres(array('d' => 1, 'g' => 'hhh'))->orderBy('id');
var_dump($select->show());
$select->clear();

$select = Asf_Db::qbs();
$select->cols(array('a', ' b', ' c', ' d '))->from('test')->wheres(array('e ' => 1, ' f' => 'hhh'));
var_dump($select->show());
$select->clear();

$select = Asf_Db::qbs();
$select->cols('a, b, c, d')->from('test')->like('k', '%1%');
var_dump($select->show());
$select->clear();

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
string(%d) "%s"
string(%d) "%s"
string(%d) "%s"
string(%d) "%s"
string(%d) "%s"
string(%d) "%s"
string(%d) "%s"
string(%d) "%s"
string(%d) "%s"
string(%d) "%s"
string(%d) "%s"
string(%d) "%s"
string(%d) "%s"
string(%d) "%s"
string(%d) "%s"
string(%d) "%s"

