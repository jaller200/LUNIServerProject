CREATE TABLE `accounts` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(35) COLLATE utf8_unicode_ci NOT NULL,
  `password` varchar(30) COLLATE utf8_unicode_ci NOT NULL,
  `email` varchar(30) COLLATE utf8_unicode_ci NOT NULL,
  `ip` varchar(32) COLLATE utf8_unicode_ci NOT NULL DEFAULT '127.0.0.1',
  `rank` smallint(1) NOT NULL DEFAULT '0',
  `lastLog` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `activeSub` smallint(1) NOT NULL DEFAULT '0',
  `subTime` int(32) NOT NULL,
  `legoClub` smallint(1) NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `name` (`name`)
) ENGINE=MyISAM  DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- --------------------------------------------------------

--
-- Struttura della tabella `characters`
--

CREATE TABLE `characters` (
  `id` int(30) unsigned NOT NULL AUTO_INCREMENT,
  `id_acc` int(10) unsigned NOT NULL,
  `name` varchar(25) COLLATE utf8_unicode_ci NOT NULL,
  `faction` tinyint(1) NOT NULL DEFAULT '0',
  `world` int(3) NOT NULL DEFAULT '0',
  `x` double NOT NULL DEFAULT '0',
  `y` double NOT NULL DEFAULT '0',
  `z` double NOT NULL DEFAULT '0',
  `friends` varchar(100) COLLATE utf8_unicode_ci NOT NULL DEFAULT '',
  `bestFriends` varchar(50) COLLATE utf8_unicode_ci NOT NULL DEFAULT '',
  `level` int(3) NOT NULL DEFAULT '1',
  `uScore` int(32) NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`)
) ENGINE=MyISAM  DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- --------------------------------------------------------

--
-- Struttura della tabella `inventory`
--

CREATE TABLE `inventory` (
  `owner` int(30) unsigned NOT NULL,
  `object` int(30) unsigned NOT NULL,
  `qnt` smallint(1) unsigned NOT NULL DEFAULT '0' COMMENT 'quantity',
  `slot` smallint(3) unsigned NOT NULL COMMENT 'bag slot'
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- --------------------------------------------------------

--
-- Struttura della tabella `npcs`
--

CREATE TABLE `npcs` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(30) COLLATE utf8_unicode_ci NOT NULL,
  `world` int(10) unsigned NOT NULL,
  `x` double NOT NULL,
  `y` double NOT NULL,
  `z` double NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- --------------------------------------------------------

--
-- Struttura della tabella `worlds`
--

CREATE TABLE `worlds` (
  `id` int(50) unsigned NOT NULL AUTO_INCREMENT,
  `zone` tinyint(1) NOT NULL DEFAULT '0' COMMENT 'the zone, 0 avant gardens etc',
  `owner` int(5) NOT NULL,
  `name` varchar(35) COLLATE utf8_unicode_ci NOT NULL,
  `desc` text COLLATE utf8_unicode_ci NOT NULL COMMENT 'description',
  `status` tinyint(1) NOT NULL DEFAULT '0' COMMENT '0 need to be cleaned, 1: private, 2: friends, 3: pubblic',
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;