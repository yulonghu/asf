--TEST--
Check for Asf_Application configs loading php file
--SKIPIF--
<?php if (!extension_loaded("asf")) print "skip"; ?>
--INI--
asf.lowcase_path=1
asf.ctype_id=1
asf.use_namespace=0
--FILE--
<?php
define('ROOT_PATH', __DIR__);
require ROOT_PATH . "/build.inc";

start();

file_put_contents(APP_PATH . "/services/index.php", <<<PHP
<?php
class IndexService extends Asf_AbstractService
{
    public function indexAction()
    {
    }
}
PHP
);

file_put_contents(CONFIG_PATH . "/config.php", <<<PHP
<?php
return array(
    'asf' => array(
        'root_path'  => MODULE_PATH,
    )
);
PHP
);

$handle = new Asf_Application(CONFIG_PATH . '/config.php');
$handle->run();

echo PHP_EOL;
var_dump($handle->getConfig()->toArray());

shutdown();
?>
--EXPECTF--
{"errno":0,"errmsg":"","data":""}
array(1) {
  ["asf"]=>
  array(1) {
    ["root_path"]=>
    string(%d) "%s"
  }
}

