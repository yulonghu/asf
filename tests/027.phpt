--TEST--
Check for Asf_Application, get started No.4 for build
--SKIPIF--
<?php if (!extension_loaded("asf")) print "skip"; ?>
--INI--
asf.ctype_id=0
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
        Asf_Sg::\$inputs['user'] = 'one';
    }

    public function _initAre()
    {
        var_dump(Asf_Sg::\$inputs['user']);
        Asf_Sg::\$inputs['user'] = 'two';
    }

    public function _initYou()
    {
        var_dump(Asf_Sg::\$inputs['user']);
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
$handle->bootstrap()->run();

shutdown();
?>
--EXPECT--
string(3) "one"
string(3) "two"

