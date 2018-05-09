# Asf - API Services Framework
[![Build Status](https://travis-ci.org/yulonghu/asf.svg?branch=master)](https://travis-ci.org/yulonghu/asf)

This is a PHP framework written in C. It is Simple, Fast, Standards, Security.

## Install
### Requirements
- PHP 7.0 +
- GCC 4.4.0+ (Recommended GCC 4.8+)

### DownLoad
```
git clone https://github.com/yulonghu/asf.git
```

### Compile for Linux/Unix/Mac
```
$ /path/to/phpize
$ ./configure --with-php-config=/path/to/php-config
$ make && make install
```
### Documentation

http://www.box3.cn/phpasf/index.html

## Get Started

### Use tools to create a new project
```
/php-bin-path/php/ /tools/asf_project.php /to-path/project_name
```
#### Layout
```
+ public
  | - index.php
+ config
  | - config.php
+ library
+ modules
    | - Bootstrap.php
    | - Constants.php
  + api
    |+ services
	   |- Index.php  // Default service
    |+ logics
    |+ daos
```

### config/config.php
```php
<?php
$configs = array(
    'asf' => array(
        'root_path' => realpath(dirname(__FILE__)),
    )
);

return $configs;
```

### public/index.php
```php
<?php
define('APP_PATH', dirname(__DIR__));

$app = new Asf_Application(APP_PATH . '/config/config.php');
$app->run();
```

### Default service
```php
<?php
class IndexService
{
    public function indexAction()
    {
        return 'Hello World';
    }
}
```

### Run in Nginx/Apache/Lighttpd
http://www.your-domain.com

#### Output results
```
{
    "errno": 0,
    "data": "Hello World"
}
```

## License
Asf is open source software under the [PHP License v3.01](http://www.php.net/license/3_01.txt)
