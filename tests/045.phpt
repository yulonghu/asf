--TEST--
Check for Config autoloading library
--SKIPIF--
<?php if (!extension_loaded("asf")) print "skip"; ?>
--INI--
asf.lowcase_path=1
asf.ctype_id=0
asf.action_suffix=1
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
        \$obj = new IndexLogic();
        var_dump(\$obj->getUser());

        \$lib = new Lib();
        var_dump(\$lib->getUser());

        var_dump(Asf_Loader::get('Lib')->getUser());
        var_dump(Asf_Loader::get('IndexLogic')->getUser());

        var_dump(Asf_Loader::get('Lib')->getUser());
        var_dump(Asf_Loader::get('Libdd'));
    }
}
PHP
);

file_put_contents(APP_PATH . "/logics/index.php", <<<PHP
<?php
class IndexLogic
{
    public function getUser()
    {
        return 'logic_user';
    }
}
PHP
);

file_put_contents(LIBRARY_PATH . "/lib.php", <<<PHP
<?php
class Lib
{
    public function getUser()
    {
        return 'lib_user';
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
string(10) "logic_user"
string(8) "lib_user"
string(8) "lib_user"
string(10) "logic_user"
string(8) "lib_user"
bool(false)

