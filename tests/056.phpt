--TEST--
Check for IndexService(new Asf_Log_Adapter_File)
--SKIPIF--
<?php if (!extension_loaded("asf")) print "skip"; ?>
--INI--
asf.ctype_id=0
asf.action_suffix=1
asf.use_namespace=0
--FILE--
<?php
ini_set('asf.ctype_id', 0);

define('ROOT_PATH', __DIR__);
require ROOT_PATH . "/build.inc";
start();

class IndexService
{
    public function indexAction()
    {
        $logger = new Asf_Log_Adapter_File('biz-dd_log');
        $logger->info(json_encode(array('1', 'a', 'm2' => md5(time()))));
        try {
            $logger->infos(json_encode(array('1', 'a', 'm2' => md5(time()))));
        } catch (Error $e) {
            var_dump($e->getMessage());
        }
    }
}

$configs = array(
    'asf' => array(
        'root_path'  => MODULE_PATH,
        'log_path'   => LOG_PATH,
    )
);

$handle = new Asf_Application($configs);
$handle->run();

shutdown();

?>
--EXPECTF--
string(%d) "%s"

