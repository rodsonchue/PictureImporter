/*
*	Author: Rodson Chue
*	Last Updated: 01 JUL 2016
*/

#include <curl.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
	return fwrite(ptr, size, nmemb, stream);
}

void downloadAllLinks(std::vector<std::string> urlLinks){
	CURLcode res;
	int counter = 1;
	std::cout << "Downloading all links...\n";
	for (auto eaLink : urlLinks){
		CURL *curl = curl_easy_init();
		FILE* file;
		std::string filename = "links/" + std::to_string(counter) + ".jpg";
		file = fopen(filename.c_str(), "wb");

		std::cout << "#" << counter << "\t" << eaLink << "\n";

		curl_easy_setopt(curl, CURLOPT_URL, eaLink.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);

		/* Perform the request, then check the return code */
		res = curl_easy_perform(curl);

		/* Check for errors */
		if (res != CURLE_OK){
			fprintf(stdout, "\tDownload failed: %s\n",
				curl_easy_strerror(res));
			fclose(file);
		}
		else {
			fseek(file, 0, SEEK_END);
			int size = ftell(file);

			if (size > 0){
				std::cout << "\tDownloaded to " << filename << std::endl;
				fclose(file);
				counter++;
			}
			else
			{
				std::cout << "\tDownloaded file is empty. Skipping over" << std::endl;
				fclose(file);
				remove(filename.c_str());
			}
		}

		//Sanity check in case forgot to close it somewhere
		if (file) fclose(file);

		/* always cleanup */
		curl_easy_cleanup(curl);
	}
}

std::vector<std::string> getUrlLinks(char* url){
	std::vector<std::string> urlLists;
	CURL *curl = curl_easy_init();
	std::string tmpFile = "tmp.txt";
	CURLcode res;
	FILE* file;
	file = fopen(tmpFile.c_str(), "wb");
	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);

	/* Perform the request, res will get the return code */
	res = curl_easy_perform(curl);
	fclose(file);

	/* Check for errors */
	if (res != CURLE_OK){
		fprintf(stderr, "curl_easy_perform() failed: %s\n",
			curl_easy_strerror(res));

		/* always cleanup */
		curl_easy_cleanup(curl);

		return urlLists;
	}

	std::ifstream urlListFile;
	urlListFile.open(tmpFile);
	std::string line;
	while (std::getline(urlListFile, line)){
		if (!line.empty())
			urlLists.push_back(line);
	}

	std::cout << "\nUrl list downloaded successfully\n\n";
	if (std::remove(tmpFile.c_str()) == 0){
		std::cout << "tmp file deleted\n";
	}

	/* always cleanup */
	curl_easy_cleanup(curl);

	return urlLists;
}

int main(int argc, char** argv){
	if (argc < 2){
		std::cerr << "Missing url link argument!\n";
		return -1;
	}

	curl_global_init(CURL_GLOBAL_DEFAULT);

	std::vector<std::string> urlLinks = getUrlLinks(argv[1]);
	downloadAllLinks(urlLinks);
	
	curl_global_cleanup();

	return 0;
}