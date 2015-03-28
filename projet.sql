-- phpMyAdmin SQL Dump
-- version 3.4.5
-- http://www.phpmyadmin.net
--
-- Host: localhost
-- Generation Time: Apr 26, 2013 at 01:01 
-- Server version: 5.5.16
-- PHP Version: 5.3.8

SET SQL_MODE="NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;

--
-- Database: `projet`
--

-- --------------------------------------------------------

--
-- Table structure for table `droits`
--

CREATE TABLE IF NOT EXISTS `droits` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `droit` varchar(20) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin1 AUTO_INCREMENT=4 ;

--
-- Dumping data for table `droits`
--

INSERT INTO `droits` (`id`, `droit`) VALUES
(1, 'administrateur'),
(2, 'acces_modification'),
(3, 'acces_affichage');

-- --------------------------------------------------------

--
-- Table structure for table `logs`
--

CREATE TABLE IF NOT EXISTS `logs` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `utilisateur` int(11) NOT NULL,
  `login` varchar(15) NOT NULL,
  `date` datetime NOT NULL,
  `action` int(11) NOT NULL,
  `produit` int(11) NOT NULL,
  `quantite` int(11) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;

-- --------------------------------------------------------

--
-- Table structure for table `messagerie`
--

CREATE TABLE IF NOT EXISTS `messagerie` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `ip_addr` int(11) NOT NULL,
  `login` varchar(256) NOT NULL,
  `date_add` datetime NOT NULL,
  `contenu` text NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin1 AUTO_INCREMENT=14 ;

--
-- Dumping data for table `messagerie`
--

INSERT INTO `messagerie` (`id`, `ip_addr`, `login`, `date_add`, `contenu`) VALUES
(1, 0, 'aritz', '2013-03-12 00:00:00', 'est un beau goss'),
(2, 0, 'jerome', '2013-03-12 00:00:00', 'ne l''est pas'),
(3, 0, 'ze', '2013-03-12 00:00:00', 'zez'),
(4, 0, 'dsd', '2013-03-12 00:00:00', 'dsd'),
(5, 0, 'dsds', '2013-03-12 00:00:00', 'dsd'),
(6, 0, 'dsds', '2013-03-12 00:00:00', 'dsds'),
(7, 0, 'dsds', '2013-03-12 00:00:00', 'dsds'),
(8, 0, 'dsds', '2013-03-12 00:00:00', 'dssds'),
(9, 0, 'ff', '2013-03-12 00:00:00', 'ff'),
(10, 0, 'rtr', '2013-03-12 16:35:44', 'trt'),
(11, 0, 'rr', '2013-03-12 16:35:46', 'tt'),
(12, 0, 'ee', '2013-03-12 16:35:48', 'rr'),
(13, 0, 'f', '2013-03-12 17:53:52', 'df');

-- --------------------------------------------------------

--
-- Table structure for table `produit`
--

CREATE TABLE IF NOT EXISTS `produit` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `produit` varchar(30) NOT NULL,
  `type` int(11) NOT NULL,
  `stock` int(11) NOT NULL,
  `ajout_date` datetime NOT NULL,
  `last_update_date` datetime NOT NULL,
  `last_update_user` int(11) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin1 AUTO_INCREMENT=17 ;

--
-- Dumping data for table `produit`
--

INSERT INTO `produit` (`id`, `produit`, `type`, `stock`, `ajout_date`, `last_update_date`, `last_update_user`) VALUES
(9, 'fenetre PVC', 1, 2673893, '2013-03-13 00:12:54', '2013-03-13 01:04:40', 0),
(10, 'Evier', 2, 45, '2013-03-13 00:13:05', '2013-03-13 00:13:05', 0),
(11, 'fils', 3, 2673860, '2013-03-13 00:13:22', '2013-03-13 01:07:09', 0),
(12, 'Etagere', 1, 2673860, '2013-03-13 00:57:15', '2013-03-22 20:31:12', 0),
(13, 'blabla', 2, 45, '2013-03-11 08:26:16', '2013-03-13 00:33:23', 0),
(14, 'Perche', 2, 48, '2013-03-22 15:31:03', '2013-03-22 15:31:03', 0),
(15, 'poudre', 2, 46, '2013-03-22 19:36:26', '2013-03-22 19:36:26', 0),
(16, 'Joint de coude', 2, -7, '2013-03-28 15:31:58', '2013-04-02 16:02:35', 0);

-- --------------------------------------------------------

--
-- Table structure for table `type`
--

CREATE TABLE IF NOT EXISTS `type` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `type` varchar(30) NOT NULL,
  `date_ajout` datetime NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin1 AUTO_INCREMENT=4 ;

--
-- Dumping data for table `type`
--

INSERT INTO `type` (`id`, `type`, `date_ajout`) VALUES
(1, 'menuiserie', '0000-00-00 00:00:00'),
(2, 'plomberie', '0000-00-00 00:00:00'),
(3, 'electricité', '0000-00-00 00:00:00');

-- --------------------------------------------------------

--
-- Table structure for table `utilisateurs`
--

CREATE TABLE IF NOT EXISTS `utilisateurs` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `pseudo` varchar(15) NOT NULL,
  `password` varchar(15) NOT NULL,
  `email` varchar(200) NOT NULL,
  `droits` int(11) NOT NULL,
  `nom` varchar(20) NOT NULL,
  `prenom` varchar(20) NOT NULL,
  `anciennete` year(4) NOT NULL,
  `departement` varchar(50) NOT NULL,
  `poste` varchar(50) NOT NULL,
  `ville` varchar(15) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin1 AUTO_INCREMENT=5 ;

--
-- Dumping data for table `utilisateurs`
--

INSERT INTO `utilisateurs` (`id`, `pseudo`, `password`, `email`, `droits`, `nom`, `prenom`, `anciennete`, `departement`, `poste`, `ville`) VALUES
(2, 'Togrim', 'projet', '', 1, 'Gueret', 'Jérôme', 2011, 'trou perdu', 'feeder en herbe', 'Brouteville'),
(3, 'Mike', 'projet', '', 1, 'Benmergui', 'Michael', 2011, 'Bioengineering', 'Sous fifre', 'Madrid'),
(4, '', '', '', 1, 'Dupond', 'Marc', 1998, 'Research', 'Assistant au touillage du café', 'Lyon');

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
