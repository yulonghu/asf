--TEST--
Check for Asf_Logger (extends Asf_AbstractService)
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

class IndexService extends Asf_AbstractService
{
    public function indexAction()
    {
        $logger = $this->getFileLogger('biz-dd_log');
        $logger->info('xxxxx');
        $logger->info(111);
        $logger->close();
        $logger = NULL;

        $logger = Asf_Logger::adapter(0, 'biz-dd_log');
        $logger->info('Asf_Logger');
        $logger = NULL;

        $logger = Asf_Logger::adapter(1, 'biz-dd_log');
        $logger->info('Asf_Logger message');
        $logger->info(222);
        $logger = NULL;

        try {
            $logger = new Asf_Logger();
        } catch (Error $e) {
            var_dump($e->getMessage());
        }
    }
}

$configs = array(
    'asf' => array(
        'root_path'  => MODULE_PATH,
        'log_path' => LOG_PATH,
    )
);

$handle = new Asf_Application($configs);
$handle->run();

shutdown();

?>
--EXPECTF--

