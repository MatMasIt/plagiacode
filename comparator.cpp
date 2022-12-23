/**
 * @file comparator.cpp
 * @author Mattia Mascarello (mattia.mascarello@edu.unito.it)
 * @brief Text file comparing utilities
 * @version 0.1
 * @date 2022-12-23
 *
 * @copyright Copyright (c) 2022
 *
 */

/**
 * @brief Extract text from a file stream
 *
 * @param file
 * @return std::string
 */
std::string fileToString(std::ifstream &file)
{
    file.seekg(0, std::ios::end); // seek to end
    size_t size = file.tellg();
    std::string buffer(size, ' '); // allocate buffer
    file.seekg(0);
    file.read(&buffer[0], size); // save in buffer
    return buffer;
}

/**
 * @brief Remove all occurrences of a characrer from the string in-place
 *
 * @param a string
 * @param c character
 */
void removeChar(std::string &a, char c)
{
    std::string::iterator end_pos = std::remove(a.begin(), a.end(), c);
    a.erase(end_pos, a.end());
}

/**
 * @brief filepair - distance   pair
 *
 */
typedef std::pair<std::string, long> fPair;

/**
 * @brief Sort the map of file pairs and and display it
 *
 * @param map filemap to distances
 */
void printSorted(std::map<std::string, long> map) // reverse
{
    std::vector<fPair> vec;

    // copy key-value pairs from the map to the vector
    std::copy(map.begin(),
              map.end(),
              std::back_inserter<std::vector<fPair>>(vec));

    // sort the vector by increasing the order of its pair's second value
    // if the second value is equal, order by the pair's first value
    std::sort(vec.begin(), vec.end(),
              [](const fPair &l, const fPair &r)
              {
                  if (l.second != r.second) // reverse
                  {
                      return l.second > r.second;
                  }

                  return l.first < r.first;
              });

    // print the vector
    for (auto const &pair : vec)
    {
        std::cout << pair.first << ":" << pair.second << std::endl;
    }
}
/**
 * @brief Compare all the entries in the vector and return a map of filepairs
 *
 * @param files list of files
 * @param invisibleIgnore whether to ignore invisible characters
 * @return std::map<std::string, long>  (fileA|fileB) -> distance
 */
std::map<std::string, long> startComparison(std::vector<std::filesystem::directory_entry> files, bool invisibleIgnore)
{
    std::map<std::string, long> map;
    // compare file to file
    for (std::filesystem::directory_entry &fileA : files)
    {
        for (std::filesystem::directory_entry &fileB : files)
        {
            if (fileA != fileB) // yes, the operators are overloaded
            {
                std::string altKey = fileB.path().string() + std::string("|") + fileA.path().string() ;

                if (map.count(altKey)) // distance is symmetric, so fileA|fileB is the same key of fileB|fileA
                {
                    continue;
                }
                // create streams
                std::ifstream streamA(fileA.path());
                std::ifstream streamB(fileB.path());
                // extract content
                std::string a = fileToString(streamA);
                std::string b = fileToString(streamB);
                if (invisibleIgnore)
                {
                    removeChar(a, ' ');
                    removeChar(b, ' ');
                    removeChar(a, '\n');
                    removeChar(b, '\n');
                }
                long distance = LevenshteinDistance<std::string>(a, b); // calculate distances between files
                if (verbose)
                    std::cout << fileA.path() << "|" << fileB.path() << ":" << distance << std::endl; // verbose output

                std::string key = fileA.path().string()  + std::string("|") + fileB.path().string() ; // create A|B key
                map.insert({key, distance});                                                                // apend to map
            }
        }
    }
    return map;
}

/**
 * @brief Scan files in a list of paths and return a list of entries
 *
 * @param filePaths list of paths
 * @param recursive whether to perform recursive scan
 * @return std::vector<std::filesystem::directory_entry> list of entries
 */
std::vector<std::filesystem::directory_entry> scanFiles(std::vector<std::string> filePaths, bool recursive)
{
    std::vector<std::filesystem::directory_entry> files; // list of entries
    for (std::string &path : filePaths)                  // iterate over string paths
    {
        if (!std::filesystem::exists(path)) // file ot not exits
        {
            std::cerr << "E: File \"" << path << "\" does not exist" << std::endl;
            exit(1);
        }
        if (std::filesystem::is_regular_file(path))
        {
            files.push_back(std::filesystem::directory_entry(std::filesystem::path(path))); // add single file
            if (verbose)
                std::cout << "Added " << path << " to list" << std::endl;
        }
        else
        {
            if (recursive) // templating the two iterators in a genric is too much work, i'll just use duplicate code
            {
                // iterate recursively
                for (const std::filesystem::directory_entry &entry : std::filesystem::recursive_directory_iterator(path))
                {
                    if (verbose)
                        std::cout << "Inspecting" << entry.path() << std::endl;
                    if (entry.is_regular_file())
                    {
                        files.push_back(entry);
                        if (verbose)
                            std::cout << "Added " << entry.path() << " to list" << std::endl;
                    }
                }
            }
            else
            {
                // iterate non-recursively
                for (const std::filesystem::directory_entry &entry : std::filesystem::directory_iterator(path))
                {

                    if (verbose)
                        std::cout << "Inspecting" << entry.path() << std::endl;
                    if (entry.is_regular_file())
                    {
                        files.push_back(entry);
                        if (verbose)
                            std::cout << "Added " << entry.path() << " to list" << std::endl;
                    }
                }
            }
        }
    }
    return files;
}
