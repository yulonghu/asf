<?php
use Asf\Application;

define('APP_PATH', dirname(__DIR__));

$app = new Application(APP_PATH . '/config/config.php');
$app->Constants()->Bootstrap()->run();

