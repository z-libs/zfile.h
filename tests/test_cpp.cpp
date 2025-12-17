
#include <iostream>
#include <string>
#include <cassert>
#include <vector>
#include <algorithm>

#define ZFILE_IMPLEMENTATION
#include "zfile.h"

#define TEST(name) printf("[TEST] %-40s", name);
#define PASS() std::cout << "\033[0;32mPASS\033[0m\n";

void test_path_wrapper() 
{
    TEST("C++ Path Wrapper (Operators, Exists)");

    z_file::path p("folder");
    
    // Operator '/'.
    z_file::path full = p / "data" / "config.json";
    
    // Convert to string to check.
    std::string s = full.string().c_str();
    
    // Check suffix (separator varies by OS).
    assert(s.find("config.json") != std::string::npos);
    assert(s.find("data") != std::string::npos);

    // Extension.
    auto ext = full.extension();
    
    assert(std::string(ext.data(), ext.size()) == ".json");

    // Existence (should fail).
    assert(!full.exists());

    PASS();
}

void test_directory_iterator() 
{
    TEST("C++ Directory Iterator (Range-based)");

    // Setup: Create a dir with 3 files.
    const char *dir = "cpp_test_dir";
    
    // Ensure clean state.
    if (zfile_exists(dir))
    {
        // Simple cleanup if it exists and is empty/files only.
        zfile_remove("cpp_test_dir/a.txt");
        zfile_remove("cpp_test_dir/b.log");
        zfile_remove("cpp_test_dir/c.bin");
        remove(dir); 
    }

    zfile_mkdir_recursive(dir);
    zfile_write_all("cpp_test_dir/a.txt", "a", 1);
    zfile_write_all("cpp_test_dir/b.log", "b", 1);
    zfile_write_all("cpp_test_dir/c.bin", "c", 1);

    std::vector<std::string> found;

    // Range-based for loop.
    for (auto entry : z_file::dir_iterable(dir)) 
    {
        // Skip '.' and '..'.
        if (entry.name[0] == '.') continue;
        found.push_back(entry.name);
    }

    // Verify.
    assert(found.size() == 3);
    assert(std::find(found.begin(), found.end(), "a.txt") != found.end());
    assert(std::find(found.begin(), found.end(), "b.log") != found.end());
    
    // Cleanup.
    zfile_remove("cpp_test_dir/a.txt");
    zfile_remove("cpp_test_dir/b.log");
    zfile_remove("cpp_test_dir/c.bin");
    remove(dir);

    PASS();
}

int main() 
{
    std::cout << "=> Running tests (zfile.h, C++)\n";
    test_path_wrapper();
    test_directory_iterator();
    std::cout << "=> All tests passed successfully.\n";
    return 0;
}
