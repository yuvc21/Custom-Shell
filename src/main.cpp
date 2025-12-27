#include "zlib_reader/zstr.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

int main(int argc, char *argv[]) {
  // Flush after every std::cout / std::cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  // You can use print statements as follows for debugging, they'll be visible
  // when running tests.
  std::cerr << "Logs from your program will appear here!\n";
  //
  if (argc < 2) {
    std::cerr << "No command provided.\n";
    return EXIT_FAILURE;
  }

  std::string command = argv[1];

  if (command == "init") {
    try {
      std::filesystem::create_directory(".git");
      std::filesystem::create_directory(".git/objects");
      std::filesystem::create_directory(".git/refs");

      std::ofstream headFile(".git/HEAD");
      if (headFile.is_open()) {
        headFile << "ref: refs/heads/main\n";
        headFile.close();
      } else {
        std::cerr << "Failed to create .git/HEAD file.\n";
        return EXIT_FAILURE;
      }

      std::cout << "Initialized git directory\n";
    } catch (const std::filesystem::filesystem_error &e) {
      std::cerr << e.what() << '\n';
      return EXIT_FAILURE;
    }
  } else if (command == "cat-file") {
    if (argc <= 3) {
      std::cerr << "Invalid arguments, required `-p <blob-sha>`\n";
      return EXIT_FAILURE;
    }
    const std::string flag = argv[2];
    if (flag != "-p") {
      std::cerr << "Invalid flag for cat-file, expected `-p`\n";
      return EXIT_FAILURE;
    }
    const std::string value = argv[3];
    const std::string dir_name = value.substr(0, 2);
    const std::string blob_sha = value.substr(2);

    std::string path = ".git/objects/" + dir_name + "/" + blob_sha;
    zstr::ifstream input(path, std::ofstream::binary);
    if (!input.is_open()) {
      std::cerr << "Failed to open object file?\n";
      return EXIT_FAILURE;
    }

    std::string blob_data{std::istreambuf_iterator<char>(input),
                           std::istreambuf_iterator<char>()};
    input.close();
    const auto object_content = blob_data.substr(blob_data.find('\0') + 1);
    std::cout << object_content << std::flush;

  } else if (command == "hash-object"){
    if(argc < 4 || std::string(argv[2]) != "w"){
      std::cerr << "Missing parameter: -w <filename>\n";
      return EXIT_FAILURE;
    }
    auto fileToHashPath = std::filesystem::path(std::string(argv[3]));
    auto fileToHash = std::ifstream(fileToHashPath);
    //to prevent modification
    const auto data = std::string(std::istreambuf_iterator<char>(fileToHash),std::istreambuf_iterator<char>());
   
    const std::string formattedData = "blob " + std::to_string(data.size()) + '\0' + data;
    const auto hash = sha1(formattedData);
    std::cout << hash << std::endl;
    std::filesystem::path dirs = std::filesystem::path(".git") / "objects" / hash.substr(0,2);
    auto filePath = dirs / hash.substr(2);
    std::filesystem::create_directories(dirs);

    std::ofstream ofs(filePath);

    auto compressedSize = compressBound(formattedData.size());
    std::string compressed;
    compressed.resize(compressedSize);

    compress(reinterpret_cast<Bytef*>(&compressed[0]),&compressedSize, reinterpret_cast<const Bytef*>(formattedData.data()), formattedData.size());
    compressed.resize(compressedSize);
    ofs << compressed;

  }
  else {
    std::cerr << "Unknown command " << command << '\n';
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}