--TEST--
Check for Asf_Application, and asf.lowcase_path=0 No.2
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

// {"errno":0,"errmsg":"","data":"zhangsan"}

shutdown();
?>
--EXPECTF--
string(%d) "%s"

