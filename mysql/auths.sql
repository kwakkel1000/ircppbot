SET SQL_MODE="NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";
-- -------------------------------------------------------- 

-- 
-- Table structure for table `auths` 
-- 

CREATE TABLE IF NOT EXISTS `auths` (
  `auth` varchar(128) NOT NULL,
  `language` varchar(128) NOT NULL,
  `width` varchar(128) NOT NULL,
  `columns` varchar(128) NOT NULL,
  `botaccess` int(5) NOT NULL,
  `god` int(1) NOT NULL DEFAULT '-1',
  PRIMARY KEY (`auth`)
  ) ENGINE=MyISAM DEFAULT CHARSET=latin1;
