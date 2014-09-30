/*
 * Retorna a data atual
 */
const std::string Data() {
	time_t now = time(0);
	struct tm tstruct;
	char buf[80];
	tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "%Y-%m-%d", &tstruct);
	return buf;
}
/*
 * Escreve no arquivo de log
 */

void log(string query) {
	string arquivo = "logs/" + Data();
	std::ofstream outfile(arquivo.c_str(), std::fstream::app);
	outfile << query << std::endl;
	outfile.close();
}
void logErro(string query) {
	string arquivo = "logs/erros/" + Data();
	std::ofstream outfile(arquivo.c_str(), std::fstream::app);
	outfile << query << std::endl;
	outfile.close();
}
