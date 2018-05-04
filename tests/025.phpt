--TEST--
Check for Asf_Application, and asf.lowcase_path=0 No.1
--SKIPIF--
<?php if (!extension_loaded("asf")) print "skip"; ?>
--INI--
asf.lowcase_path=0
asf.ctype_id=1
asf.action_suffix=1
asf.use_namespace=0
--FILE--
<?php
define('ROOT_PATH', __DIR__);
require ROOT_PATH . "/build.inc";
start();

file_put_contents(APP_PATH . "/services/Index.php", <<<PHP
<?php
class IndexService
{
    public function indexAction()
    {
        return 'hi, Asf';
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
{"errno":0,"errmsg":"","data":"hi, Asf"}

