# CMake generated Testfile for 
# Source directory: /root/Project_LemonLime/unix/test
# Build directory: /root/Project_LemonLime/unix/test
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(watcher_run_c_test "python3" "scripts/run.py")
set_tests_properties(watcher_run_c_test PROPERTIES  _BACKTRACE_TRIPLES "/root/Project_LemonLime/unix/test/CMakeLists.txt;25;add_test;/root/Project_LemonLime/unix/test/CMakeLists.txt;0;")
add_test(watcher_run_sh_test "python3" "scripts/run_sh.py")
set_tests_properties(watcher_run_sh_test PROPERTIES  _BACKTRACE_TRIPLES "/root/Project_LemonLime/unix/test/CMakeLists.txt;26;add_test;/root/Project_LemonLime/unix/test/CMakeLists.txt;0;")
add_test(watcher_MLE_static_test "python3" "scripts/mle_static.py")
set_tests_properties(watcher_MLE_static_test PROPERTIES  _BACKTRACE_TRIPLES "/root/Project_LemonLime/unix/test/CMakeLists.txt;27;add_test;/root/Project_LemonLime/unix/test/CMakeLists.txt;0;")
add_test(watcher_unlimit_memory_test "python3" "scripts/unlimit.py")
set_tests_properties(watcher_unlimit_memory_test PROPERTIES  _BACKTRACE_TRIPLES "/root/Project_LemonLime/unix/test/CMakeLists.txt;28;add_test;/root/Project_LemonLime/unix/test/CMakeLists.txt;0;")
add_test(watcher_TLE_test "python3" "scripts/tle.py")
set_tests_properties(watcher_TLE_test PROPERTIES  _BACKTRACE_TRIPLES "/root/Project_LemonLime/unix/test/CMakeLists.txt;29;add_test;/root/Project_LemonLime/unix/test/CMakeLists.txt;0;")
add_test(watcher_filename_with_space_test "python3" "scripts/space.py")
set_tests_properties(watcher_filename_with_space_test PROPERTIES  _BACKTRACE_TRIPLES "/root/Project_LemonLime/unix/test/CMakeLists.txt;30;add_test;/root/Project_LemonLime/unix/test/CMakeLists.txt;0;")
add_test(watcher_symlink_abs_test "python3" "scripts/symlink_abs.py")
set_tests_properties(watcher_symlink_abs_test PROPERTIES  _BACKTRACE_TRIPLES "/root/Project_LemonLime/unix/test/CMakeLists.txt;31;add_test;/root/Project_LemonLime/unix/test/CMakeLists.txt;0;")
add_test(watcher_symlink_rel_test "python3" "scripts/symlink_rel.py")
set_tests_properties(watcher_symlink_rel_test PROPERTIES  _BACKTRACE_TRIPLES "/root/Project_LemonLime/unix/test/CMakeLists.txt;32;add_test;/root/Project_LemonLime/unix/test/CMakeLists.txt;0;")
add_test(watcher_redirect_IO_test "python3" "scripts/redirect.py")
set_tests_properties(watcher_redirect_IO_test PROPERTIES  _BACKTRACE_TRIPLES "/root/Project_LemonLime/unix/test/CMakeLists.txt;33;add_test;/root/Project_LemonLime/unix/test/CMakeLists.txt;0;")
add_test(watcher_RE_test "python3" "scripts/runtimeerr.py")
set_tests_properties(watcher_RE_test PROPERTIES  _BACKTRACE_TRIPLES "/root/Project_LemonLime/unix/test/CMakeLists.txt;34;add_test;/root/Project_LemonLime/unix/test/CMakeLists.txt;0;")
