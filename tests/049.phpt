--TEST--
Check for Asf_Application No.2
--SKIPIF--
<?php if (!extension_loaded("asf")) print "skip"; ?>
--INI--
asf.ctype_id=1
asf.use_namespace=0
--FILE--
<?php
define('ROOT_PATH', __DIR__);
require ROOT_PATH . "/build.inc";
start();

file_put_contents(APP_PATH . "/services/Userlog.php", <<<PHP
<?php
class UserlogService extends Asf_AbstractService
{
    public function listAction()
    {
        return 'zhangsan';
    }
}
PHP
);

$configs = array(
    'asf' => array(
        'root_path'  => MODULE_PATH,
        'dispatcher' => array(
            'default_service' => 'Userlog',
            'default_action'  => 'list',
        )
    )
);

try {
    $handle = new Asf_Application($configs);
    $handle->run();
} catch (Exception $e) {
    var_dump($e->getMessage());
}

shutdown();
?>
--EXPECTF--
{"errno":0,"errmsg":"","data":"zhangsan"}

