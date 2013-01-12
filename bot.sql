SET SQL_MODE="NO_AUTO_VALUE_ON_ZERO";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;

--
-- Database: `centravi_trantest`
--

-- --------------------------------------------------------

--
-- Table structure for table `auth`
--

CREATE TABLE IF NOT EXISTS `auth` (
  `id` int(11) NOT NULL auto_increment,
  `UserUuid` varchar(64) NOT NULL,
  `auth` varchar(512) NOT NULL,
  `botaccess` int(11) NOT NULL default '-1',
  `god` varchar(2) NOT NULL default '-1',
  `language` varchar(10) NOT NULL default 'en',
  PRIMARY KEY  (`id`,`auth`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;

-- --------------------------------------------------------

--
-- Table structure for table `ChannelBotCommands`
--

CREATE TABLE IF NOT EXISTS `ChannelBotCommands` (
  `id` int(2) NOT NULL auto_increment,
  `command` varchar(128) NOT NULL,
  `bind` varchar(128) NOT NULL,
  `access` int(5) NOT NULL default '-1',
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=36 ;

--
-- Dumping data for table `ChannelBotCommands`
--

INSERT INTO `ChannelBotCommands` (`id`, `command`, `bind`, `access`) VALUES
(1, 'reloadchan', 'reloadchan', -1),
(30, 'channelcommands', 'commands', 0),
(4, 'adduser', 'adduser', -1),
(5, 'adduser', 'add', -1),
(6, 'users', 'users', -1),
(7, 'deluser', 'deluser', -1),
(8, 'deluser', 'del', -1),
(9, 'changelevel', 'changelevel', -1),
(10, 'changelevel', 'clvl', -1),
(11, 'op', 'op', -1),
(12, 'deop', 'deop', -1),
(13, 'voice', 'voice', -1),
(14, 'devoice', 'devoice', -1),
(16, 'auth', 'auth', -1),
(17, 'ping', 'ping', -1),
(18, 'up', 'up', -1),
(19, 'down', 'down', -1),
(20, 'resync', 'resync', -1),
(21, 'resync', 'sync', -1),
(22, 'version', 'version', -1),
(23, 'access', 'access', -1),
(24, 'access', 'a', -1),
(25, 'myaccess', 'myaccess', -1),
(26, 'myaccess', 'ma', -1),
(27, 'kickuser', 'kick', -1),
(28, 'kickuser', 'k', -1),
(31, 'kickban', 'kb', 200),
(32, 'ping', 'victor', 0),
(33, 'uptime', 'uptime', -1),
(34, 'readchannel', 'readchannel', 400),
(35, 'stats', 'stats', -1);

-- --------------------------------------------------------

--
-- Table structure for table `channels`
--

CREATE TABLE IF NOT EXISTS `channels` (
  `id` int(11) NOT NULL auto_increment,
  `ChannelUuid` varchar(64) NOT NULL,
  `channel` varchar(512) NOT NULL,
  `giveops` int(11) NOT NULL default '300',
  `givevoice` int(11) NOT NULL default '100',
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=45 ;

-- --------------------------------------------------------

--
-- Table structure for table `OCommandsCommands`
--

CREATE TABLE IF NOT EXISTS `OCommandsCommands` (
  `id` int(2) NOT NULL auto_increment,
  `command` varchar(128) NOT NULL,
  `bind` varchar(128) NOT NULL,
  `access` int(5) NOT NULL default '1001',
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=23 ;

--
-- Dumping data for table `OCommandsCommands`
--

INSERT INTO `OCommandsCommands` (`id`, `command`, `bind`, `access`) VALUES
(3, 'ousers', 'ousers', 600),
(4, 'changeolevel', 'changeolevel', 1000),
(5, 'changeolevel', 'oaccess', 1000),
(6, 'say', 'say', 300),
(7, 'raw', 'raw', 1000),
(8, 'god', 'god', 200),
(9, 'god', 'devil', 200),
(10, 'addobind', 'addobind', 1000),
(13, 'addbind', 'addbind', 1000),
(12, 'delobind', 'delobind', 1000),
(14, 'delbind', 'delbind', 1000),
(15, 'addchannel', 'addchannel', 300),
(16, 'delchannel', 'delchannel', 300),
(17, 'ocommandscommands', 'commands', 1),
(18, 'support', 'support', 0),
(19, 'action', 'action', 300),
(20, 'action', 'emote', 300),
(21, 'broadcast', 'broadcast', 999),
(22, 'listchannels', 'listchannels', 200);

-- --------------------------------------------------------

--
-- Table structure for table `reply`
--

CREATE TABLE IF NOT EXISTS `reply` (
  `id` int(11) NOT NULL auto_increment,
  `reply_name` varchar(256) NOT NULL,
  `reply` varchar(512) NOT NULL,
  `language` varchar(5) NOT NULL,
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=21 ;

--
-- Dumping data for table `reply`
--

INSERT INTO `reply` (`id`, `reply_name`, `reply`, `language`) VALUES
(1, 'need_oaccess', 'you need oaccess to execute this command', 'en'),
(2, 'need_god', 'you need to enable god mode to execute this command', 'en'),
(3, 'changeolevel', 'user $nick$[$auth$] changed oaccess to $access$ oaccess', 'en'),
(4, 'delbind', 'deleted $bind$ alias for $command$', 'en'),
(5, 'delbind_nobind', 'there is no $bind$ alias for $command$. not deleted', 'en'),
(6, 'delobind', 'deleted $bind$ alias for $command$', 'en'),
(7, 'addbind', 'added $bind$ alias for $command$', 'en'),
(8, 'addobind', 'added $bind$ alias for $command$', 'en'),
(9, 'addbind_exists', 'the binding $bind$ for $command$ already exists', 'en'),
(10, 'addobind_exists', 'the binding $bind$ for $command$ already exists', 'en'),
(11, 'delobind_nobind', 'there is no $bind$ alias for $command$. not deleted', 'en'),
(12, 'addchannel', '$channel$ registred by $nick$[$auth$] owner $regnick$[$regauth$]', 'en'),
(13, 'delchannel', '$channel$ unregistrated', 'en'),
(14, 'ccommands', 'overview of Channel Commands', 'en'),
(15, 'ocommands', 'overview of Oper Commands', 'en'),
(16, 'myaccess_number_channels', 'you have access in $channels$ channels', 'EN'),
(17, 'STATS_CHANNEL_COUNT', '$ChannelCount$ channels', 'EN'),
(18, 'STATS_USER_COUNT', '$UserCount$ users', 'EN'),
(19, 'STATS_AUTH_COUNT', '$AuthCount$ auths', 'EN'),
(20, 'STATS_ACCESS_COUNT', '$AccessCount$ access entries', 'EN');

-- --------------------------------------------------------

--
-- Table structure for table `users`
--

CREATE TABLE IF NOT EXISTS `users` (
  `id` int(11) NOT NULL auto_increment,
  `UserUuid` varchar(64) NOT NULL,
  `ChannelUuid` varchar(64) NOT NULL,
  `access` int(11) NOT NULL,
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=705 ;
