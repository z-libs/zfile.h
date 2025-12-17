#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

// 1. Define implementations
#define ZSTR_IMPLEMENTATION
#include "zstr.h"
#define ZFILE_IMPLEMENTATION
#include "zfile.h"

// 2. Test Helpers
#define TEST(name) printf("[TEST] %-35s", name);
#define PASS() printf(" \033[0;32mPASS\033[0m\n")

void test_path_manipulation(void) 
{
    TEST("Path Join, Ext, Base, Dir");

    // Join
    zstr p1 = zfile_join("folder", "file.txt");
    zfile_normalize(&p1); 
    
    const char *cstr = zstr_cstr(&p1);
    size_t len = strlen(cstr);
    assert(len >= 8 && strcmp(cstr + len - 8, "file.txt") == 0);

    // Ext
    zstr_view ext = zfile_ext("archive.tar.gz");
    assert(zstr_view_eq(ext, ".gz"));
    
    ext = zfile_ext("readme");
    assert(ext.len == 0);

    // Base
    zstr_view base = zfile_base("/var/log/syslog");
    assert(zstr_view_eq(base, "syslog"));

    // Dir
    zstr_view dir = zfile_dir("/var/log/syslog");
    assert(zstr_view_eq(dir, "/var/log"));

    zstr_free(&p1);
    PASS();
}

void test_file_io(void) 
{
    TEST("Write, Read, Append, Size, Remove");

    const char *fname = "test_io.txt";
    const char *data1 = "Hello";
    const char *data2 = " World";

    // Write
    assert(zfile_write_all(fname, data1, 5) == Z_OK);
    assert(zfile_exists(fname));
    assert(zfile_is_file(fname));
    assert(zfile_size(fname) == 5);

    // Append
    assert(zfile_append(fname, data2, 6) == Z_OK);
    assert(zfile_size(fname) == 11);

    // Read
    zstr content = zfile_read_all(fname);
    assert(strcmp(zstr_cstr(&content), "Hello World") == 0);

    // Remove
    assert(zfile_remove(fname) == Z_OK);
    assert(!zfile_exists(fname));

    zstr_free(&content);
    PASS();
}

void test_atomic_save(void) 
{
    TEST("Atomic Save (Temp + Rename)");

    const char *fname = "test_atomic.txt";
    const char *data = "Important Data";

    // Save atomically
    int res = zfile_save_atomic(fname, data, strlen(data));
    assert(res == Z_OK);
    assert(zfile_exists(fname));

    // Verify content
    zstr content = zfile_read_all(fname);
    assert(strcmp(zstr_cstr(&content), data) == 0);

    zfile_remove(fname);
    zstr_free(&content);
    PASS();
}

void test_buffered_reader(void) 
{
    TEST("Buffered Line Reader");

    const char *fname = "test_lines.txt";
    const char *text = "Line 1\nLine 2\r\nLine 3"; 
    zfile_write_all(fname, text, strlen(text));

    int count = 0;
    
    // Test the macro
    ZFILE_FOR_EACH_LINE(fname, line) {
        count++;
        if (count == 1) assert(zstr_view_eq(line, "Line 1"));
        if (count == 2) assert(zstr_view_eq(line, "Line 2"));
        if (count == 3) assert(zstr_view_eq(line, "Line 3"));
    }
    
    assert(count == 3);
    zfile_remove(fname);
    PASS();
}

void test_directories(void) 
{
    TEST("Mkdir Recursive & Iteration");

    const char *root = "test_dir";
    const char *sub = "test_dir/subdir";
    const char *file = "test_dir/file.bin";

    // Recursive Mkdir
    if (zfile_exists(root)) {
        // Simple cleanup attempt (only works if empty, just to be safe)
        // A real robust test would perform recursive delete here.
        // For now we rely on the test cleaning up after itself.
    }
    
    assert(zfile_mkdir_recursive(sub) == Z_OK);
    assert(zfile_is_dir(root));
    assert(zfile_is_dir(sub));

    // Create a dummy file to find
    zfile_write_all(file, "x", 1);

    // Iterate
    zdir_iter *it = zdir_open(root);
    assert(it != NULL);

    zdir_entry entry;
    bool found_file = false;
    bool found_sub = false;

    while (zdir_next(it, &entry)) {
        if (strcmp(entry.name, "file.bin") == 0) {
            // entry.type might be ZDIR_UNKNOWN on some filesystems (lazy stat)
            assert(entry.type == ZDIR_FILE || entry.type == ZDIR_UNKNOWN);
            found_file = true;
        }
        if (strcmp(entry.name, "subdir") == 0) {
            assert(entry.type == ZDIR_DIR || entry.type == ZDIR_UNKNOWN);
            found_sub = true;
        }
    }
    zdir_close(it);

    assert(found_file);
    assert(found_sub);

    // Cleanup
    zfile_remove(file);
    // Note: rmdir logic is platform specific, standard 'remove' works on empty dirs
    remove("test_dir/subdir"); 
    remove("test_dir");
    
    PASS();
}

int main(void) 
{
    printf("=> Running tests (zfile.h, C)\n");
    test_path_manipulation();
    test_file_io();
    test_atomic_save();
    test_buffered_reader();
    test_directories();
    printf("=> All tests passed successfully.\n");
    return 0;
}
