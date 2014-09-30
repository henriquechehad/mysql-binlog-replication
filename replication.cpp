/*
 *  replication.cpp
 *  MySQL Replication
 *
 *  Created by Henrique Caetano Chehad on 04/12/10.
 *  IPsafe.
 *
 * 					VERSAO 0.1
 *
 * bzr branch lp:mysql-replication-listener
 * g++ replication.cpp -o replication -lreplication -lboost_system -lmysqlclient
 *
 */

#include <string>
#include <iostream>
#include <fstream>

#include <mysql/mysql.h>
#include <mysql/errmsg.h>

#include "binlog_api.h"
#include "res/INIReader.h"
#include "res/INIReader.cpp"
#include "funcoes.cpp"

using std::string;
using mysql::Binary_log;
using mysql::system::create_transport;

MYSQL_ROW row;
MYSQL_RES *result;
int indiceServidores = -1;

class Banco {
public:
	string host;
	string banco;
	string usuario;
	string senha;
	string porta;
	string tabela;

	MYSQL conn;

	string getTabela() const {
		return tabela;
	}

	string getBanco() const {
		return banco;
	}

	string getHost() const {
		return host;
	}

	string getPorta() const {
		return porta;
	}

	string getSenha() const {
		return senha;
	}

	string getUsuario() const {
		return usuario;
	}

	void setDados(string host, string porta, string banco, string usuario,
			string senha) {
		this->host = host;
		this->banco = banco;
		this->usuario = usuario;
		this->senha = senha;
		this->porta = porta;

	}

	void conectar() {
		if (!mysql_init(&this->conn)) {
			std::cout << "ERRO: mysql_init falhou." << std::endl;
			logErro("ERRO: mysql_init falhou.");
		}
		if (!mysql_real_connect(&this->conn, this->host.c_str(),
				this->usuario.c_str(), this->senha.c_str(), this->banco.c_str(),
				atoi(this->porta.c_str()), NULL, 0)) {
			std::cout << mysql_error(&conn) << std::endl;
		} else {
			std::cout << "Conectado ao host: " << this->host << std::endl;
		}

	}
	void checaConexao() {
		int error;
		if (error = mysql_ping(&conn)) {
			switch (error) {
			case CR_SERVER_GONE_ERROR:
			case CR_SERVER_LOST:
				std::cout << "Erro na conexao do servidor: " << this->host
						<< std::endl;
				logErro("Erro na conexao do servidor: ");
				logErro(this->host);
				logErro(mysql_error(&conn));

				break;
			default:
				std::cout << "Erro na conexao do servidor: " << this->host
						<< std::endl;
				std::cout << mysql_error(&conn) << this->host << std::endl;
				logErro("Erro na conexao do servidor: ");
				logErro(this->host);
				logErro(mysql_error(&conn));
			}
			this->conectar();
		}
	}

	void query(string query) {
		if (!mysql_query(&conn, query.c_str())) {
			std::cout << mysql_error(&conn) << std::endl;
			logErro(mysql_error(&conn));
		}
	}

	MYSQL getConn() {
		return this->conn;
	}

	void carregaConf() {
		INIReader reader("replication.conf");
		if (reader.ParseError() < 0) {
			logErro("Nao foi possivel carregar o arquivo replication.conf");
			std::cout
					<< "Nao foi possivel carregar o arquivo replication.conf\n";
		}
		this->host = reader.Get("replication", "host", "UNKNOWN");
		this->porta = reader.Get("replication", "porta", "UNKNOWN");
		this->usuario = reader.Get("replication", "usuario", "UNKNOWN");
		this->senha = reader.Get("replication", "senha", "UNKNOWN");
		this->banco = reader.Get("replication", "banco", "UNKNOWN");
		//this->tabela = reader.Get("replication", "tabela", "UNKNOWN");
	}

	int pesquisaServidores() {
		string sql =
				"SELECT host, porta, banco, usuario, senha FROM servidores.lista;";
		if (mysql_query(&conn, sql.c_str())) {
			std::cout << mysql_error(&conn) << std::endl;
			logErro(mysql_error(&conn));
			return false;
		} else {
			result = mysql_store_result(&conn);
			return true;
		}
	}
};
Banco *banco = new Banco[64];
Banco *connLocal = new Banco[1];

int main(int argc, char** argv) {

	connLocal[0].carregaConf();
	connLocal[0].conectar();
	connLocal[0].pesquisaServidores();

	if (!connLocal[0].pesquisaServidores()) {
		std::cout << "Erro ao pesquisar servidores" << std::endl;
		logErro("Erro ao pesquisar servidores");
		exit(2);
	}
	while (row = mysql_fetch_row(result)) {
		indiceServidores++;
		banco[indiceServidores].setDados(row[0], row[1], row[2], row[3],
				row[4]);
		banco[indiceServidores].conectar();
	}

	char url[128];
	sprintf(url, "mysql://%s@%s:%s", connLocal[0].getUsuario().c_str(),
	//connLocal[0].getSenha().c_str(),
			connLocal[0].getHost().c_str(), connLocal[0].getPorta().c_str());

	std::cout << url << std::endl;
	Binary_log binlog(create_transport(url));
	binlog.connect();

	//std::cout << url << std::endl;
//	Binary_log_event *event;

	while (true) {
		Binary_log_event *event;
		int result = binlog.wait_for_next_event(&event);
		if (result == ERR_EOF)
			break;

		std::cout << "Found event of type " << event->get_event_type()
				<< std::endl;

		//switch (event->header()->type_code) {
		switch (event->get_event_type()) {
		case QUERY_EVENT: {

			const mysql::Query_event *qev =
					static_cast<const mysql::Query_event *>(event);

			//std::cout << "Banco comando: " << qev->db_name << std::endl;

			if (strcmp(qev->db_name.c_str(), "DefaultBD") == 0) { // FILTRO PELO BANCO CONFIGURADO

				//std::cout << static_cast<Query_event*>(event)->query << std::endl;
				//std::cout << "Evento tipo: " << event->get_event_type() << std::endl;

				/*
				 * Remove BEGIN antes de todas as querys
				 */
				std::stringstream query;
				query << qev->query << ";";
				size_t found;
				found = query.str().find("BEGIN");

				if (found) {

					for (int i = 0; i <= indiceServidores; i++) {
						std::cout << query.str() << std::endl;
						banco[i].checaConexao();
						banco[i].query("use DefaultBD;");
						banco[i].query(query.str());
						std::cout << "Executando em servidor: "
								<< banco[i].getHost() << std::endl;
					}
					log(query.str());
				}
			}
		}
			break;
		}
	}
}
