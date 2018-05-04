# Asf - API Services Framework  

This is a PHP framework written in C. It is simple, fast, standards, security.

## Install

### Requirements
- PHP 7.0 +
- GCC 4.4.0+ (Recommended GCC 4.8+)

### DownLoad

```
git clone git://github.com/yulonghu/asf.git
```

### Linux/Unix/Mac

```
$/path/to/phpize
$./configure --with-php-config=/path/to/php-config
$make && make install
```
## Documentation

### Need Help
http://www.box3.cn/phpasf/index.html

### Start with the Hello Wrold

```php
<?php

use Asf\AbstractService as Service;

class IndexService extends Service
{
    public function indexAction()
    {
        return 'Hello World';
    }
}
```

## License

Asf is open source software under the [PHP License v3.01](http://www.php.net/license/3_01.txt). 

