SET SQL_MODE="NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";
-- -------------------------------------------------------- 

-- 
-- Table structure for table `reply` 
-- 

CREATE TABLE IF NOT EXISTS `reply` (
  `reply_name` varchar(128) NOT NULL,
  `reply` varchar(128) NOT NULL,
  `language` varchar(128) NOT NULL,
  PRIMARY KEY (`reply_name`, `language`)
  ) ENGINE=MyISAM DEFAULT CHARSET=latin1;
