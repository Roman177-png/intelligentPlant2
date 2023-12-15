-- phpMyAdmin SQL Dump
-- version 5.2.0
-- https://www.phpmyadmin.net/
--
-- Host: 127.0.0.1
-- Generation Time: Dec 15, 2023 at 03:27 AM
-- Server version: 10.4.14-MariaDB
-- PHP Version: 8.1.10

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
START TRANSACTION;
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Database: `csp`
--

-- --------------------------------------------------------

--
-- Table structure for table `plant_data`
--

CREATE TABLE `plant_data` (
  `id` int(11) NOT NULL,
  `moilSoilture` varchar(100) NOT NULL,
  `temperature` varchar(100) NOT NULL,
  `humidity` varchar(100) NOT NULL,
  `pressure` varchar(100) NOT NULL,
  `date` varchar(100) NOT NULL,
  `timeWatering` varchar(100) NOT NULL,
  `name` varchar(100) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Dumping data for table `plant_data`
--

INSERT INTO `plant_data` (`id`, `moilSoilture`, `temperature`, `humidity`, `pressure`, `date`, `timeWatering`, `name`) VALUES
(1, '66', '26.4', '42.2', '993.8', '2023-12-15 03:52:48', '2023-09-12 12:23:12', 'ficus'),
(2, '57', '26.6', '41.4', '994.1', '2023-12-15 03:55:26', '2023-09-12 12:23:12', 'ficus'),
(3, '29', '26.7', '41.4', '994.1', '2023-12-15 03:55:36', '2023-09-12 12:23:12', 'ficus'),
(4, '76', '26.7', '41.3', '994.1', '2023-12-15 03:55:46', '2023-09-12 12:23:12', 'ficus'),
(5, '39', '26.6', '41.2', '994.1', '2023-12-15 03:55:56', '2023-09-12 12:23:12', 'ficus'),
(6, '63', '26.6', '41.2', '994.2', '2023-12-15 03:56:06', '2023-09-12 12:23:12', 'ficus'),
(7, '88', '26.7', '41.1', '994.2', '2023-12-15 03:56:16', '2023-09-12 12:23:12', 'ficus'),
(8, '21', '26.7', '41.1', '994.2', '2023-12-15 03:56:26', '2023-09-12 12:23:12', 'ficus');

--
-- Indexes for dumped tables
--

--
-- Indexes for table `plant_data`
--
ALTER TABLE `plant_data`
  ADD PRIMARY KEY (`id`);

--
-- AUTO_INCREMENT for dumped tables
--

--
-- AUTO_INCREMENT for table `plant_data`
--
ALTER TABLE `plant_data`
  MODIFY `id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=9;
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
