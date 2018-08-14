--TEST--
Check for Asf_Ensure others function
--SKIPIF--
<?php if (!extension_loaded("asf")) print "skip"; ?>
--INI--
asf.use_namespace=0
--FILE--
<?php
class Constants
{
    const ERR_TEST_CODE = 500;

    public static $ErrDescription = array(
        self::ERR_TEST_CODE => 'This is test default text',
    );
}

var_dump(Asf_Ensure::isNull(null, Constants::ERR_TEST_CODE));
var_dump(Asf_Ensure::isEmpty('', Constants::ERR_TEST_CODE));
var_dump(Asf_Ensure::isFalse(false, Constants::ERR_TEST_CODE));
var_dump(Asf_Ensure::isTrue(true, Constants::ERR_TEST_CODE));

var_dump(Asf_Ensure::notEmpty('xxx', Constants::ERR_TEST_CODE));
var_dump(Asf_Ensure::notFalse('xxx', Constants::ERR_TEST_CODE));
var_dump(Asf_Ensure::notNull('xxx', Constants::ERR_TEST_CODE));
Asf_Ensure::out(110, 'xxx');
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

