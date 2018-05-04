--TEST--
Check for Application::run(), The module name is admin
--SKIPIF--
<?php if (!extension_loaded("asf")) print "skip"; ?>
--INI--
asf.use_namespace=0
asf.ctype_id=0
--FILE--
<?php
define('ROOT_PATH', __DIR__);
require ROOT_PATH . "/build.inc";
start();

file_put_contents(APP_PATH_ADMIN . "/services/Main.php", <<<PHP
<?php
class MainService
{
    public function userAction()
    {
        var_dump(Asf_Loader::get('UserLogic')->test());
    }
}
PHP
);

file_put_contents(APP_PATH_ADMIN . "/logics/User.php", <<<PHP
<?php
class UserLogic
{
    public function test()
    {
        return Asf_Loader::get('UserDao')->test();
    }
}
PHP
);

file_put_contents(APP_PATH_ADMIN . "/daos/User.php", <<<PHP
<?php
class UserDao
{
    public function test()
    {
        return 'This is Test';
    }
}
PHP
);

$configs = array(
    'asf' => array(
        'root_path' => MODULE_PATH,
        'dispatcher' => array(
            'default_module'  => 'admin',
            'default_service' => 'main',
            'default_action'  => 'user',
        )
    )
);

$handle = new Asf_Application($configs);
$handle->run();

shutdown();

?>
--EXPECTF--
string(%d) "%s"

