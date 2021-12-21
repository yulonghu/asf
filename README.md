# Asf - API Services Framework
[![Build Status](https://travis-ci.org/yulonghu/asf.svg?branch=master)](https://travis-ci.org/yulonghu/asf)

Simple, High performance PHP framework written in C. 

## Introduction
API Services Framework (or simply Asf). It's has many micro-innovation and is a new generation of lightweight MVC framework.

## Features
- The class provided by the framework, loaded when the PHP process starts, and resident in memory.
- Errno = 999, PHP fatal error interception.
- Strong early warning mechanism (DB, Cache, Script, PHP Error).
- Support for Logger Buffer Cache
- Support for GPC methods, making it easier to use PHP global variables ($_GET, $_POST, $_COOKIE)
- Support for configuration file resident memory
- Support for faster, easier-to-operate cookie management

## Install
### Requirements
- PHP 7.0, PHP 7.1, PHP 7.2, PHP 7.3, PHP 7.4
- GCC 4.4.0+ (Recommended GCC 4.8+)

### DownLoad
```
git clone https://github.com/yulonghu/asf.git
```

### Compile for Linux/Unix/Mac

Provide two installation methods.
Select No.1, Use the following methods to install the extension:

```bash
cd asf/travis
sudo ./install -i /path/to/phpize -c /path/to/php-config
```

Select No.2, PHP extension standard installation:

```bash
cd asf/asf
/path/to/phpize
./configure --with-php-config=/path/to/php-config
make && make install
```

#### Add the extension to your php.ini
```ini
extension=asf.so
```

Restart the web server

### Documentation

http://www.box3.cn/phpasf/index.html

## Get Started

### Use tools to create a new project
```
/php-bin-path/php asf-src/tools/asf_project.php /to-path/project_name
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
        'root_path' => APP_PATH . '/modules',
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
