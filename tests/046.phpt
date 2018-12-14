--TEST--
Check for Asf_Ensure & Asf_Service
--SKIPIF--
<?php if (!extension_loaded("asf")) print "skip"; ?>
--INI--
asf.lowcase_path=1
asf.ctype_id=0
asf.use_namespace=0
--FILE--
<?php
ini_set('asf.lowcase_path', 1);

define('ROOT_PATH', __DIR__);
require ROOT_PATH . "/build.inc";
start();

file_put_contents(APP_PATH . "/services/index.php", <<<PHP
<?php
class IndexService
{
    public function indexAction()
    {
        var_dump('----first---');
        Asf_Ensure::notNull(true, Constants::ERR_TEST_CODE);
        var_dump('----two---');
    }
}
PHP
);

file_put_contents(CONSTANTS_PATH . "/constants.php", <<<PHP
<?php
class Constants
{
    const ERR_TEST_CODE = 500;

    public static \$ErrDescription = array(
        self::ERR_TEST_CODE => 'This is test default text'
    );
}
PHP
);

$configs = array(
    'asf' => array(
        'root_path'  => MODULE_PATH,
        'log_path'   => LOG_PATH,
        'dispatcher' => array(
            'log' => array(
                'err' => 1
            )
        )
    )
);

$handle = new Asf_Application($configs);
$handle->constants()->run();

shutdown();
?>
--EXPECT--
string(12) "----first---"
string(10) "----two---"

