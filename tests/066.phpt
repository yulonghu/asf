--TEST--
Check for Application::task(), The module name is admin
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
}
PHP
);

file_put_contents(APP_PATH_ADMIN . "/logics/User.php", <<<PHP
<?php
class UserLogic
{
    public function test()
    {
        return 'This is Task';
    }
}
PHP
);

file_put_contents(APP_PATH . "/logics/Wrong.php", <<<PHP
<?php
class WrongLogic
{
    public function test()
    {
    }
}
PHP
);

file_put_contents(MODULE_PATH . "/Bootstrap.php", <<<PHP
<?php
class Bootstrap
{
    public function _initHow()
    {
    }
}
PHP
);

$configs = array(
    'asf' => array(
        'root_path' => MODULE_PATH,
        'dispatcher' => array(
            'default_module'  => 'admin',
        )
    )
);

$handle = new Asf_Application($configs);
var_dump($handle->Bootstrap()->task());

var_dump(Asf_Loader::get('MainService'));
var_dump(Asf_Loader::get('UserLogic'));
var_dump(Asf_Loader::get('UserLogic')->test());
var_dump($handle->getMode());

/* class 'WrongLogic' not found */
var_dump(Asf_Loader::get('WrongLogic'));

shutdown();

?>
--EXPECTF--
bool(true)
object(MainService)#%d (%d) {
}
object(UserLogic)#%d (%d) {
}
string(%d) "%s"
int(%d)
bool(false)

