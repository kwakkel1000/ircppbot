
SET SQL_MODE="NO_AUTO_VALUE_ON_ZERO";

--
-- Database: `ircppbot`
--

-- --------------------------------------------------------

--
-- Table structure for table `auth`
--

CREATE TABLE IF NOT EXISTS `auth` (
  `id` int(11) NOT NULL auto_increment,
  `auth` varchar(512) NOT NULL,
  `oaccess` int(11) NOT NULL default '-1',
  `god` varchar(2) NOT NULL default '-1',
  PRIMARY KEY  (`id`,`auth`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=82 ;

-- --------------------------------------------------------

--
-- Table structure for table `cbans`
--

CREATE TABLE IF NOT EXISTS `cbans` (
  `id` int(5) unsigned NOT NULL auto_increment,
  `cid` int(5) unsigned NOT NULL,
  `uid` int(5) unsigned NOT NULL,
  `host` varchar(64) NOT NULL,
  `reason` varchar(128) NOT NULL,
  `from` int(16) unsigned NOT NULL,
  `till` int(16) unsigned NOT NULL,
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=2 ;

-- --------------------------------------------------------

--
-- Table structure for table `channels`
--

CREATE TABLE IF NOT EXISTS `channels` (
  `id` int(11) NOT NULL auto_increment,
  `channel` varchar(512) NOT NULL,
  `giveops` int(11) NOT NULL default '300',
  `givevoice` int(11) NOT NULL default '100',
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=10 ;

-- --------------------------------------------------------

--
-- Table structure for table `commands`
--

CREATE TABLE IF NOT EXISTS `commands` (
  `id` int(2) NOT NULL auto_increment,
  `command` varchar(128) NOT NULL,
  `bind` varchar(128) NOT NULL,
  `caccess` int(5) NOT NULL default '-1',
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=32 ;

-- --------------------------------------------------------

--
-- Table structure for table `ocommands`
--

CREATE TABLE IF NOT EXISTS `ocommands` (
  `id` int(2) NOT NULL auto_increment,
  `command` varchar(128) NOT NULL,
  `bind` varchar(128) NOT NULL,
  `oaccess` int(5) NOT NULL default '1001',
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=19 ;

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
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=16 ;

-- --------------------------------------------------------

--
-- Table structure for table `users`
--

CREATE TABLE IF NOT EXISTS `users` (
  `id` int(11) NOT NULL auto_increment,
  `uid` int(11) NOT NULL,
  `cid` int(11) NOT NULL,
  `access` int(11) NOT NULL,
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=24 ;

