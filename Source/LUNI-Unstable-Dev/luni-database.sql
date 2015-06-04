-- phpMyAdmin SQL Dump
-- version 4.0.9
-- http://www.phpmyadmin.net
--
-- Host: 127.0.0.1
-- Generation Time: May 24, 2015 at 09:17 PM
-- Server version: 5.5.34
-- PHP Version: 5.4.22

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;

--
-- Database: `luni`
--

-- --------------------------------------------------------

--
-- Table structure for table `accounts`
--

CREATE TABLE IF NOT EXISTS `accounts` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(35) COLLATE utf8_unicode_ci NOT NULL,
  `password` varchar(30) COLLATE utf8_unicode_ci NOT NULL,
  `email` varchar(30) COLLATE utf8_unicode_ci NOT NULL,
  `ip` varchar(32) COLLATE utf8_unicode_ci NOT NULL DEFAULT '127.0.0.1',
  `rank` smallint(1) NOT NULL DEFAULT '0',
  `numChars` tinyint(4) NOT NULL,
  `frontChar` tinyint(4) NOT NULL,
  `lastLog` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `activeSub` smallint(1) NOT NULL DEFAULT '0',
  `subTime` int(32) NOT NULL,
  `legoClub` smallint(1) NOT NULL,
  `locked` tinyint(4) NOT NULL,
  `banned` tinyint(4) NOT NULL,
  `loginTries` int(1) NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `name` (`name`)
) ENGINE=MyISAM  DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci AUTO_INCREMENT=12 ;

--
-- Dumping data for table `accounts`
--

INSERT INTO `accounts` (`id`, `name`, `password`, `email`, `ip`, `rank`, `numChars`, `frontChar`, `lastLog`, `activeSub`, `subTime`, `legoClub`, `locked`, `banned`, `loginTries`) VALUES
(10, 'jon002', 'B10niCL3)', '', '127.0.0.1', 0, 1, 0, '2015-02-22 03:58:51', 0, 0, 0, 0, 0, 0),
(9, 'jaller200', 'B10niCL3)', '', '127.0.0.1', 0, 2, 1, '2015-02-22 03:58:46', 0, 0, 0, 0, 0, 0),
(11, 'bobbyjoe', 'bobbyjoe', '', '127.0.0.1', 0, 0, 0, '2015-03-28 00:23:57', 0, 0, 0, 0, 0, 0);

-- --------------------------------------------------------

--
-- Table structure for table `characters`
--

