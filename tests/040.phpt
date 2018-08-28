--TEST--
Check for Asf_Application, not found IndexLogic
--SKIPIF--
<?php if (!extension_loaded("asf")) print "skip"; ?>
--INI--
asf.ctype_id=0
asf.use_namespace=0
--FILE--
<?php
ini_set('asf.ctype_id', 0);

define('ROOT_PATH', __DIR__);
require ROOT_PATH . "/build.inc";
start();

file_put_contents(APP_PATH . "/services/Index.php", <<<PHP
<?php
class IndexService
{
    public function indexAction()
    {
        try {
            new IndexLogic();
        } catch (Exception \$e) {
            var_dump(\$e->getMessage());
        }
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
--EXPECTF--
string(%d) "%s"

