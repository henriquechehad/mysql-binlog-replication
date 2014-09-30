mysql-binlog-replication
========================


This application is used to replicate to one or more MySQL databases using Binlog lib

Version: 0.1-beta


### Installation


    Install lib requeriments: 
        mysql-replication-listener

    Create and configure slave servers table
        servidores.sql

    Configure the file: 
        replication.conf

    Compile: 
        g++ replication.cpp -o replication -lreplication -lboost_system -lmysqlclient

    Run




Author: Henrique Chehad
