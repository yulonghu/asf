<?php
class Constants
{
    const START = 'Hello World';
    const MASTER = 'master';
    const SLAVE = 'slave';

    const ERR_TEST_USER = 10000;
    const ERR_TEST_PASS = 10001;

    public static $ErrDescription = [
        self::ERR_TEST_USER => 'This is Test User',
        self::ERR_TEST_PASS => 'This is Test Pass',
    ];
}

