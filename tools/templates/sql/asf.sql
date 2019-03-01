CREATE DATABASE IF NOT EXISTS `asf`;
USE `asf`;

CREATE TABLE IF NOT EXISTS `test` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `user` varchar(50) DEFAULT '',
  `pass` varchar(50) NOT NULL DEFAULT '',
  `status` smallint(3) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  UNIQUE KEY `user` (`user`)
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;

INSERT INTO `test` (`id`, `user`, `pass`, `status`) VALUES
	(1, 'lisi', '1551085088', 1),
	(2, 'zhangsan', '1510663719', 1),
	(3, 'zhangsan952', '1510838275', 1),
	(4, 'zhangsan58', '1510838276', 0),
	(5, 'zhangsan495', '1510838284', 0),
	(6, 'zhangsan755', '1510838285', 0),
	(7, 'zhangsan515', '1510838294', 0);
