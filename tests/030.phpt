--TEST--
Check for Asf_Ensure, get started No.2 for build
--SKIPIF--
<?php if (!extension_loaded("asf")) print "skip"; ?>
--INI--
asf.lowcase_path=1
asf.ctype_id=1
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

var_dump(Asf_Ensure::notNull('notnull', Constants::ERR_TEST_CODE));
var_dump(Asf_Ensure::notNull('notnull', Constants::ERR_TEST_CODE));
?>
--EXPECT--
bool(true)
bool(true)

