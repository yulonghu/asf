<?php
define('APP_PATH', dirname(__DIR__));

$app = new Asf_Application(APP_PATH . '/config/config.php');
$app->run();

