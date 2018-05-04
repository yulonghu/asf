--TEST--
Check for Asf_Ensure, get started No.1 for build
--SKIPIF--
<?php if (!extension_loaded("asf")) print "skip"; ?>
--INI--
asf.lowcase_path=0
asf.ctype_id=1
asf.action_suffix=1
asf.use_namespace=0
--FILE--
<?php
class Constants
{
    const ERR_TEST_CODE = 500;
    const ERR_USER_CODE = 501;
    const ERR_USER_STR  = 'aaa';

    public static $ErrDescription = array(
        self::ERR_TEST_CODE => 'This is test default text',
        self::ERR_USER_CODE => 'This is test user text',
        self::ERR_USER_STR  => 'This is test string',
    );
}

try {
    new Asf_Ensure();
} catch (Error $e) {
    var_dump($e->getMessage());
}

Asf_Ensure::notNull(null, Constants::ERR_TEST_CODE);
?>
--EXPECTF--
{"errno":500,"errmsg":"This is test default text","data":""}

