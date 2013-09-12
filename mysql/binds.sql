SET SQL_MODE="NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";
-- -------------------------------------------------------- 

-- 
-- Table structure for table `binds` 
-- 

CREATE TABLE IF NOT EXISTS `binds` (
  `command` varchar(128) NOT NULL,
  `alias` varchar(128) NOT NULL,
  `who` varchar(32) NOT NULL,
  `access` int(5) NOT NULL DEFAULT '-1',
  PRIMARY KEY (`alias`),
  UNIQUE KEY `alias` (`alias`,`who`) ) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- 
-- Dumping data for table `binds` 
-- 

INSERT INTO `binds` (`command`, `alias`, `who`, `access`) VALUES 
('version', 'versions', 'core', 1000), 
('rehash', 'rehash', 'core', 1000), 
('stop', 'stop', 'core', 1000), 
('restart', 'restart', 'core', 1000), 
('listbinds', 'listbinds', 'core', 1000), 
('listmodules', 'listmodules', 'core', 1000), 
('reloadall', 'reloadall', 'core', 1000), 
('listchannels', 'listchannels', 'core', 1000), 
('listusers', 'listusers', 'core', 1000), 
('listauths', 'listauths', 'core', 1000), 
('reload', 'reload', 'core', 1000), 
('load', 'load', 'core', 1000), 
('unload', 'unload', 'core', 1000), 
('listuserchannels', 'listuserchannels', 'core', 1000), 
('listchannelusers', 'listchannelusers', 'core', 1000), 
('listauthusers', 'listauthusers', 'core', 1000), 
('join', 'join', 'core', 1000), 
('part', 'part', 'core', 1000), 
('raw', 'raw', 'core', 1000), 
('delbind', 'delbind', 'core', 1000), 
('setbind', 'setbind', 'core', 1000),
('setbind', 'addbind', 'core', 1000);
