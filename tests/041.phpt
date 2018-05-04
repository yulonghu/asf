--TEST--
Check for Asf_Db
--SKIPIF--
<?php if (!extension_loaded("asf")) print "skip"; ?>
--INI--
asf.use_namespace=0
asf.ctype_id=0
--FILE--
<?php
ini_set('asf.ctype_id', 0);

class IndexService
{
    public function indexAction($g_user = 1, $p_pass = 2, $c_cat = 3, $newsid)
    {
        var_dump($g_user, $p_pass, $c_cat, $newsid);
    }
}

$configs = array(
    'asf' => array(
        'root_path'  => __DIR__,
    )
);

$handle = new Asf_Application($configs);
$handle->run();

?>
--EXPECT--
int(1)
int(2)
int(3)
NULL

