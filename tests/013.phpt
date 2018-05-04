--TEST--
Check for Asf_Loader::get()/clean() in Application
--SKIPIF--
<?php if (!extension_loaded("asf")) print "skip"; ?>
--INI--
asf.use_namespace=0
--FILE--
<?php
ini_set('asf.ctype_id', 1);

$configs = array(
    'asf' => array(
        'root_path' => realpath(dirname(__FILE__)),
    )
);

class IndexLogic
{
    public $user;

    public function __construct()
    {/*{{{*/
        $this->user = 'zhangsan';
    }/*}}}*/

    public function getUser()
    {/*{{{*/
        return $this->user;
    }/*}}}*/
}

class IndexService
{
    public function indexAction()
    {/*{{{*/
        try {
            var_dump(Asf_Loader::get(''));
        } catch (Exception $e) {
            var_dump($e->getMessage());
        }

        var_dump(Asf_Loader::get(11));

        $data = Asf_Loader::get('IndexLogic')->getUser();

        var_dump(Asf_Loader::clean('IndexLogic'));
        var_dump(Asf_Loader::clean('IndexLogic'));
        
        var_dump(Asf_Loader::get('IndexLogic'));

        return $data;
    }/*}}}*/
}

$handle = new Asf_Application($configs);
$handle->run();

?>
--EXPECTF--
string(%d) "%s"
bool(false)
bool(true)
bool(false)
bool(false)
{"errno":0,"errmsg":"","data":"zhangsan"}

