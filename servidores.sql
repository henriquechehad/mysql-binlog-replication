SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0;
SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0;
SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='TRADITIONAL';

CREATE SCHEMA IF NOT EXISTS `servidores` DEFAULT CHARACTER SET latin1 COLLATE latin1_swedish_ci ;
USE `servidores` ;

-- -----------------------------------------------------
-- Table `servidores`.`lista`
-- -----------------------------------------------------
CREATE  TABLE IF NOT EXISTS `servidores`.`lista` (
  `id` INT NOT NULL AUTO_INCREMENT ,
  `host` VARCHAR(120) NULL ,
  `usuario` VARCHAR(45) NULL ,
  `senha` VARCHAR(45) NULL ,
  `porta` VARCHAR(45) NULL ,
  `banco` VARCHAR(45) NULL ,
  PRIMARY KEY (`id`) )
ENGINE = InnoDB;



SET SQL_MODE=@OLD_SQL_MODE;
SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS;
SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS;