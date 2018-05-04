--TEST--
Check for Asf_Config::get
--SKIPIF--
<?php if (!extension_loaded("asf")) print "skip"; ?>
--INI--
asf.lowcase_path=1
asf.ctype_id=0
asf.use_namespace=0
--FILE--
<?php
ini_set('asf.ctype_id', 0);
ini_set('asf.lowcase_path', 1);

echo '------uninitialized-------', PHP_EOL;
var_dump(Asf_Config::get());

$configs = array(
    'asf' => array(
        'root_path'  => realpath(dirname(__FILE__)),
    )
);

class IndexService
{
    public function indexAction()
    {
        echo '------long method: get config value-------', PHP_EOL;
        var_dump(Asf_Application::getInstance()->getConfig()->toArray());
        var_dump(Asf_Application::getInstance()->getConfig()->asf->root_path);
        var_dump(Asf_Application::getInstance()->getConfig()->get('asf')->get('root_path'));

        echo '------short method: get config value-------', PHP_EOL;
        var_dump(Asf_Config::get()->toArray());
        var_dump(Asf_Config::get('asf')->root_path);
        var_dump(Asf_Config::get('asf')->get('root_path'));
        var_dump(Asf_Config::get('asf1', "default_value"));
    }
}

$handle = new Asf_Application($configs);
$handle->run();

?>
--EXPECTF--
------uninitialized-------
NULL
------long method: get config value-------
array(1) {
  ["asf"]=>
  array(1) {
    ["root_path"]=>
    string(%d) "%s"
  }
}
string(%d) "%s"
string(%d) "%s"
------short method: get config value-------
array(1) {
  ["asf"]=>
  array(1) {
    ["root_path"]=>
    string(%d) "%s"
  }
}
string(%d) "%s"
string(%d) "%s"
string(%d) "default_value"

