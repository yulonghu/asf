--TEST--
Check for Asf_Application, get started No.1 for build
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
class IndexService
{
    public function indexAction()
    {
    }
}
PHP
);

$configs = array(
    'asf' => array(
        'root_path'  => MODULE_PATH,
    )
);

$handle = new Asf_Application($configs);
$handle->run();

shutdown();
?>
--EXPECT--
{"errno":0,"errmsg":"","data":""}