CREATE TABLE IF NOT EXISTS `characters` (
  `id` bigint(20) unsigned NOT NULL,
  `accountID` int(10) unsigned NOT NULL,
  `objectID` bigint(20) NOT NULL,
  `name` varchar(25) COLLATE utf8_unicode_ci NOT NULL,
  `unapprovedName` varchar(66) COLLATE utf8_unicode_ci NOT NULL,
  `nameRejected` tinyint(4) NOT NULL DEFAULT '0',
  `freeToPlay` tinyint(4) NOT NULL DEFAULT '0',
  `shirtColor` int(11) NOT NULL DEFAULT '0',
  `shirtStyle` int(11) NOT NULL DEFAULT '0',
  `pantsColor` int(11) NOT NULL DEFAULT '0',
  `hairStyle` int(11) NOT NULL DEFAULT '0',
  `hairColor` int(11) NOT NULL DEFAULT '0',
  `lh` int(11) NOT NULL DEFAULT '0',
  `rh` int(11) NOT NULL DEFAULT '0',
  `eyebrows` int(11) NOT NULL DEFAULT '0',
  `eyes` int(11) NOT NULL DEFAULT '0',
  `mouth` int(11) NOT NULL DEFAULT '0',
  `headID` int(11) NOT NULL DEFAULT '0',
  `neckID` int(11) NOT NULL DEFAULT '0',
  `shirtID` int(11) NOT NULL DEFAULT '0',
  `pantsID` int(11) NOT NULL DEFAULT '0',
  `leftHandID` int(11) NOT NULL DEFAULT '0',
  `rightHandID` int(11) NOT NULL DEFAULT '0',
  `faction` tinyint(1) NOT NULL DEFAULT '0',
  `worldID` int(11) NOT NULL DEFAULT '0',
  `lastZoneId` int(11) NOT NULL,
  `mapInstance` int(11) NOT NULL,
  `mapClone` int(11) NOT NULL,
  `posX` double NOT NULL DEFAULT '0',
  `posY` double NOT NULL DEFAULT '0',
  `posZ` double NOT NULL DEFAULT '0',
  `friends` varchar(100) COLLATE utf8_unicode_ci NOT NULL DEFAULT '',
  `bestFriends` varchar(50) COLLATE utf8_unicode_ci NOT NULL DEFAULT '',
  `level` int(3) NOT NULL DEFAULT '1',
  `uScore` int(32) NOT NULL DEFAULT '0',
  `mailCount` int(11) NOT NULL,
  `rotX` double NOT NULL,
  `rotY` double NOT NULL,
  `rotZ` double NOT NULL,
  `rotW` double NOT NULL,
  `curHealth` double NOT NULL,
  `curShield` double NOT NULL,
  `curImagination` double NOT NULL,
  `maxHealth` double NOT NULL,
  `maxShield` double NOT NULL,
  `maxImagination` double NOT NULL,
  PRIMARY KEY (`objectID`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Dumping data for table `characters`
--

INSERT INTO `characters` (`id`, `accountID`, `objectID`, `name`, `unapprovedName`, `nameRejected`, `freeToPlay`, `shirtColor`, `shirtStyle`, `pantsColor`, `hairStyle`, `hairColor`, `lh`, `rh`, `eyebrows`, `eyes`, `mouth`, `headID`, `neckID`, `shirtID`, `pantsID`, `leftHandID`, `rightHandID`, `faction`, `worldID`, `lastZoneId`, `mapInstance`, `mapClone`, `posX`, `posY`, `posZ`, `friends`, `bestFriends`, `level`, `uScore`, `mailCount`, `rotX`, `rotY`, `rotZ`, `rotW`, `curHealth`, `curShield`, `curImagination`, `maxHealth`, `maxShield`, `maxImagination`) VALUES
(0, 10, 63450, 'Joe', 'CharmingChillyAlien', 0, 0, 10, 28, 13, 10, 8, 33664016, 33216708, 1, 1, 4, 0, 0, 4382, 2527, 0, 0, 0, 0, 1000, 0, 0, 0, 0, 0, '', '', 1, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0),
(0, 9, 63515, 'Bob', 'ExtraMajesticPigeon', 0, 0, 9, 32, 6, 5, 14, 33664016, 33216708, 1, 3, 4, 0, 0, 4352, 2524, 0, 0, 0, 0, 1000, 0, 0, 0, 0, 0, '', '', 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
(1, 9, 12241, 'JoeTrainer', 'SergeantShaggyDodo', 0, 0, 10, 26, 10, 2, 16, 33664016, 33216708, 8, 10, 25, 0, 0, 4380, 2523, 0, 0, 0, 0, 1000, 0, 0, -27.805, 463.337, 182.939, '', '', 1, 0, 0, 0, 0, 0, 0, 4, 0, 12, 9, 10, 24);

-- --------------------------------------------------------

--
-- Table structure for table `inventory`
--

CREATE TABLE IF NOT EXISTS `inventory` (
  `owner` int(30) unsigned NOT NULL,
  `object` int(30) unsigned NOT NULL,
  `qnt` smallint(1) unsigned NOT NULL DEFAULT '0' COMMENT 'quantity',
  `slot` smallint(3) unsigned NOT NULL COMMENT 'bag slot'
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- --------------------------------------------------------

--
-- Table structure for table `mail`
--

CREATE TABLE IF NOT EXISTS `mail` (
  `mailID` bigint(20) NOT NULL AUTO_INCREMENT,
  `mailAttachment` int(11) NOT NULL,
  `sendTime` bigint(20) NOT NULL,
  `isRead` tinyint(4) NOT NULL,
  `mailSubject` varchar(50) NOT NULL,
  `mailBody` varchar(400) NOT NULL,
  `mailSender` text NOT NULL,
  `mailTo` varchar(84) NOT NULL,
  PRIMARY KEY (`mailID`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin1 AUTO_INCREMENT=2 ;

--
-- Dumping data for table `mail`
--

INSERT INTO `mail` (`mailID`, `mailAttachment`, `sendTime`, `isRead`, `mailSubject`, `mailBody`, `mailSender`, `mailTo`) VALUES
(1, 0, 0, 0, 'Welcome to LUNIServer!', 'Welcome to the LEGO Universe Private Server!', 'Joe', 'Bob');

-- --------------------------------------------------------

--
-- Table structure for table `npcs`
--

CREATE TABLE IF NOT EXISTS `npcs` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(30) COLLATE utf8_unicode_ci NOT NULL,
  `world` int(10) unsigned NOT NULL,
  `x` double NOT NULL,
  `y` double NOT NULL,
  `z` double NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci AUTO_INCREMENT=1 ;

-- --------------------------------------------------------

--
-- Table structure for table `worlds`
--

CREATE TABLE IF NOT EXISTS `worlds` (
  `id` int(50) unsigned NOT NULL AUTO_INCREMENT,
  `zone` tinyint(1) NOT NULL DEFAULT '0' COMMENT 'the zone, 0 avant gardens etc',
  `owner` int(5) NOT NULL,
  `name` varchar(35) COLLATE utf8_unicode_ci NOT NULL,
  `desc` text COLLATE utf8_unicode_ci NOT NULL COMMENT 'description',
  `status` tinyint(1) NOT NULL DEFAULT '0' COMMENT '0 need to be cleaned, 1: private, 2: friends, 3: public',
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci AUTO_INCREMENT=1 ;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
