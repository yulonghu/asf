--TEST--
Check for Asf_Request_Interface
--SKIPIF--
<?php if (!extension_loaded("asf")) print "skip"; ?>
--INI--
asf.lowcase_path=0
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
    private \$_req = NULL;

    public function __construct()
    {
    }

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

